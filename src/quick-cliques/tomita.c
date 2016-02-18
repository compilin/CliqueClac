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
#include<time.h>

#include"misc.h"
#include"LinkedList.h"
#include"MemoryManager.h"
#include"tomita_algorithm.h"

/*! \file tomita.c

   \brief Execute the tomita algorithm in tomita_algorithm.c
          and print the number of cliques found and wall clock
          execution time.

    \author Darren Strash (first name DOT last name AT gmail DOT com)

    \copyright Copyright (c) 2011 Darren Strash. This code is released under the GNU Public License (GPL) 3.0.

    \image html gplv3-127x51.png

    \htmlonly
    <center>
    <a href="gpl-3.0-standalone.html">See GPL 3.0 here</a>
    </center>
    \endhtmlonly
*/

int main()
{

#ifdef MEMORY_DEBUG
    fprintf(stderr, "WARNING: MEMORY_DEBUG is defined, timing will be off.\n");
#endif

    int n; // number of vertices
    int m; // 2x number of edges

    LinkedList** adjacencyList = readInGraphAdjList(&n,&m);

    char** adjacencyMatrix = Calloc(n, sizeof(char*));

    int i;

    for(i=0;i<n;i++)
    {
        adjacencyMatrix[i] = Calloc(n, sizeof(char));
        int j = 0;
        Link* curr = adjacencyList[i]->head->next;

        while(!isTail(curr))
        {
            adjacencyMatrix[i][(int)(curr->data)] = 1; 
            curr = curr->next;
        }

    }

    #ifdef RETURN_CLIQUES_ONE_BY_ONE
    LinkedList* cliques = createLinkedList();
    #endif

    runAndPrintStatsMatrix( &listAllMaximalCliquesMatrix,
                             "tomita",
                             adjacencyMatrix, 
                             #ifdef RETURN_CLIQUES_ONE_BY_ONE
                             cliques,
                             #endif
                             n );

    // Free up memory from adjacency list.

    #ifdef RETURN_CLIQUES_ONE_BY_ONE
    destroyCliqueResults(cliques);
    #endif

    i = 0;
    while(i<n)
    {
        Free(adjacencyMatrix[i]);
        destroyLinkedList(adjacencyList[i]);
        i++;
    }

    Free(adjacencyMatrix); 
    Free(adjacencyList); 

    return 0;
}
