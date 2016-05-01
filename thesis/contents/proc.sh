#!/bin/bash
k=0
for i in "00" "01 02 03"  "04 05"  "06 " " 07" "08 09 10" "11 12 13"
do
  rm /tmp/joined$k.txt
  for j in $i
  do
    cat $j* >> /tmp/joined$k.txt
    cp $j* /tmp/$j.txt
  done
  k=$(( $k + 1 ))
done

