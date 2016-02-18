//
// Created by Lauréline Nevin <git@compilin.dev> on 12/02/16.
//

#ifndef SIMPLECLIQUES_ENUMERATECLIQUES_H
#define SIMPLECLIQUES_ENUMERATECLIQUES_H
#include "bronkerbosch.h"

namespace enumcliques {
	CliqueList enumerateCliques(fmdm::Graph &G, fmdm::Node &node);
}


#endif //SIMPLECLIQUES_ENUMERATECLIQUES_H
