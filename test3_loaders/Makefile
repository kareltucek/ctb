FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O1 -msse4.1

all : test


output_csv.h : ../ctb instab.xml graph.xml
	../ctb -f ./program ${CTB_ARGS}
	diff output_xml.h output_csv.h


test : output_csv.h
	${CXX} ${FLAGS} main.cpp -DTEMPLATED -o test -l tinyxml2 
	./test


clean :
	-rm test output_xml.h output_csv.h

