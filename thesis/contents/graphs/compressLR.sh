#!/bin/bash
cat | sed 's/\.[0-9][0-9]*//g' > /tmp/mygraph124
skip=$(cat $1 | grep '#nohorizontal')
linear=$(cat $1 | grep '#horizontalLinear')
width=$(cat $1 | grep '#width' | grep '[0-9][0-9]*' -o)
horizontal=$(cat $1 | grep '#horizontal' | grep '[0-9][0-9]*' -o)
[ "$linear" == "" ] && linear="false"
[ "$linear" == "false" ] || linear="true"
[ "$width" == "" ] && width=380
[ "$horizontal" == "" ] && horizontal=120
command=$(cat /tmp/mygraph124 | grep -o '[0-9][0-9]*bp,' | sed 's/bp,//g' | sort -n -u | awk '
BEGIN {i=0;max = 0;}
/[0-9]/ {
y[i]=$1; avg[i]=$1; y2[i]=i; mm[i]=$1; mx[i]=$1; i++;
if($1 > max) {max = $1;}
  next;
}
END { 
maxcols = 5;
maxmove = 50;

ii = i;
while(ii > maxcols)
{
  mindist = 10000;
  at = 0;
  for(j = 0; j < i-1; j++)
  {
    if(avg[j+1] - avg[j] < mindist && y2[j+1] != y2[j])
    {
      mindist = avg[j+1] - avg[j];
      at = j;
    }
  }
  if(mx[y2[at+1]]-mm[y2[at]] > maxmove)
  {
    break;
  }
  r = (avg[at]+avg[at+1])/2;
  myy2 = y2[at];
  for(j = 1; j < i; j++)
  {
    if(y2[j] > myy2)
    {
      y2[j] = y2[j]-1;
    }
    if(y2[j] == myy2)
      avg[j] = r;
  }
  for(j = y2[at+1]; j < i; j++)
  {
    mx[j-1] = mx[j];
    mm[j] = mm[j+1];
  }
  ii--;
}

width = '"$width"';
unit = width/ii;
maxunit = '"$horizontal"';
if(unit > maxunit)
  unit = maxunit;
for(j=0; j < i; j++)
{
  if("'"$linear"'" == "true")
  {
    printf("s/(%ibp###,/(%ibp,/g;",y[j],y[j]*width/max);
  }
  else
  {
    printf("s/(%ibp###,/(%ibp,/g;",y[j],y2[j]*unit);
  }
}
}
')

[ "$skip" != "" ] && command=""

cat /tmp/mygraph124 |  sed 's/,/###,/g' | sed "$command" | sed 's/###,/,/g' 

rm /tmp/mygraph124

