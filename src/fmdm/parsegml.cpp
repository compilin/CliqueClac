//
// Created by Lauréline Nevin <git@compilin.dev> on 10/02/16.
//

#include <iostream>
#include <fstream>
#include <functional>
#include "parsegml.h"


using namespace std;
using namespace fmdm;

typedef pair<VertexIdType, VertexIdType> EdgeType;
typedef unordered_map<string, VertexIdType> labToId;
typedef unordered_map<VertexIdType, string> idToLab;

static idToLab ids;

const size_t BUFFERSIZE = 4096;

typedef struct parsegraph {
	labToId ids;
	vector<EdgeType> edges;
} ParseGraph;


const char *getAttr(const char **attr, string name) {
	for (int i = 0; attr[i]; i += 2) {
		if (strcmp(attr[i], name.c_str()) == 0)
			return attr[i + 1];
	}
	return NULL;
}

void start(void *data, const char *el, const char **attr) {
	ParseGraph &pg = *(ParseGraph *) data;

	if (strcmp(el, "node") == 0) {
		const char *name = getAttr(attr, "id");
		VertexIdType id = (VertexIdType) pg.ids.size();
		pg.ids.insert(labToId::value_type(string(name), id));
		ids.insert(idToLab::value_type(id, string(name)));
	} else if (strcmp(el, "edge") == 0) {
		const char *from = getAttr(attr, "source"), *to = getAttr(attr, "target");
		if (from == NULL || to == NULL) {
			cerr << "Error : " << (from == NULL ? "source" : "destination") << endl;
			throw new runtime_error("Error : source or target field unknown");
		}
		bool fromFound = false;
		try {
			VertexIdType source = pg.ids.at(string(from));
			fromFound = true;
			VertexIdType target = pg.ids.at(string(to));
			pg.edges.push_back(EdgeType(source, target));
		} catch (out_of_range e) {
			cerr << e.what() << endl;
			cerr << "key : " << (fromFound ? to : from) << endl;
			exit(1);
		}
	}
}

const char *idToLabel(VertexIdType vid) {
	auto lab = ids.find(vid);
	if (lab != ids.end())
		return lab->second.c_str();
	else {
		auto buff = new char[20];
		sprintf(buff, "%lu", (unsigned long) vid);
		return buff;
	}
}

Graph parsegml(char *filename, bool verb) {

	XML_Parser parser = XML_ParserCreate("UTF-8");
	XML_SetStartElementHandler(parser, start);
	ParseGraph pg;
	XML_SetUserData(parser, &pg);

	ifstream in(filename);
	if (!in) {
		char *err = new char[512];
		sprintf(err, "Error opening file : %s", strerror(errno));
		throw ifstream::failure(err);
	}

	long tot = 0;
	void *buf;
	do {
		buf = XML_GetBuffer(parser, BUFFERSIZE);
		in.read((char *) buf, BUFFERSIZE - 1);
		if (in.gcount() > 0) {
			XML_ParseBuffer(parser, (int) (in.gcount()), XML_FALSE);
		}
		tot += in.gcount();
	} while (in.good());

	if (!in.eof()) {
		char *err = new char[512];
		sprintf(err, "Error while reading : %s", strerror(errno));
		throw ifstream::failure(err);
	} else if (verb)
		cout << "Reached EOF" << endl;

	in.close();
	XML_ParserFree(parser);
	if (verb)
		printf("Successfully parsed. %ld bytes read, %ld vertices, %ld edges\n",
		       tot, pg.ids.size(), pg.edges.size());

	Graph G;

	G.n = pg.ids.size();
	G.G = (adj **) calloc((size_t) G.n, sizeof(adj *));
	adj *adjs = (adj *) calloc(pg.edges.size() * 2, sizeof(adj));
	size_t curadj = 0;

	for (EdgeType edge : pg.edges) {
		adj *adj = adjs + (curadj++);
		adj->s = (VertexIdType) edge.second;
		adj->suiv = G.G[edge.first];
		G.G[edge.first] = adj;

		adj = adjs + (curadj++);
		adj->s = (VertexIdType) edge.first;
		adj->suiv = G.G[edge.second];
		G.G[edge.second] = adj;
	}


	G.labels = &idToLabel;

	return G;
}
