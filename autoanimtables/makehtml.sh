#!/usr/bin/env bash

for f in *.dat.txt; do
    newf=$(basename $f .dat.txt).html
    echo "$f -> $newf"
    ansifilter -H -r style.css \
                -m colormap \
                -i $f \
                -o $newf
done 

./htmltree.sh > index.html

