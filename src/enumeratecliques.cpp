//
// Created by Lauréline Nevin <git@compilin.dev> on 12/02/16.
//

#include "writegraph.hpp"
#include "enumeratecliques.hpp"

namespace cliqueclac {
	using namespace fmdm;
	using namespace std;
	using boost::dynamic_bitset;
	namespace qc=quickcliques;
	using qc::LinkedList;

	typedef deque<Node *> NodeList;
	typedef shared_ptr<NodeList> NodeListPtr;

	size_t getNodeSubgraph(LinkedList **adj, Node node, LinkedList **subadj, map<VertexIdType, Node *> &subgToNode);


	/**
	 * \private
	 * \brief Gets a leaf node under n
	 * \param n Node to get a vertex for
	 * \returns The vertex id of the first LEAF node found under n
	 *
	 * This function will take the first child of n recursively until n is a LEAF node,
	 * then returns the associated vertex id
	 */
	VertexIdType getAnyLeaf(Node *n) {
		while (n != nullptr && n->type != LEAF && (n->type == PARALLEL || n->type == SERIES || n->type == PRIME))
			n = n->fils->pointe;
		if (n == nullptr)
			throw runtime_error("Error : Encountered nullptr in getAnyLeaf");
		if (n->type == LEAF)
			return n->nom;
		if (n->type != PARALLEL && n->type != SERIES && n->type != PRIME)
			throw runtime_error("Error : Unknown NodeType encountered in getAnyLeaf");
		throw runtime_error("Unknown error in getAnyLeaf");
	}


	void calculateCliques(const qc::Graph &G, Node &node, map<int, LinkedList *> &cliqueMap, LinkedList **subadj) {
		bool firstCall = false;
		if (subadj == nullptr) {
			firstCall = true;
			subadj = new LinkedList *[G.n];
		}
		map<VertexIdType, Node *> leafToNode;
		if (node.type == LEAF)
			return;
		if (node.type == SERIES || node.type == PARALLEL || node.type == PRIME) {
			Child *c = node.fils;
			while (c != nullptr) {
				calculateCliques(G, *c->pointe, cliqueMap, subadj);
				c = c->suiv;
			}
			if (node.type == PRIME) {
				LinkedList *list = qc::createLinkedList();
				auto nb = getNodeSubgraph(G.adj, node, subadj, leafToNode);
				if (nb > INT_MAX)
					throw runtime_error("Node has too many child for quickcliques to handle (i.e > INT_MAX)");
#ifdef PRINTGRAPHS
					char fname[32];
					qc::Graph subg{(int) nb, subadj,
					               (function<const char *(VertexIdType)>) [&leafToNode, &G](VertexIdType id) {
						               return G.labels(leafToNode.at(id)->id);
					               }};
					sprintf(fname, "subg_%d.gv", node.id);
					writeGraph(getIterator(subg), fname, "GV");
					cout << "Wrote virtual graph for node "<< node.id <<" to "<< fname<<endl;
#endif
				qc::listAllMaximalCliquesDegeneracy(subadj, nullptr, list, nullptr, (int) nb);
				//cout << "Found " << qc::length(list) << " cliques for Prime node " << node.id << endl;

				qc::Link *el = list->head->next;
				while (!qc::isTail(el)) {
					int *dat = (int *) el->data;
					size_t n = 0;
					for (int i = 0; dat[i] > -1; i++)
						n++;
					Node **nodes = new Node *[n + 1];
					for (size_t i = 0; i < n; i++)
						nodes[i] = leafToNode[dat[i]];
					nodes[n] = nullptr;

					el->data = nodes;
					free(dat);
					el = el->next;
				}

				cliqueMap[node.id] = list;
				for (size_t i = 0; i < nb; i++) {
					qc::destroyLinkedList(subadj[i]);
					subadj[i] = nullptr;
				}
			}
		} else
			throw std::runtime_error("Error : unknown node type in enumerateClique");
		if (firstCall) {
			delete[] subadj;
		}
	}


