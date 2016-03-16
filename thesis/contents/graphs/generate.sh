#!/bin/bash
for i in *.dot
do
  echo "generating graph $i"
  cat $i | dot2tex --figonly > $(echo $i | sed 's/.dot$/.tex/g')
done

