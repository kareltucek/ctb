FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++14  -g -gdwarf-2 -O0 -Wno-deprecated-increment-bool

all : output.h test run

output.h : ../ctb ../sse_set/C_table_bobox.csv graph.xml
	../ctb -f program ${CTB_ARGS}

test : output.h
	${CXX} ${FLAGS} -DNDEBUG -ggdb main.cpp -I../../bobox/boback/h -I../../ulibpp/h   ../../bobox/boback/lib/release/boback.a  ../../ulibpp/lib/release/ulibpp.a -lstdc++ -lrt -lnuma -pthread  -o test -msse4.1

run :
	./test model.bobolang || true

clean :
	-rm  output.h test



