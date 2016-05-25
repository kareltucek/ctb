#FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0 
FLAGS=  -Wall -Wno-unused-variable -Wno-return-local-addr -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0   -fmax-errors=5  -fno-inline -DDEBUGFIELDS 
#-DTESTOVANI

export CTB_ARGS= -w -g
export CXX=g++

SOURCES = datatypes.h ctb.h instructions.h test.cpp writer.h loader_xml.h graph.h aliasenv_maker.h aliasenv_simple.h aliasenv_bobox.h loader_csv.h Makefile tagmaster.h proxy.h generator.h errorhandling.h parser.h loader_test.h cartesian_multiplier.h aliasenv_generator.h aliasenv_simu.h ptrglue.h cf_transform.h multicont.h graph_factor.h aliasenv_cfmacros.h split.h aliasenv_cf.h aliasenv_bind.h languages.h

all : test ctb basictest

basictest : testdir1 testdir2 testdir3 ssedir testdir5 testdir7 testdir10 testdir11 testdir12 testdir13 testdir14 testdir15 testdir16 testdir17
fulltest : all testdir6 testdir4

test : test.cpp $(SOURCES)
	${CXX} ${FLAGS} test.cpp -l tinyxml2 -o test

ctb : main.cpp $(SOURCES)
	${CXX} ${FLAGS} main.cpp -l tinyxml2 -o ctb

tmp :
	g++ --std=c++0x tmp.cpp

testdir1 : test
	./test && make -C test1_simple

testdir2 : ctb
	make -C test2_exports

testdir3 : ctb
	make -C test3_loaders

testdir4 : ctb
	make -C test4_sse_simulation_complete_instruction_check

testdir5 : ctb
	make -C test5_sse_simulation_simple_graph

testdir6 : ctb
	-make -C test6_sse_bobox_with_preloads

testdir7 : ctb
	make -C test7_graph_transformations

testdir8 : ctb
	make -C test8_graph_transformations2

testdir9 : ctb
	make -C test9_cf_macros

testdir10 : ctb
	make -C test10_cf_macros

testdir11 : ctb
	make -C test11_simple_cf_test

testdir12 : ctb
	make -C test12_vectorized_cf_test

testdir13 : ctb
	make -C test13_vectorized_cf_sse_test

testdir14 : ctb
	make -C test14_inference_test 

testdir15 : ctb
	make -C test15_obscure_conversions 

testdir16 : ctb
	make -C test16_matrix_mult 

testdir17 : ctb
	make -C test17_split 


ssedir : ctb
	make -C sse_set

clean : cleanctb cleanthesis cleantests

cleanctb :
	-rm test ctb

cleanthesis :
	-make -C thesis clean

cleantests :
	-make -C sse_set clean
	-make -C test1_simple clean
	-make -C test2_exports clean
	-make -C test3_loaders clean
	-make -C test4_sse_simulation_complete_instruction_check clean
	-make -C test5_sse_simulation_simple_graph clean
	-make -C test6_sse_bobox_with_preloads clean
	-make -C test7_graph_transformations clean
	-make -C test8_graph_transformations2 clean
	-make -C test9_cf_macros clean
	-make -C test10_cf_macros clean
	-make -C test11_simple_cf_test clean
	-make -C test12_vectorized_cf_test clean
	-make -C test13_vectorized_cf_sse_test clean
	-make -C test14_inference_test clean
	-make -C test15_obscure_conversions clean
	-make -C test16_matrix_mult clean
	-make -C test17_split clean
