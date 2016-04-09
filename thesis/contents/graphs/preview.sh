#!/bin/bash

source ${0%/*}/options.sh

cat $1 | sed "$optionscommand" | sed 's/#newline/\\n/g' | tee /tmp/graphdebug.dot | dot -Tpng > /tmp/jjjj.png && gpicview /tmp/jjjj.png

