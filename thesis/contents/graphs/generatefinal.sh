#!/bin/bash

mkdir final
rm final/*
cp fixed/* final/
for i in *.tex
do
  [ -f final/$i ] || cp $i final/
done

