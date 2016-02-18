//
// Created by Lauréline Nevin <git@compilin.dev> on 10/02/16.
//

#ifndef SIMPLECLIQUES_PARSEGML_H
#define SIMPLECLIQUES_PARSEGML_H

#include <string.h>
#include <expat.h>
#include <unordered_map>
#include <vector>
#include "dm.hpp"

fmdm::Graph parsegml(char *filename, bool verb = false);

#endif //SIMPLECLIQUES_PARSEGML_H
