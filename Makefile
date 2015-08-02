#FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0 
FLAGS= -DTESTOVANI -Wall -Wno-return-local-addr -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0 

CXX=g++11

all : test output/test_simple.h 
	#testdir ctb

test : datatypes.h ctb.h instructions.h test.cpp writer.h loader_xml.h graph.h model_maker.h model_simple.h model_bobox.h loader_csv.h Makefile
	${CXX} ${FLAGS} test.cpp -DTEMPLATED -o test -l tinyxml2


testdir :
	./test && make -C output


ctb : main.cpp
	${CXX} ${FLAGS} main.cpp -DTEMPLATED -o ctb -l tinyxml2

clean :
	rm *.o test ctb



