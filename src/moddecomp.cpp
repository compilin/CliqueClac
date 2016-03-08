//
// Created by Lauréline Nevin <Lauréline Nevin> on 29/02/16.
//

#include <iostream>
#include "moddecomp.hpp"

using namespace std;
using fmdm::VertexIdType;
using fmdm::Graph;

namespace moddecomp {

	shared_ptr<DecompTreeList> modularDecomposition(Graph G, NodeMask mask, NodeMask &subgraph) {
		VertexIdType pivot = (VertexIdType) mask.find_first();
		NodeMask neigh = subgraph;
		neigh[pivot] = false;
		NodeMask nonneight = subgraph - neigh;
		DecompTreeList dtl{pivot, nullptr, nullptr};
	}

	shared_ptr<DecompTreeList> modularDecomposition(Graph G, NodeMask mask) {

	}

	bool compareDMTrees(const fmdm::Node &, const fmdm::Node &) {
		// TODO
		throw runtime_error("Unimplemented");
	}
}
