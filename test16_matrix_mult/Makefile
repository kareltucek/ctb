FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++14  -g -gdwarf-2 -O3 -Wno-deprecated-increment-bool

all : make_anyway output.preprocessed.cpp graph.out graph_cf.out   run

make_anyway :
	make -C .. ctb

test : output.preprocessed.cpp
	clang++ -ggdb output.preprocessed.cpp -o test -msse4.2 -fmax-errors=10 --std=c++11 -Wno-parentheses-equality -O3

visual : make_anyway
	../ctb -f program_visual ${CTB_ARGS}

graph.out graph_cf.out output.preprocessed.cpp : macros.h graph.xml ../ctb
	../ctb -f program ${CTB_ARGS}


run : graph.out test
	./test 
	#diff graph_cf.out graph_cf.out.expected

clean :
	-rm  graph.out graph_cf.out *.png *output*cpp output*.h



