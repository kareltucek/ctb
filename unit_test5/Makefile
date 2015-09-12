FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O1

CXX=g++11


all : test run

output.h : ../ctb ../sse_set/C_table.csv graph.xml
	../ctb -f program

test : output.h
	${CXX} ${FLAGS} simu.cpp -DTEMPLATED -o test -l tinyxml2 -msse4.1

run : 
	./test

clean :
	-rm test output.h



