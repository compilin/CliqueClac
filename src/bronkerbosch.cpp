//
// Created by Lauréline Nevin <git@compilin.dev> on 10/02/16.
//


#include <bitset>
#include <map>
#include <iostream>
#include "bronkerbosch.h"

using std::deque;
using std::vector;
using std::map;
using std::cout;
using std::endl;
using std::runtime_error;
using namespace fmdm;
using boost::dynamic_bitset;

/**
 * Bron-Kerbosch implementation
 */
namespace enumcliques {
	typedef dynamic_bitset<unsigned long> NodeSet;

	/**
	 * \private
	 * \brief Sorts adjacency lists by vertex number
	 * \param adj Pointer to the first element of the adjacency list
	 * \see sortAdjacencyLists
	 */
	void sortAdjacencyList(adj *adj) {
		if (adj == nullptr || adj->suiv == nullptr)
			return;

		bool modified;
		do {
			Adj *cur = adj;
			modified = false;
			while (cur->suiv != nullptr) {
				if (cur->s > cur->suiv->s) {
					std::swap(cur->s, cur->suiv->s);
					modified = true;
				}
				cur = cur->suiv;
			}
		} while (modified);
	}

	/**
	 * \private
	 * \brief Sorts all adjacency lists of a graph
	 * \param G the graph to sort
	 * \see getCommonNeighbours
	 *
	 * Unless you are certain the adjacency lists are already sorted in your graph,
	 * This function MUST BE CALLED before any call to enumerateCliques.
	 */
	void sortAdjacencyLists(Graph &G) {
		for (int i = 0; i < G.n; i++)
			sortAdjacencyList(G.G[i]);
	}

	/**
	 * \private
	 * \brief Calculates the list of common neighbours between n1 and n2. Assumes adjacency are sorted
	 * \param dst Pre-allocated destination array that will contain the list of common neighbours
	 * \param n1 first node
	 * \param n2 second node
	 * \returns Size of the array
	 * \see sortAdjacencyLists
	 */
	int intersection(VertexIdType *dst, adj *n1, adj *n2) {
		int nb = 0;
		while (n1 != nullptr && n2 != nullptr) {
			if (n1->s == n2->s) {
				dst[nb++] = n1->s;
				n1 = n1->suiv;
				n2 = n2->suiv;
			} else if (n1->s < n2->s)
				n1 = n1->suiv;
			else
				n2 = n2->suiv;
		}
		return nb;
	}

	NodeSet intersection(NodeSet set1, adj *adj) {
		NodeSet inter(set1.size());
		while (adj != nullptr) {
			inter[adj->s] = set1[adj->s];
			adj = adj->suiv;
		}
		return inter;
	}

	NodeSet subtraction(NodeSet set, adj *adj) {
		NodeSet sub = set;
		while (adj != nullptr) {
			sub[adj->s] = false;
			adj = adj->suiv;
		}
		return sub;
	}

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

	/**
	 * \private
	 * \brief returns the degree of a node in a given subgraph
	 * \param node the node
	 * \param mask a bitset designating the subgraph to consider, so that mask[v] == (v is in subgraph)
	 * \returns The degree of node in the subgraph
	 */
	uint32_t getNodeDegree(adj *node, NodeSet &mask) {
		uint32_t deg = 0;
		while (node != nullptr) {
			if (mask[node->s])
				deg++;
			node = node->suiv;
		}
		return deg;
	}

	/**
	 * \brief Calculates the degeneracy order of a subgraph
	 * \param G the supergraph
	 * \param subg the list of nodes in the subgraphs
	 * \param nb the size of subg
	 * \param mask the bitset mask so that mask[v] == (v is in subgraph)
	 * \returns a newly-allocated array of vertex ids
	 */
	VertexIdType *calculateDegeneracy(Graph G, VertexIdType *subg, size_t nb, const NodeSet &mask) {

		VertexIdType *degeneracy = new VertexIdType[nb];

		{
			auto tmpmask = mask;
			VertexIdType maxNode;
			size_t current = 0;
			unsigned int i;
			do {
				uint32_t maxDeg = 0;
				maxNode = -1;
				for (i = 0; i < nb; i++)
					if (tmpmask[subg[i]]) {
						uint32_t deg = getNodeDegree(G.G[subg[i]], tmpmask);
						if (deg >= maxDeg) {
							maxDeg = deg;
							maxNode = subg[i];
						}
					}
				if (maxNode < 0)
					throw runtime_error("Error while calculating degeneracy : Ran out of nodes to process before nb");
				degeneracy[current++] = maxNode;
				tmpmask[maxNode] = false;
			} while (current < nb);
		}
		return degeneracy;
	}

	void bronKarboschRecur(Graph G, NodeSet R, NodeSet P, NodeSet X, CliqueList &list) {
		if (!P.any() && !X.any()) {
			size_t n = R.count();
			Clique clique(n);
			size_t v = R.find_first();
			int i = 0;
			while (v != NodeSet::npos) {
				clique[i++] = (VertexIdType) v;
				v = R.find_next((size_t) v);
			}
			list.push_back(clique);
			return;
		}
		VertexIdType u = (VertexIdType) std::min(P.find_first(), X.find_first());
		auto PsubN = subtraction(P, G.G[u]);
		for (size_t v = (VertexIdType) PsubN.find_first(); v != NodeSet::npos; v = PsubN.find_next(v)) {
			R[v] = true;
			bronKarboschRecur(G, R, intersection(P, G.G[v]), intersection(X, G.G[v]), list);
			R[v] = false;
			P[v] = false;
			X[v] = true;
		}
	}

/**
 * \brief Returns a list of cliques in the sugraph of the children of the given (supposedly prime) node
 * \param G the graph to search through
 * \param node a node in the modular decomposition tree of G
 * \returns a CliqueList containing all maximal cliques
 */
	vector<vector<Node *>> enumeratePrime(Graph &G, Node &node) {
		size_t nb, i;
		child *c;
		for (c = node.fils, i = 0; c != nullptr; c = c->suiv)
			i++;
		nb = i;

		VertexIdType *subg = new VertexIdType[nb];
		map<VertexIdType, Node *> map;

		auto mask = dynamic_bitset<unsigned long>((unsigned long) G.n);

		for (i = 0, c = node.fils; c != nullptr; c = c->suiv, i++) {
			subg[i] = getAnyLeaf(c->pointe);
			map[subg[i]] = c->pointe;
		}

		for (i = 0; i < nb; i++)
			mask[subg[i]] = true;

		VertexIdType *degeneracy = calculateDegeneracy(G, subg, nb, mask);

		NodeSet P = mask, // All vertices in subraph
				X((unsigned long) G.n, 0UL), R((unsigned long) G.n, 0UL); // No vertices
		CliqueList lst;

		for (i = 0; i < nb; i++) {
			VertexIdType v = degeneracy[i];
			R[v] = true;
			bronKarboschRecur(G, R, intersection(P, G.G[v]), intersection(X, G.G[v]), lst);
			R[v] = false;

			P[v] = false;
			X[v] = true;
		}

		vector<vector<Node *>> retList(lst.size());
		i = 0;
		for (Clique cli : lst) {
			vector<Node *> clique(cli.size());
			size_t j = 0;
			for (VertexIdType id : cli) {
				clique[j++] = map.at(id);
			}
			retList[i++] = clique;
		}

		delete[] degeneracy;
		delete[] subg;

		return retList;
	}
}