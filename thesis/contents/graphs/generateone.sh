#!/bin/bash
fixcommand='s/{\([^}]*\)#newline\([^}]*\)}/{\$\\begin{matrix} \\text{\1} \\\\ \\text{\2} \\end{matrix}\$ }/g;s/#vertical.*//g'
fixcommand2='s/strokecol}{rgb}{1[.0]*,1[.0]*,1[.0]*};/strokecol}{rgb}{0.0,0.0,0.0};/g'

source ${0%/*}/options.sh

while [ $# -gt 0 ]
do
  i=$1
  shift
  echo "generating graph $i"
  cat $i | sed "$optionscommand" | tee log -a | tee $i.debug0 | dot2tex -ftikz -s --figonly -t raw | tee $i.debug | 
  ./compressLR.sh $i | ./compressTB.sh $i |
  sed "$fixcommand2" | sed "$fixcommand" > $(echo $i | sed 's/.dot$/.tex/g')
  printf "\n\n\n" >> log
done
