#include <iostream>
#include "parsegml.h"

using namespace fmdm;

int main(int narg, char **arg) {
    if (narg < 2) {
        std::cerr << "Must specify a GraphML file" << std::endl;
        exit(1);
    }

    Graph G = parsegml(arg[1]);
    auto R = decomposition_modulaire(G);

    if (narg > 2 && strcmp(arg[2], "-v") == 0)
        printarbre(R);

}