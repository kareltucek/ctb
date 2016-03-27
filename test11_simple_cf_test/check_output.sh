#!/bin/bash
[ `./test | grep OUTPUT | wc -l` -eq 100 ] && echo "We obtained 100 results as expected" && exit 0
echo "Something went wrong, we did not get expected number of output lines."
exit 1

