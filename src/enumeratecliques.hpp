//
// Created by Lauréline Nevin <git@compilin.dev> on 10/02/16.
//

#ifndef SIMPLECLIQUES_ENUMERATECLIQUES_H
#define SIMPLECLIQUES_ENUMERATECLIQUES_H
#include <iostream>
#include <map>
#include <deque>
#include "quick-cliques/degeneracy_algorithm.hpp"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include "fmdm/dm.hpp"

namespace cliqueclac {
	/**
	 * \struct Clique
	 * \brief Defines a clique.
	 */
	typedef std::vector<fmdm::VertexIdType> Clique;

	/**
	 * \struct CliqueList
	 * \brief Defines a list of cliques. Listed cliques are all allocated inside the data array pointed by data,
	 * for easy freeing of memory.
	 */
	typedef std::vector<Clique> CliqueList;


	CliqueList enumerateCliques(const fmdm::Graph &G, fmdm::Node &node);

	CliqueList enumerateCliques(const fmdm::Graph &G);

	CliqueList enumerateCliques(const quickcliques::Graph &G);

	quickcliques::Graph fmdmGraphToStrash(const fmdm::Graph &input);
	fmdm::VertexIdType getAnyLeaf(fmdm::Node *n);

	void sortCliqueList(CliqueList &list);

	template<typename T>
	void printNodes(T itIn, T itOut, fmdm::labelFunction labels) {
		std::cout << "[";
		for (auto it = itIn; it != itOut; it ++) {
			if ((*it)->type == fmdm::LEAF)
				std::cout << (it == itIn ? "" : ", ") << labels(getAnyLeaf(*it));
			else
				std::cout << (it == itIn ? "" : ", ") << (*it)->id;
		}
		std::cout << "]" << std::endl;
	}

	template <typename T>
	void printNodeList(T itIn, T itOut, fmdm::labelFunction labels) {
		for (auto it = itIn; it != itOut; it ++)
			printNodes(it->begin(), it->end(), labels);
	}
}


#endif //SIMPLECLIQUES_ENUMERATECLIQUES_H
