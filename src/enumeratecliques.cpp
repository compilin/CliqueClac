//
// Created by Lauréline Nevin <git@compilin.dev> on 12/02/16.
//

#include <iostream>
#include <bitset>
#include <algorithm>
#include <map>
#include <bits/stl_deque.h>
#include "enumeratecliques.h"

namespace enumcliques {
	using namespace fmdm;
	using std::deque;
	using std::vector;
	using std::map;
	using std::shared_ptr;
//	using std::cout;
	using std::endl; // I would just use the namespace at that point but CLion is dumb
	using boost::dynamic_bitset;

	typedef deque<Node *> NodeList;
	typedef shared_ptr<NodeList> NodeListPtr;

	void calculateCliques(Graph &G, Node &node, map<int, vector<vector<Node *>>> &cliqueMap) {
		if (node.type == LEAF)
			return;
		if (node.type == SERIES || node.type == PARALLEL || node.type == PRIME) {
			Child *c = node.fils;
			while (c != nullptr) {
				calculateCliques(G, *c->pointe, cliqueMap);
				c = c->suiv;
			}
			if (node.type == PRIME) {
				cliqueMap[node.id] = move(enumeratePrime(G, node));
			}
		} else
			throw std::runtime_error("Error : unknown node type in enumerateClique");
	}


	CliqueList enumerateCliques(Graph &G, Node &node) {

		deque <NodeListPtr> cliquelist; // List of nodes to replace by their children until it's all LEAF nodes
		CliqueList doneList; // List of cliques only containing LEAF nodes
		map<int, vector<vector < Node * >>> primeMap; // map of Prime node IDs => CliqueList
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
					for (auto c : cliques) {
						auto newList = NodeListPtr(new NodeList(*cur));
						newList->pop_front();
						for (auto cn : c) {
							if (cn->type == LEAF)
								newList->push_back(cn);
							else
								newList->push_front(cn);
						}
						cliquelist.push_back(newList);
					}
					cliquelist.pop_front();
				}
			}
		}


		return doneList;
	}


}