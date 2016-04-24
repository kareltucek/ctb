#!/bin/bash
cat | sed 's/\.[0-9][0-9]*//g' > /tmp/mygraph123
skip=$(cat $1 | grep '#novertical')
linear=$(cat $1 | grep '#verticalLinear')
vdist=$(cat $1 | grep '#vertical' | grep -o '[0-9]*')
height=$(cat $1 | grep '#height' | grep -o '[0-9]*')
[ "$linear" == "" ] && linear="false"
[ "$linear" == "false" ] || linear="true"
[ "$height" == "" ] && height=100
[ "$vdist" == "" ] && vdist=30
command=$(cat /tmp/mygraph123 | grep -o ',[0-9][0-9]*' | sed 's/,//g' | sort -n -u | awk '
BEGIN {i=0;max=1;}
/[0-9]/ {
  y[i]=$1; i++; 
  if($1 > max) 
    max = $1;
  next;
}
END { 
for(j=0; j < i; j++)
{
  height='"$height"';
  if("'"$linear"'" == "true")
    printf("s/,###%ibp/,%ibp/g;",y[j],y[j]*height/max);
  else
    printf("s/,###%ibp/,%ibp/g;",y[j],10+j*'"$vdist"');
}
}
')

[ "$skip" != "" ] && command=""

cat /tmp/mygraph123 |  sed 's/,/,###/g' | sed "$command" | sed 's/,###/,/g' 

rm /tmp/mygraph123

