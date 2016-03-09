#include <iostream>
#include <sys/resource.h>
#include "parsegraphml.hpp"
#include "enumeratecliques.hpp"
#include "writegraph.hpp"

#define MSECONDS(time) (((time) * 1e-09))
#define VERBOSE string("no")

//static const rlim_t MAX_MEM = ;

using namespace std;
using namespace fmdm;
using namespace cliqueclac;
namespace qc = quickcliques;

void printCliqueList(ostream &out, const CliqueList &cliques);

int main(int argc, char **argv) {

	if (argc < 2) {
		cerr << "Must supply a GML file name as argument" << endl;
		return 1;
	}

	struct rlimit64 rl;
	getrlimit64(RLIMIT_AS, &rl);
	rl.rlim_cur = 2L * (1L<<30); // 2G
	setrlimit64(RLIMIT_AS, &rl);

	Node *dm_root;
	qc::Graph qcG;
	{
		auto G = parsegml(argv[1], true);
		dm_root = decomposition_modulaire(G);
		qcG = fmdmGraphToStrash(G);
		if (VERBOSE == "console")
			printarbre(dm_root, G.labels);
	}
	cout << "MD tree to mdtree.gv" << endl;
	writeGraph(getIterator(dm_root, qcG.labels), "mdtree.gv", "GV", nullptr, "");

	double timeratio;
	{
		struct timespec spec;
		clock_getres(CLOCK_PROCESS_CPUTIME_ID, &spec);
		timeratio = spec.tv_sec + (spec.tv_nsec / 1000000000.);
	}

	cout << "Enumerating cliques without MD" << endl;
	auto time = -clock();
	auto cliques = enumerateCliques(qcG);
	cout << "Time taken : " << MSECONDS(time + clock()) << "ms" << endl;
	if (VERBOSE != "no") {
		sortCliqueList(cliques);
		if (VERBOSE == "file" || VERBOSE == "both") {
			auto out = ofstream("cliquelist_bk.txt");
			printCliqueList(out, cliques);
			out.close();
		}
		if (VERBOSE == "console" || VERBOSE == "both")
			printCliqueList(cout, cliques);
	}

	cout << "Enumerating cliques with MD" << endl;
	time = -clock();
	cliques = enumerateCliques(qcG, *dm_root);
	cout << "Time taken : "<< MSECONDS(time + clock()) << "ms" << endl;
	if (VERBOSE != "no") {
		sortCliqueList(cliques);
		if (VERBOSE == "file" || VERBOSE == "both") {
			auto out = ofstream("cliquelist_bk.txt");
			printCliqueList(out, cliques);
			out.close();
		}
		if (VERBOSE == "console" || VERBOSE == "both")
			printCliqueList(cout, cliques);
	}

	return 0;
}

void printCliqueList(ostream &out, const CliqueList &cliques) {
	for (Clique clique : cliques) {
			out << "[";
			auto first = clique[0];
			for (VertexIdType id : clique) {
				out << (id == first ? "" : ", ") << id;
			}
			out << "]" << endl;
		}
	if (VERBOSE == "file") {
			dynamic_cast<ofstream*>(&out)->close();
		}
}