#!/bin/bash

for f in figures/*
do
  cat $f | sed 's/\(tikzpicture.*\),\]/\1,xscale=2.0,yscale=2.0]/;s/\(node.*\)\]/\1,thick]/g;s/solid/solid,thick/g;s/->/-{Latex[length=4mm]}/g;s/dotted/dashed,thick/g;' > $(echo "$f" | sed 's=[^/]*=figures2=')
done

