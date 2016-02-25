#!/bin/python3
from sys import argv
from networkx import generators

if len(argv) < 3:
    print("Must supply number of cliques, clique size and edge rewrite probability")
    exit(1)

nbc = int(argv[1])
csize = int(argv[2])
prob = float(argv[3])
outfile = argv[4] if len(argv) > 4 else "out.graphml"

graph = generators.relaxed_caveman_graph(nbc, csize, prob)
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
        out.write("\t<node id=\"%s\"/>" % n)

    for e in graph.edges_iter():
        out.write("\n<edge source=\"%s\" target=\"%s\"/>" % e)

    out.write("</graph></graphml>\n")

    print("Successfully wrote to file")
