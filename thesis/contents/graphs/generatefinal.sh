#!/bin/bash

mkdir final
rm final/*
for i in fixed/*
do
  cat $i | ./postprocess.sh > $(echo "$i" | sed 's/^fixed/final/g')
done
for i in *.tex
do
  [ -f final/$i ] || cat $i | ./postprocess.sh > final/$i
done

