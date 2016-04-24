#!/bin/bash
rm log

for i in *.dot
do
  ./generateone.sh "$i"
done

./generatefinal.sh

