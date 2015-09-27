FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O1

CXX=g++


all : test run

test_simple.h : ../ctb ../test instab.xml graph.xml
	cd ..; ./test

test : test_simple.h
	${CXX} ${FLAGS} test_simple.cpp -DTEMPLATED -o test -l tinyxml2 -msse4.1

run : 
	-diff test_simple.h test_simple.h.reference
	./test

rewrite :
	cp test_simple.h test_simple.h.reference

clean :
	-rm test test_simple.h



