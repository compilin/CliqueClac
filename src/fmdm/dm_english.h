/******************************************************

Copyright 2004, 2010 Fabien de Montgolfier
fm@liafa.jussieu.fr

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

**********************************************************/

/********************************************************

MODULAR DECOMPOSITION OF UNDIRECTED GRAPHS
by Fabien de Montgolfier

The program dm.c offer a single function, 
decomposition_modulaire().

The input is a graph, its output is its modular decomposition tree.

Input graph is stored using adjacency lists, and trees using a pointers representation (see below)
********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "dm_types.h"

#ifdef __cplusplus
namespace fmdm {
	extern "C" {
#endif

/* prototype of the function.
   Input is a graph, output the root of the modular decomposition tree */

Node *decomposition_modulaire(Graph G);

void printarbre(Node *N, labelFunction label);

#ifdef __cplusplus
}
}
#endif