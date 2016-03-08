//
// Created by Lauréline Nevin <Lauréline Nevin> on 19/02/16.
//

#ifndef CLIQUECLAC_WRITEGRAPH_H
#define CLIQUECLAC_WRITEGRAPH_H

#include <fstream>
#include <map>
#include <memory>

#include "moddecomp.hpp"
#include "fmdm/dm_english.h"
#include "quick-cliques/degeneracy_algorithm.h"

namespace cliqueclac {

	class GraphAttributes {
	public:
		GraphAttributes() {
			graph = std::map<std::string, std::string>();
			edges = std::map<std::string, std::string>();
			nodes = std::map<std::string, std::string>();
			nodesPerType = std::map<fmdm::NodeType, std::map<std::string, std::string>>();
			for (fmdm::NodeType n : {fmdm::LEAF, fmdm::PRIME, fmdm::PARALLEL, fmdm::SERIES}) {
				nodesPerType[n] = std::map<std::string, std::string>();
			}
		}

		std::map<std::string, std::string> graph;
		std::map<std::string, std::string> edges;
		std::map<std::string, std::string> nodes;
		std::map<fmdm::NodeType, std::map<std::string, std::string>> nodesPerType;
	};

	class AdjacencyIterator {
	public:
		virtual void reset() = 0;

		virtual fmdm::VertexIdType next() = 0;

		virtual fmdm::VertexIdType current() = 0;

	};

	class GraphIterator {
	public:
		virtual ~GraphIterator() { }

		virtual void reset() = 0;

		virtual fmdm::VertexIdType next() = 0;

		virtual fmdm::VertexIdType current() = 0;

		virtual AdjacencyIterator *getAdjacency(fmdm::VertexIdType id) = 0;

		virtual fmdm::NodeType getType(fmdm::VertexIdType) {
			return fmdm::NodeType::UNKN; // Only applies to MDTreeIterator, others return UNKN
		}

		virtual char *label(char *dst, fmdm::VertexIdType) = 0;

		virtual bool isDirected() = 0;
	};

	void writeGraph(std::shared_ptr<GraphIterator> G, std::ostream &out, std::string format,
	                GraphAttributes *attr = nullptr, std::string title = "");

	void writeGraph(std::shared_ptr<GraphIterator> G, std::string outname, std::string format,
	                GraphAttributes *attr = nullptr, std::string title = "");

	std::shared_ptr<GraphIterator> getIterator(fmdm::Node *root, quickcliques::labelFunction label);

	//GraphIterator *getIterator(fmdm::Graph &graph);

	//std::shared_ptr<GraphIterator> getIterator(moddecomp::MDTreeNode &root, quickcliques::labelFunction label);

	std::shared_ptr<GraphIterator> getIterator(quickcliques::Graph & graph);
}

#endif //CLIQUECLAC_WRITEGRAPH_H
