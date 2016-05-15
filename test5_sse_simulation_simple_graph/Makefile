FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0

all : test run

output.h : ../ctb ../sse_set/C_table_bobox.csv graph.xml
	../ctb -f program ${CTB_ARGS}

test : output.h simu.cpp macros.h
	${CXX} ${FLAGS} simu.cpp -DTEMPLATED -o test -l tinyxml2 -msse4.1

run : 
	./test

clean :
	-rm test output.h



