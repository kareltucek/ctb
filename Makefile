#FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0 
FLAGS= -DTESTOVANI -Wall -Wno-return-local-addr -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0   -fmax-errors=5

CXX=g++11

all : test ctb testdir1 testdir2 testdir3

test : datatypes.h ctb.h instructions.h test.cpp writer.h loader_xml.h graph.h aliasenv_maker.h aliasenv_simple.h aliasenv_bobox.h loader_csv.h Makefile tagmaster.h proxy.h generator.h errorhandling.h parser.h
	${CXX} ${FLAGS} test.cpp -DTEMPLATED -l tinyxml2 -o test

ctb : main.cpp test
	${CXX} ${FLAGS} main.cpp -DTEMPLATED  -l tinyxml2 -o ctb

testdir1 : test
	./test && make -C unit_test1

testdir2 : ctb
	make -C unit_test2

testdir3 : ctb
	make -C unit_test3


clean :
	-rm test ctb
	-make -C unit_test2 clean
	-make -C unit_test1 clean
	-make -C unit_test3 clean




