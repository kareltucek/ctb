#!/bin/bash

options='node[shape=box]'
includecommand="s/#include/"$options"/g"
layercommand='s/#l1/[style=dotted]/g;s/#l2/[style=dotted,arrowhead=odot]/g'
labelcommand='s/#label/shape=rect/g'
textcommand='s/#text/shape=ellipse/g'

export optionscommand="$includecommand;$layercommand;$labelcommand;$textcommand"
