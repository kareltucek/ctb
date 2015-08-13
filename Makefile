#FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0 
FLAGS= -DTESTOVANI -Wall -Wno-return-local-addr -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0 

CXX=g++11

all : test ctb testdir3 testdir2 testdir1

test : datatypes.h ctb.h instructions.h test.cpp writer.h loader_xml.h graph.h model_maker.h model_simple.h model_bobox.h loader_csv.h Makefile tagmaster.h proxy.h generator.h
	${CXX} ${FLAGS} test.cpp -DTEMPLATED -o test -l tinyxml2

ctb : main.cpp test
	${CXX} ${FLAGS} main.cpp -DTEMPLATED -o ctb -l tinyxml2

testdir1 : test
	./test && make -C unit_test1

testdir2 : ctb
	make -C unit_test2

testdir3 : ctb
	make -C unit_test3


clean :
	rm *.o test ctb



