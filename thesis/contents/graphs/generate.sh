#!/bin/bash
options='node[shape=box];edge[arrowsize=4.0]'
fixcommand='s/{\([^}]*\)#newline\([^}]*\)}/{\$\\begin{matrix} \\text{\1} \\\\ \\text{\2} \\end{matrix}\$ }/g;s/#vertical.*//g'

for i in *.dot
do
  echo "generating graph $i"
  cat $i | sed 's/#include/'"$options"'/g' | dot2tex  -ftikz --figonly -t raw -s | tee $i.debug | ./compressLR.sh $i | ./compressTB.sh $i | sed "$fixcommand" > $(echo $i | sed 's/.dot$/.tex/g')
done

