#!/bin/bash
cat | sed 's/\.[0-9]*//g' > /tmp/mygraph124
command=$(cat /tmp/mygraph124 | grep -o '[0-9][0-9]*bp,' | sed 's/bp,//g' | sort -n -u | awk '
BEGIN {i=0;}
/[0-9]/ {y[i]=$1; avg[i]=$1; y2[i]=i; mm[i]=$1; mx[i]=$1; i++; next;}
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

unit = 380/ii;
maxunit = 70;
if(unit > maxunit)
  unit = maxunit;
for(j=0; j < i; j++)
{
  printf("s/(%ibp###,/(%ibp,/g;",y[j],y2[j]*unit);
}
}
')

cat /tmp/mygraph124 |  sed 's/,/###,/g' | sed "$command" | sed 's/###,/,/g' 

rm /tmp/mygraph124

