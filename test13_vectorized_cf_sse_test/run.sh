#!/bin/bash
#This will show nicely how the algorithm proceeded for the purpose of debug
./test | head -n 1000000 | tr ',' '\n' | tr -s ' \t' | ./astable sep BUFFER '(#|JUMPED|OUTPUT)' 20 | less -S

