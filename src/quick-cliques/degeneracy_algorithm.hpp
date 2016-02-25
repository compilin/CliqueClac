//
// Simple C++ "wrapper" header around degeneracy_algorithm.h
//

#ifndef CLIQUECLAC_DEGENERACY_ALGORITHM_HPP_H
#define CLIQUECLAC_DEGENERACY_ALGORITHM_HPP_H


#include <functional>

namespace quickcliques {
	extern "C" {
#include "degeneracy_algorithm.h"
	}

	typedef struct graph {
		int n;
		LinkedList **adj;
		std::function<const char *(long)> labels;
	} Graph;
}

#endif //CLIQUECLAC_DEGENERACY_ALGORITHM_HPP_H
