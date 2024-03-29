#ifndef _DJS_DEGENERACY_ALGORITHM_H_
#define _DJS_DEGENERACY_ALGORITHM_H_

/* 
    This program is free software: you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation, either version 3 of the License, or 
    (at your option) any later version. 
 
    This program is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details. 
 
    You should have received a copy of the GNU General Public License 
    along with this program.  If not, see <http://www.gnu.org/licenses/> 
*/

#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"LinkedList.h"

#ifdef __cplusplus

#include <functional>

namespace quickcliques {
	extern "C" {
#endif

#include"misc.h"
#include"MemoryManager.h"
#include"degeneracy_helper.h"

/*! \file degeneracy_algorithm.h

    \brief see degeneracy_algorithm.c

    \author Darren Strash (first name DOT last name AT gmail DOT com)

    \copyright Copyright (c) 2011 Darren Strash. This code is released under the GNU Public License (GPL) 3.0.

    \image html gplv3-127x51.png

    \htmlonly
    <center>
    <a href="gpl-3.0-standalone.html">See GPL 3.0 here</a>
    </center>
    \endhtmlonly
*/

void listAllMaximalCliquesDegeneracyRecursive( long* cliqueCount,
                                               #ifdef RETURN_CLIQUES_ONE_BY_ONE
                                               LinkedList* cliques,
                                               #endif
                                               LinkedList* partialClique, 
                                               int* vertexSets, int* vertexLookup,
                                               int** neighborsInP, int* numNeighbors,
                                               int beginX, int beginP, int beginR);

long listAllMaximalCliquesDegeneracy(LinkedList **adjList,
                                     int **adjacencyList,
                                      #ifdef RETURN_CLIQUES_ONE_BY_ONE
                                     LinkedList *cliques,
                                      #endif
                                     int *degree,
                                     int size);

#ifdef __cplusplus

}

	typedef std::function<const char *(long)> labelFunction;

	typedef struct graph {
		int n;
		LinkedList **adj;
		labelFunction labels;
	} Graph;
}
#endif


#endif
