#!/bin/sh
# Greps "not documented" warnings from a Doxygen warning log and format them
# If -o or --other is passed, this will grep all warnings other than "not documented" instead
# If -g or --generate is passed, doxygen is called (on doc/Doxyfile) beforehand

FILE="./doxygen_warnings.log"
INVERT=""
if [[ "$*" =~ "-o" ]] || [[ "$*" =~ "--other" ]]; then
	INVERT="-v"
fi

if [[ "$*" =~ "-g" ]] || [[ "$*" =~ "--generate" ]]; then
	echo "Generating/refreshing doc"
	doxygen doc/Doxyfile >/dev/null || exit 1
fi

if ! [ -f "$FILE" ]; then
	echo "$FILE not found. Please run doxygen first"
	exit 1
fi

PREFIX=$(pwd | sed 's/\//\\\//g')
grep $INVERT "not documented" "$FILE" | sed -r -e "s/$PREFIX\/(.*[0-9]): /\1\t/" -e "s/graphshell:://g" | sed -r \
		 -e "s/warning: Member ([^(]*)\(.*\) of class (.*) is not .*$/\2\tMember: \1/" \
		 -e "s/warning: The following parameters of ([^(]*)\(.*\) are not documented:/\1\tParameter/" \
		 -e "s/warning: Compound (\S+) is not documented./\1\tClass/" \
		 | sed -r 's/^([^\t]*)\t(.*)$/\2\t\1/' | column -t -s$'\t' | sort -u
