#!/bin/bash
[ -f $1.bak ] || mv $1 $1.bak
cat $1.bak | sed 's/param="/params="ioindex=/g' > $1

