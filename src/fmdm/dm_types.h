//
// Created by Lauréline Nevin <Lauréline Nevin> on 29/02/16.
//

#ifndef CLIQUECLAC_DM_TYPES_H
#define CLIQUECLAC_DM_TYPES_H


#ifdef __cplusplus
namespace fmdm {
	extern "C" {
#endif

/**********************************
Definition of graphs (for the input)
The graph has n vertices. Each vertex is numbered from 0 to n-1.
A graph is a structure (not an object-oriented program !).
If you declare "graphe Gr", Gr.n is the numer of vertices of Gr.
Gr.G is an array of size n. Gr.G[i] points the first element
of adjaceny list of vertex i (NULL if i is isolated)
An adjency list is an usual linked list (vertex;pointer to next).
Adjacency lists may be unsorted.

WARNING : the input graph *MUST* be symmetric : if j belongs to adjacency list of i then i belongs to adjacency list of j. Graphs that do not respect this produce unpredictible and false results.
**********************************/


typedef long VertexIdType;

typedef const char *(*labelFunction)(VertexIdType);


/* structure for creating an adjacency list */
typedef struct adj {
	VertexIdType s; // number of the vertex
	struct adj *suiv; // adress of next pair in the list, NULL if last
} Adj;

typedef struct {
	long n; //number of vertices of the graph
	Adj **G; // array of size n. G[i] points the first pair of the adjaceny list of vertex i
	labelFunction labels; // Fonction to convert vertex IDs to labels
} Graph;

/********************************
Output : definition of modular decomposition tree.
Each internal node is labelled SERIE (for series), PARALLELE (for parallel) or PREMIER (for prime) depending of the quotient's type.
Each leaf is labelled FEUILLE and also contains the vertex number of the leaf.
As the tree is an inclusion tree, the vertex-set corresponding to an internal node correspond to the vertices numbers of the leaves that descend from that tree. The function decomposition_modulaire() return a pointer to the root of the tree.
*/


/* define the type of nodes. UNKN,MODULE,ARTEFACT are for internal use*/

typedef enum nodeType {
	LEAF = 0,
	UNKN = 1,
	MODULE = 2,
	ARTIFACT = 3,
	SERIES = 4,
	PARALLEL = 5,
	PRIME = 6
} NodeType;
/* defines a node of the tree */

typedef struct node {
	NodeType type;    // is FEUILLE, SERIE, PARALLELE or PREMIER
	struct node *pere;    // adress of parent node, NULL if root
	struct child *fpere;    // points the head of the linked list of sons (if type is not FEUILLE, else is NULL)
	VertexIdType nom;    // if type=FEUILLE, number of the corresponding vertex of the graph
	struct child *fils; // points the head of the linked list of sons
	struct child *lastfils;  // internal use (points the last item in the listed list of sons)
	int id;    // internal use (node unique ID)
	VertexIdType fv;  // internal use (first vertex in factorizing permutation)
	VertexIdType lv;  // internal use (last vertex in factorizing permutation)
} Node;

/* linked list that strore the sons of an internal node (in any order) */

typedef struct child {
	struct node *pointe; // adress of the node in the tree
	struct child *suiv; // adress of the next pair in the list, NULL if last
} Child;


#ifdef __cplusplus
}
}
#endif

#endif //CLIQUECLAC_DM_TYPES_H
