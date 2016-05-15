FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++14  -g -gdwarf-2 -O0 -Wno-deprecated-increment-bool

all : make_anyway output.preprocessed.cpp graph.out graph_cf.out   run

make_anyway :
	make -C .. ctb

visual : make_anyway
	../ctb -f program_visual ${CTB_ARGS}

graph.out graph_cf.out output.preprocessed.cpp : graph.xml instab.csv
	../ctb -f program ${CTB_ARGS}

run : 
	diff graph_cf.out graph_cf.out.expected || :
	diff output.cpp output.cpp.expected || :

clean :
	-rm  graph.out graph_cf.out *.png output*cpp output*.h



