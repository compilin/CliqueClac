//
// Created by Lauréline Nevin <git@compilin.dev> on 12/02/16.
//

#include "enumeratecliques.hpp"

namespace enumcliques {
	using namespace fmdm;
	using namespace std;
	using boost::dynamic_bitset;
	namespace qc=quickcliques;
	using qc::LinkedList;

	typedef deque<Node *> NodeList;
	typedef shared_ptr<NodeList> NodeListPtr;

	size_t getNodeSubgraph(LinkedList **adj, Node node, LinkedList **subadj, map<VertexIdType, Node*> &leafToNode);


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


	void calculateCliques(Graph &G, Node &node, map<int, LinkedList*> &cliqueMap) {
		auto adj = fmdmGraphToStrash(G);
		LinkedList **subadj = new LinkedList*[G.n];
		map<VertexIdType, Node*> leafToNode;
		if (node.type == LEAF)
			return;
		if (node.type == SERIES || node.type == PARALLEL || node.type == PRIME) {
			Child *c = node.fils;
			while (c != nullptr) {
				calculateCliques(G, *c->pointe, cliqueMap);
				c = c->suiv;
			}
			if (node.type == PRIME) {
				//cliqueMap[node.id] = move(enumeratePrime(G, node));
				LinkedList * list = qc::createLinkedList();
				auto nb = getNodeSubgraph(adj, node, subadj, leafToNode);
				if (nb > INT_MAX)
					throw runtime_error("Node has too many child for quickcliques to handle (i.e > INT_MAX)");
				qc::listAllMaximalCliquesDegeneracy(adj, nullptr, list, nullptr, (int) nb);

				qc::Link *el = list->head->next;
				while (!qc::isTail(el)) {
					int *dat = (int *) el->data;
					size_t n = 0;
					for (int i = 0; dat[i] > -1; i ++)
						n ++;
					Node **nodes = new Node*[n+1];
					for (size_t i = 0; i < n; i ++)
						nodes[i] = leafToNode[dat[i]];
					nodes[n] = nullptr;

					el->data = nodes;
					delete[] dat;
				}

				cliqueMap[node.id] = list;
			}
		} else
			throw std::runtime_error("Error : unknown node type in enumerateClique");
	}


	CliqueList enumerateCliques(Graph &G, Node &node) {

		deque <NodeListPtr> cliquelist; // List of nodes to replace by their children until it's all LEAF nodes
		CliqueList doneList; // List of cliques only containing LEAF nodes
		map<int, LinkedList*> primeMap; // map of Prime node IDs => list of cliques
		calculateCliques(G, node, primeMap);
//		cout << "calculated "<< primeMap.size() << " Prime nodes' cliques" << endl;
		cliquelist.push_back(NodeListPtr(new NodeList()));
		cliquelist.front()->push_back(&node);

		while (!cliquelist.empty()) {
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
						for (size_t i = 0; dat[i] != nullptr; i ++) {
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

	/**
	 * \brief Converts a Graph structure from fm-dm to quick-clique Adjacency list format
	 * \param input the Input graph
	 * \returns a newly-allocated array of LinkedList's containing the graph's adjacency lists
	 */
	LinkedList **fmdmGraphToStrash(Graph &input) {
		LinkedList **adj = new LinkedList*[input.n];

		for (size_t i = 0; i < (size_t)input.n; i ++) {
			adj[i] = qc::createLinkedList();
			Adj * adjl = input.G[i];
			while (adjl != nullptr) {
				addLast(adj[i], (void*)adjl->s); // Yeah that's what degeneracy_algorithm.c expects :/
				adjl = adjl->suiv;
			}
		}

		return adj;
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
	size_t getNodeSubgraph(LinkedList **adj, Node node, LinkedList **subadj, map<VertexIdType, Node*> &leafToNode) {
		leafToNode.clear();
		auto ch = node.fils;
		size_t n = 0;
		while (ch != nullptr) {
			VertexIdType id = getAnyLeaf(ch->pointe);
			leafToNode[id] = ch->pointe;
			subadj[n] = adj[id];
			n ++;
			ch = ch->suiv;
		}

		return n;
	}

}