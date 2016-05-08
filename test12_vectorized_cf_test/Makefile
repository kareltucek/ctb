FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++14  -g -gdwarf-2 -O0 -Wno-deprecated-increment-bool

all : make_anyway output.preprocessed.cpp graph.out graph_cf.out   run

make_anyway :
	make -C .. ctb

test : output.preprocessed.cpp
	${CXX} -ggdb output.preprocessed.cpp -o test

visual : make_anyway
	../ctb -f program_visual ${CTB_ARGS}

graph.out graph_cf.out output.preprocessed.cpp : graph.xml instab.csv ../ctb
	../ctb -f program ${CTB_ARGS}


run : graph.out test
	./check_output.sh 
	#diff graph_cf.out graph_cf.out.expected

clean :
	-rm  graph.out graph_cf.out *.png *output*cpp