	CliqueList enumerateCliques(const qc::Graph &G, Node &node) {

		deque<NodeListPtr> cliquelist; // List of nodes to replace by their children until it's all LEAF nodes
		CliqueList doneList; // List of cliques only containing LEAF nodes
		map<int, LinkedList *> primeMap; // map of Prime node IDs => list of cliques
#ifdef PRINTGRAPHS
			writeGraph(getIterator(qcG), "graph.graphml", "GV");
#endif
		calculateCliques(G, node, primeMap, nullptr);
//		cout << "calculated "<< primeMap.size() << " Prime nodes' cliques" << endl;
		cliquelist.push_back(NodeListPtr(new NodeList()));
		cliquelist.front()->push_back(&node);

		while (!cliquelist.empty()) {
			//cout << doneList.size() << " cliques done, "<< cliquelist.size() << " to process" << endl;
			NodeList *cur = &*cliquelist.front();
			if (cur->front()->type == LEAF) { // Leafs are at the end, thus this is a leaf-only list
//				cout << "Clique done : ";
//				printNodes(cur->begin(), cur->end(), G.labels);
				Clique c(cur->size());
				int i;
				deque<Node *>::iterator it;
				for (i = 0, it = cur->begin(); it != cur->end(); it++, i++)
					c[i] = (*it)->nom;
//				cout << endl;
				doneList.push_back(c);
				cliquelist.pop_front();
			} else {
				auto n = cur->front();
				auto child = n->fils;
				if (child == nullptr)
					throw std::runtime_error("Null first child!");
				if (n->type == SERIES) {
//					cout << "SERIES" << endl;
//					printNodes(cur->begin(), cur->end(), G.labels);
					cur->pop_front();
					while (child != nullptr) {
//						cout << "Child : "<< child->pointe->id <<" : " << child->pointe->type << endl;
						if (child->pointe->type == LEAF) {
							cur->push_back(child->pointe);
						} else
							cur->push_front(child->pointe);
						child = child->suiv;
					}
//					printNodes(cur->begin(), cur->end(), G.labels);
				} else if (n->type == PARALLEL) {
//					cout << "PARALLEL" << endl;
//					printNodes(cur->begin(), cur->end(), G.labels);
					while (child != nullptr) {
						auto newList = NodeListPtr(new NodeList(*cur));
						newList->pop_front();
						if (child->pointe->type == LEAF)
							newList->push_back(child->pointe);
						else
							newList->push_front(child->pointe);
						cliquelist.push_back(newList);
						child = child->suiv;
//						printNodes(newList->begin(), newList->end(), G.labels);
					}
					cliquelist.pop_front();
				} else if (n->type == PRIME) {
//					cout << "PRIME" << endl;
					auto cliques = primeMap.at(n->id);
					for (auto c = cliques->head->next; !qc::isTail(c); c = c->next) {
						auto newList = NodeListPtr(new NodeList(*cur));
						newList->pop_front();
						Node **dat = (Node **) c->data;
						for (size_t i = 0; dat[i] != nullptr; i++) {
							if (dat[i]->type == LEAF)
								newList->push_back(dat[i]);
							else
								newList->push_front(dat[i]);
						}
						cliquelist.push_back(newList);
					}
					cliquelist.pop_front();
				}
			}
		}


		return doneList;
	}

	CliqueList enumerateCliques(const qc::Graph &G) {
		auto list = qc::createLinkedList();
		qc::listAllMaximalCliquesDegeneracy(G.adj, nullptr, list, nullptr, G.n);
		CliqueList clist;
		clist.reserve((unsigned long) G.n);
		for (auto l = list->head->next; !qc::isTail(l); l = l->next) {
			Clique cli;
			int *dat = (int *) l->data;
			for (int i = 0; dat[i] >= 0; i++)
				cli.push_back(dat[i]);
			clist.push_back(cli);
		}
		return clist;
	}

	/**
	 * \brief Converts a Graph structure from fm-dm to quick-clique Adjacency list format
	 * \param input the Input graph
	 * \returns a newly-allocated array of LinkedList's containing the graph's adjacency lists
	 */
	qc::Graph fmdmGraphToStrash(const Graph &input) {
		LinkedList **adj = new LinkedList *[input.n];
		qc::Graph G{(int) input.n, adj, input.labels};

		for (size_t i = 0; i < (size_t) input.n; i++) {
			adj[i] = qc::createLinkedList();
			Adj *adjl = input.G[i];
			while (adjl != nullptr) {
				addLast(adj[i], (void *) adjl->s); // Yeah that's what degeneracy_algorithm.c expects :/
				adjl = adjl->suiv;
			}
		}

		return G;
	}

	/**
	 * \brief Creates a subgraph from a graph's ajacency lists and a node of its modular decomposition
	 * \param adj the array of adjacency lists
	 * \param node the MD node
	 * \param subadj the *previously-allocated* destination array for the subgraph
	 * \return the number of nodes in the subgraph
	 *
	 * Note that no data is copied, only pointers, so that LinkedList's in subadj are the same than the ones in adj for
	 * corresponding nodes
	 */
	size_t getNodeSubgraph(LinkedList **adj, Node node, LinkedList **subadj, map<VertexIdType, Node *> &subgToNode) {
		subgToNode.clear();
		auto ch = node.fils;
		map<VertexIdType, VertexIdType> leafToSubg;
		map<int, VertexIdType> nodeToLeaf;
		size_t n = 0, nb;
		while (ch != nullptr) {
			VertexIdType id = getAnyLeaf(ch->pointe);
			subgToNode[n] = ch->pointe;
			nodeToLeaf[ch->pointe->id] = id;
			leafToSubg[id] = n;
			n++;
			ch = ch->suiv;
		}
		nb = n;

		for (n = 0; n < nb; n++) {

			//auto node = leafToNode[n];
			auto leaf = nodeToLeaf[subgToNode[n]->id];
			auto nadj = subadj[n] = qc::createLinkedList();
			qc::Link *neigh;
			for (neigh = adj[leaf]->head->next; !qc::isTail(neigh); neigh = neigh->next) {
				auto it = leafToSubg.find((VertexIdType&) neigh->data);
				if (it != leafToSubg.end())
					qc::addLast(nadj, (void *) it->second);
			}
		}

		return n;
	}

	void sortCliqueList(CliqueList &list) {
		for (auto &cli : list) {
			sort(cli.begin(), cli.end());
		}
		sort(list.begin(), list.end(), [](const Clique &c1, const Clique &c2) {
			for (size_t i = 0; i < c1.size() && i < c2.size(); i++) {
				if (c1[i] < c2[i])
					return true;
				if (c1[i] > c2[i])
					return false;
				// if == continue
			}
			// If no element is different until min(c1.size(), c2.size()), return whether c1 is smaller than c2
			return (c1.size() < c2.size());
		});
	}
}