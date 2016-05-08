FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++14  -g -gdwarf-2 -O0 -Wno-deprecated-increment-bool

all : make_anyway graph.out graph_cf.out graph.out.png graph_cf.out.png run

make_anyway :
	make -C .. ctb

graph.out graph_cf.out : ../ctb graph.xml
	../ctb -f program ${CTB_ARGS}

visual : ../ctb graph.xml
	../ctb -f program_visual ${CTB_ARGS}

graph.out.png graph_cf.out.png : graph.out graph_cf.out
	cat graph.out | dot -Tpng > graph.out.png
	cat graph_cf.out | dot -Tpng > graph_cf.out.png

run : graph.out
	diff graph_cf.out graph_cf.out.expected || :


clean :
	-rm  graph.out graph_cf.out *.png



