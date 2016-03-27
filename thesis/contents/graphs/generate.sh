#!/bin/bash
options='node[shape=box];\nnojustify=true;'

for i in *.dot
do
  echo "generating graph $i"
  cat $i | sed 's/#include/'"$options"'/g' | dot2tex  -ftikz --figonly -s | tee $i.debug | ./compressLR.sh | ./compressTB.sh > $(echo $i | sed 's/.dot$/.tex/g')
done

