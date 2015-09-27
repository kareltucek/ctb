FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0 -Wno-deprecated-increment-bool

CXX=g++

all : output.h test run

output.h : ../ctb ../sse_set/C_table.csv graph.xml
	../ctb -f program

test :
	${CXX} ${FLAGS} -DNDEBUG main.cpp -I../../bobox/boback/h -I../../ulibpp/h   ../../bobox/boback/lib/release/boback.a  ../../ulibpp/lib/release/ulibpp.a -lstdc++ -lrt -lnuma -pthread  -o test -msse4.1

run :
	./test model.bobolang



clean :
	-rm  output.h test



