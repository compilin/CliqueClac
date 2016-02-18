#include <iostream>
#include "fmdm/parsegml.h"
#include "enumeratecliques.h"

using namespace std;
using namespace fmdm;
namespace ec = enumcliques;

int main(int argc, char **argv) {

	if (argc < 2) {
		cerr << "Must supply a GML file name as argument" << endl;
		return 1;
	}

	auto G = parsegml(argv[1], true);
	auto dm_root = decomposition_modulaire(G);
	printarbre(dm_root, G.labels);

	auto cliques = ec::enumerateCliques(G, *dm_root);
	for (ec::Clique clique : cliques) {
		cout << "[";
		auto first = clique[0];
		for (VertexIdType id : clique)
			cout << (id == first ? "" : ", ") << G.labels(id);
		cout << "]" << endl;
	}

	return 0;
}