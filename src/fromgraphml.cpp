#include <iostream>

#include "parsegraphml.hpp"
#include "writegraph.hpp"

using namespace fmdm;
using namespace std;


int main(int narg, char **arg) {
    if (narg < 2) {
        cerr << "Must specify a GraphML file" << endl;
        exit(1);
    }

    Graph G = parsegml(arg[1]);
    auto R = decomposition_modulaire(G);

    if (narg > 2 && strcmp(arg[2], "-v") == 0)
        printarbre(R, G.labels);
    if (narg > 3 && strcmp(arg[3], "-w") == 0) {
        cliqueclac::writeGraph(cliqueclac::getIterator(R, G.labels), "mdtree.gv", "GV");
    }
}