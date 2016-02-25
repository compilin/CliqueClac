#include <iostream>
#include "fmdm/parsegml.hpp"
#include "enumeratecliques.hpp"
#include "writegraph.hpp"

using namespace std;
using namespace fmdm;
namespace cc = cliqueclac;

int main(int argc, char **argv) {

	if (argc < 2) {
		cerr << "Must supply a GML file name as argument" << endl;
		return 1;
	}

	auto G = parsegml(argv[1], true);
	auto dm_root = decomposition_modulaire(G);
	printarbre(dm_root, G.labels);
	cout << "MD tree to mdtree.gv" << endl;
	cc::writeGraph(*cc::getIterator(dm_root, G.labels), "mdtree.gv", "GV", nullptr, "");

	cout << "Enumerating cliques with MD" << endl;
	cc::CliqueList cliques = cc::enumerateCliques(G, *dm_root);
	cc::sortCliqueList(cliques);
	auto out = ofstream("cliquelist_md.txt");
	for (cc::Clique clique : cliques) {
		cout << "[";
		out << "[";
		auto first = clique[0];
		for (VertexIdType id : clique) {
			cout << (id == first ? "" : ", ") << G.labels(id);
			out << (id == first ? "" : ", ") << G.labels(id);
		}
		cout << "]" << endl;
		out << "]" << endl;
	}
	out.close();


	cout << "Enumerating cliques without MD" << endl;
	cliques = cc::enumerateCliques(G);
	cc::sortCliqueList(cliques);
	out = ofstream("cliquelist_bk.txt");
	for (cc::Clique clique : cliques) {
		cout << "[";
		out << "[";
		auto first = clique[0];
		for (VertexIdType id : clique) {
			cout << (id == first ? "" : ", ") << G.labels(id);
			out << (id == first ? "" : ", ") << G.labels(id);
		}
		cout << "]" << endl;
		out << "]" << endl;
	}
	out.close();

	return 0;
}