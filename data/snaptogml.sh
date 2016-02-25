#!/bin/bash

if [[ $# < 2 ]]; then
    echo "Must supply input and output files as arguments"
    exit 1
fi

inf="$1"
outf="$2"

if ! head -n 20 "$inf" | grep -v ^# | grep ' ' > /dev/null; then
    echo "No space detected in format. If using tabs, please convert!"
    exit 1
fi

cat > "$outf" <<XML
<?xml version="1.0" encoding="UTF-8"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns
http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
<graph id="G" edgedefault="undirected">
XML

echo "Parsing vertices"
cat "$1" | grep -v '^#' | tee >(cut -d' ' -f2 > .tmp.vertices2.txt) | cut -d' ' -f1 | uniq > .tmp.vertices1.txt

echo "Sorting vertices"
sort --parallel=4 -u .tmp.vertices2.txt .tmp.vertices1.txt | sed -r 's/^.+$/<node id="\0"\/>/' >> "$outf"
rm .tmp.vertices2.txt .tmp.vertices1.txt

echo "Vertices parsed. Parsing edges"
cat "$1" | grep -v '^#' | sed -rn 's/^([a-z0-9]+)\s+([a-z0-9]+)$/<edge source="\1" target="\2" \/>/ip' >> "$outf"

cat >> "$outf" <<XML
</graph>
</graphml>
XML

echo "Done"
