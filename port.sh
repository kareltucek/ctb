#!/bin/bash
cat $1 | awk '
BEGIN {FS="\t";IFS="\t";OFS="\t";}
// {
if($2 == "instruction" || $11 == "code decl")
  {
    cc = $10;
    cd = $11;
    $10 = $12;
    $11 = $13;
    $12 = cc;
    $13 = cd;
  }
  print($0);
}
' > $1.new
