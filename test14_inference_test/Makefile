FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++14  -g -gdwarf-2 -O0 -Wno-deprecated-increment-bool

all : make_anyway graph.out graph_cf.out run

make_anyway :
	make -C .. ctb

graph.out graph_cf.out : ../ctb graph.xml
	../ctb -f program ${CTB_ARGS}

visual : ../ctb graph.xml
	../ctb -f program_visual ${CTB_ARGS}

run :
	:


clean :
	-rm  graph.out graph_cf.out *.png



