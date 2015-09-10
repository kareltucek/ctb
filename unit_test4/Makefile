#FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O1  -fmax-errors=5
FLAGS= -DTESTOVANI 


CXX=g++

all : test

output.h : ../ctb ../sse_set/C_table.csv
	../ctb -c -f ./program

test : output.h
	${CXX} ${FLAGS} main.cpp -o test -l tinyxml2 -msse4.1
	./test


clean :
	-rm test output.h 

