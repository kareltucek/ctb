#!/bin/bash
cat | sed 's/\.[0-9]*//g' > /tmp/mygraph123
command=$(cat /tmp/mygraph123 | grep -o ',[0-9][0-9]*' | sed 's/,//g' | sort -n -u | awk '
BEGIN {i=0;}
/[0-9]/ {y[i]=$1; i++; next;}
END { 
for(j=0; j < i; j++)
{
  printf("s/,###%ibp/,%ibp/g;",y[j],10+j*30);
}
}
')


cat /tmp/mygraph123 |  sed 's/,/,###/g' | sed "$command" | sed 's/,###/,/g' 

rm /tmp/mygraph123
