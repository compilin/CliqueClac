//
// Created by Lauréline Nevin <Lauréline Nevin> on 19/02/16.
//

#include <iostream>

#include "writegraph.hpp"


namespace cliqueclac {
	using namespace std;
	using fmdm::VertexIdType;
	using fmdm::NodeType;
	namespace md = moddecomp;
	namespace qc = quickcliques;


	char *MDNodeLabel(char *dst, VertexIdType id, VertexIdType leafID, NodeType type, qc::labelFunction labelf) {
		switch (type) {
			case NodeType::LEAF: {
				const char *lab = labelf(leafID);
				sprintf(dst, "%s", lab);
				break;
			}
			case NodeType::PRIME:
				sprintf(dst, "Prime %ld", id);
				break;
			case NodeType::PARALLEL:
				sprintf(dst, "&#8741;%ld", id);
				break;
			case NodeType::SERIES:
				sprintf(dst, "&#8864;%ld", id);
				break;
			default:
				cerr << "Error : unknown node type " << type << endl;
				sprintf(dst, "ERROR %ld", id);
		}
		return dst;
	}

	class FMDMMDTreeAdjIterator : public AdjacencyIterator {
		friend class FMDMMDTreeIterator;

	public:
		void reset() { curr = first; }

		VertexIdType current() { return curr != nullptr ? curr->pointe->id : -1; }

		VertexIdType next() {
			VertexIdType ret = current();
			if (curr != nullptr)
				curr = curr->suiv;
			return ret;
		}

	private:
		FMDMMDTreeAdjIterator(fmdm::Node *node) { curr = first = node->fils; }

		fmdm::child *first, *curr;
	};

	class FMDMMDTreeIterator : public GraphIterator {
	public:
		FMDMMDTreeIterator(fmdm::Node *root, qc::labelFunction labelf) {
			this->root = root;
			this->labelf = labelf;
			reset();
		}

		~FMDMMDTreeIterator() { };

		void reset() {
			curr = root;
			while (curr->type != NodeType::LEAF)
				curr = curr->fils->pointe;
			nodes[curr->id] = curr;
		}

		VertexIdType next() {
			VertexIdType cur = current();
			if (curr != nullptr) {
				if (curr->pere == nullptr) // Root
					curr = nullptr;
				else if (curr->fpere->suiv == nullptr) // Last sibling -> backtrack
					curr = curr->pere;
				else {// Next sibling
					curr = curr->fpere->suiv->pointe;
					while (curr->type != NodeType::LEAF)
						curr = curr->fils->pointe;
				}
				if (curr != nullptr) {
					nodes[curr->id] = curr;
				}
			}
			return cur;
		}

		VertexIdType current() {
			return curr == nullptr ? -1 : curr->id;
		}

		char *label(char *dst, VertexIdType id) {
			auto it = nodes.find(id);
			if (it != nodes.end()) {
				dst = MDNodeLabel(dst, id, it->second->nom, it->second->type, labelf);
			} else
				throw runtime_error("Node id not found");
			return dst;
		}

		FMDMMDTreeAdjIterator *getAdjacency(VertexIdType id) { return new FMDMMDTreeAdjIterator(nodes.at(id)); }

		NodeType getType(VertexIdType id) {
			return nodes.at(id)->type;
		}

		bool isDirected() { return true; }

	private:
		map<VertexIdType, fmdm::Node *> nodes;
		fmdm::Node *root;
		fmdm::Node *curr;
		quickcliques::labelFunction labelf;
	};

	class QCGraphAdjIterator : public AdjacencyIterator {
		friend class QCGraphIterator;

	public:
		void reset() {
			curr = adj.head->next;
		}

		VertexIdType next() {
			VertexIdType cur = current();
			if (!qc::isTail(curr))
				curr = curr->next;
			return cur;
		}

		VertexIdType current() {
			return (qc::isTail(curr) ? -1 : (VertexIdType) curr->data);
		}

	private:
		QCGraphAdjIterator(qc::LinkedList &pList) : adj(pList) {
			reset();
		}

		const qc::LinkedList &adj;
		qc::Link *curr;
	};

	class QCGraphIterator : public GraphIterator {
	public:
		QCGraphIterator(quickcliques::Graph &graph) : graph(graph) {
			reset();
		}

		~QCGraphIterator() { };

		void reset() {
			curr = 0;
		}

		VertexIdType next() {
			VertexIdType cur = current();
			curr++;
			return cur;
		}

		VertexIdType current() {
			return curr < graph.n ? curr : -1;
		}

		QCGraphAdjIterator *getAdjacency(VertexIdType id) {
			return new QCGraphAdjIterator(*graph.adj[id]);
		}

		char *label(char *dst, VertexIdType id) {
			auto lab = graph.labels(id);
			sprintf(dst, "%s", lab);
			return dst;
		}

		bool isDirected() {
			return false;
		}

	private:
		const qc::Graph &graph;
		VertexIdType curr;
	};

/*
	class MDTreeAdjIterator : public AdjacencyIterator {
	public:
	private:

	};

	class MDTreeIterator : public GraphIterator {
	public:
		MDTreeIterator(md::MDTreeNode &root, qc::labelFunction labelf) : root(root), labelf(labelf) { reset(); };
		void reset() override {

		}
	private:
		map<VertexIdType, md::MDTreeNode*> nodes;
		md::MDTreeNode &root;
		qc::labelFunction labelf;
	};
*/

