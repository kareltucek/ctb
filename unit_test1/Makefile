FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O1

CXX=g++11

test : test_simple.h
	${CXX} ${FLAGS} test_simple.cpp -DTEMPLATED -o test -l tinyxml2


clean :
	rm *.o test



