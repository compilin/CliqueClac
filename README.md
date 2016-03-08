CliqueClac
==========

An attempt at a fast clique-finding algorithm in C++.

This work conducted at the [LORIA](http://www.loria.fr/loria-news?set_language=en),
 although my own (Lauréline Nevin) participation in this is volunteer work
 out of interest in the subject.
 
This work reuses existing implementation and redistributes them with their
 respective own [licenses](#licenses). 

Context
-------

Finding maximal cliques in a graph is a well-known NP-complete problem.
 CliqueClac is an attempt at a faster clique-finding algorithm by using
 modular decomposition. The reasoning is as following:
 
 * The MD tree of a graph can be computed in [linear time](#references)
 * Enumerating cliques from a prime-less MD tree is trivial
 * We can use any other clique-finding algorithm to enumerate cliques in
 prime nodes, then replace them in the tree with pseudo-parallel nodes, 
 having a pseudo-series node child for each maximal clique found.
 * The resulting pseudo-* nodes can be treated like their normal counterparts
 as far as maximal clique enumeration goes
 
The resulting algorithm would be, in the worst case (the graph decomposes 
 into one huge Prime node), as complex as the clique-finding algorithm 
 used plus the MD Tree calculation. Hope is that in most real-life cases,
 calculating the maximal cliques of all prime nodes of the MD Tree is
 faster than calculating those of the whole graph.

Enumerating
-----------

The algorithm for enumerating cliques from a primeless MD tree is as 
follows:

    worklist = emptyList()
    worklist.add(newSet(MDTree.root))
    for node in MDTree: # Breadth-first or pre-order depth-first search
        if node.type == LEAF:
            continue # Nothing to do for leafs
        for set in worklist:
            if set.contains(node):
                set.remove(node)
                if node.type == PARALLEL:
                    # Replace each set containing a parallel with sets, 
                    # each containing one of the parallel node's children
                    worklist.remove(set)
                    for child in node.children:
                        newset = set.clone()
                        newset.add(child)
                        worklist.add(newset)
                else: # SERIES
                    # In every set that contains a SERIES, replace it with
                    # all of its children 
                    for child in node.children:
                        set.add(child)

Note that this pseudo-code is purely illustrative and actually pretty far,
 in structure, from the actual implementation, which actually iterates
 over worklist rather than through the MD tree.
 
Licenses
--------

[![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-1.png)](http://www.wtfpl.net/)

This work is distributed under the [WTFPL](http://www.wtfpl.net/), 
**although this only applies to source files direclty in src/**. Source files in
 src/fmdm and src/quick-cliques are, aside some minors modifications,
 taken from, respectively, [here](http://www.liafa.univ-paris-diderot.fr/~fm/algos/)
 and [there](http://www.dcs.gla.ac.uk/~pat/jchoco/clique/enumeration/). Both
 work are GPL, and thus are redistributed here as such. 

References
----------
 * Tedder, M., Corneil, D., Habib, M., & Paul, C. (2008). Simpler linear-time modular decomposition via recursive factorizing permutations. Lecture Notes in Computer Science. doi:10.1007/978-3-540-70575-8_52