	void writeGraphGML(ostream &out, GraphIterator &G) {
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
				"<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\">\n"
				"<key id=\"d1\" for=\"node\" attr.name=\"label\" attr.type=\"string\"/>\n"
				"<graph id=\"G\" edgedefault=\"" << (G.isDirected() ? "directed" : "undirected") << "\">" << endl;

		char *label = new char[64];
		for (VertexIdType cur = G.next(); cur >= 0; cur = G.next()) {
			out << "\t<node id=\"" << cur << "\"><data key=\"d1\">" << G.label(label, cur) << "</data></node>" << endl;
		}
		G.reset();
		for (VertexIdType cur = G.next(); cur >= 0; cur = G.next()) {
			auto adj = G.getAdjacency(cur);
			for (VertexIdType n = adj->next(); n >= 0; n = adj->next())
				out << "\t<edge source=\"" << cur << "\" target=\"" << n << "\"/>" << endl;
		}

		out << "</graph>\n"
				"</graphml>" << endl;
		delete[] label;
	}

	void writeGraphGV(ostream &out, GraphIterator &G, GraphAttributes *attr, string title) {
		out << (G.isDirected() ? "digraph" : "graph") << " " << title << " {\n" << endl;
		string edgeOp = G.isDirected() ? "->" : "--";
		for (auto gattr : attr->graph)
			out << "\t" << gattr.first << "=" << gattr.second << ";" << endl;

		if (attr->nodes.size() > 0) {
			out << "\nnode [";
			for (auto nattr : attr->nodes)
				out << nattr.first << "=" << nattr.second << ",";
			out << "];" << endl;
		}

		if (attr->edges.size() > 0) {
			out << "\nedge [";
			for (auto eattr : attr->edges)
				out << eattr.first << "=" << eattr.second << ",";
			out << "];" << endl;
		}

		char *label = new char[64];
		for (VertexIdType cur = G.next(); cur >= 0; cur = G.next()) {
			out << "\t" << cur << " [label=\"" << G.label(label, cur) << "\"";
			if (G.getType(cur) != NodeType::UNKN) // == not a MD tree
				for (auto nattr : attr->nodesPerType.at(G.getType(cur)))
					out << ", " << nattr.first << "=\"" << nattr.second << "\"";
			out << "];" << endl;
		}
		G.reset();
		for (VertexIdType cur = G.next(); cur >= 0; cur = G.next()) {
			auto adj = G.getAdjacency(cur);
			for (VertexIdType n = adj->next(); n >= 0; n = adj->next())
				if (G.isDirected() || n > cur)
					out << "\t" << cur << " " << edgeOp << " " << n << ";" << endl;
		}

		out << "}" << endl;
		delete[] label;
	}

	void writeGraph(std::shared_ptr<GraphIterator> G, ostream &out, string format, GraphAttributes *attr, string title) {
		if (format == "GML" || format == "XML" || format == "GraphML") {
			writeGraphGML(out, *G);
		} else if (format == "GV" || format == "GraphViz") {
			GraphAttributes defattrs;
			defattrs.nodes["style"] = "filled";
			defattrs.nodes["fillcolor"] = "white";
			defattrs.graph["ranksep"] = "3";
			defattrs.nodesPerType[NodeType::LEAF]["shape"] = "circle";
			defattrs.nodesPerType[NodeType::LEAF]["color"] = "black";
			defattrs.nodesPerType[NodeType::PARALLEL]["shape"] = "rect";
			defattrs.nodesPerType[NodeType::PARALLEL]["color"] = "Teal";
			defattrs.nodesPerType[NodeType::SERIES]["shape"] = "rect";
			defattrs.nodesPerType[NodeType::SERIES]["color"] = "Goldenrod";
			defattrs.nodesPerType[NodeType::PRIME]["shape"] = "box3d";
			defattrs.nodesPerType[NodeType::PRIME]["color"] = "Firebrick";

			if (attr != nullptr) {
				defattrs.graph.insert(attr->graph.begin(), attr->graph.end());
				defattrs.nodes.insert(attr->nodes.begin(), attr->nodes.end());
				defattrs.edges.insert(attr->edges.begin(), attr->edges.end());
			}
			writeGraphGV(out, *G, &defattrs, title);
		} else
			throw runtime_error("Unknown graph format. Refer to doc for recognized formats.");
	}

	void writeGraph(std::shared_ptr<GraphIterator> G, string outname, string format, GraphAttributes *attr, string title) {
		ofstream out(outname);
		writeGraph(G, out, format, attr, title);
		out.flush();
		out.close();
	}

	shared_ptr <GraphIterator> getIterator(fmdm::Node *root, qc::labelFunction label) {
		return shared_ptr<GraphIterator>(new FMDMMDTreeIterator(root, label));
	}


//	GraphIterator *getIterator(fmdm::Graph &) {
//		throw runtime_error("Unimplemented!");
//	}

	shared_ptr <GraphIterator> getIterator(qc::Graph &graph) {
		return shared_ptr<GraphIterator>(new QCGraphIterator(graph));
	}


/*	std::shared_ptr<GraphIterator> getIterator(md::MDTreeNode &root, quickcliques::labelFunction label) {

	}*/
}