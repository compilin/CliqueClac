#!/bin/python3
from sys import argv
from networkx import generators
from random import random

if len(argv) < 3:
    print("Must supply number of cliques, clique size and edge rewrite probability")
    exit(1)

nbc = int(argv[1])
csize = int(argv[2])
prob = float(argv[3])
outfile = argv[4] if len(argv) > 4 else "out.graphml"

graph = generators.connected_caveman_graph(nbc, csize)

for i in graph.nodes_iter():
    for j in graph.nodes_iter():
        if random() < prob:
            if j in graph.adj[i]:
                graph.remove_edge(i,j)
            else:
                graph.add_edge(i,j)

print("writing to %s" % outfile)
with open(outfile, 'w') as out:
    out.write("""<?xml version="1.0" encoding="UTF-8"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns
http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
<graph id="G" edgedefault="undirected">
""")
    for n in graph.nodes_iter(False):
        out.write("\t<node id=\"%s\"/>\n" % n)

    for e in graph.edges_iter():
        out.write("\t<edge source=\"%s\" target=\"%s\"/>\n" % e)

    out.write("</graph></graphml>\n")

    print("Successfully wrote to file")
