FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++14  -g -gdwarf-2 -O0 -Wno-deprecated-increment-bool

CXX=g++

all : make_anyway output.preprocessed.cpp graph.out graph_cf.out graph.out.png graph_cf.out.png run

make_anyway :
	make -C .. ctb

test : output.preprocessed.cpp
	${CXX} -ggdb output.preprocessed.cpp -o test -msse4.1

visual : make_anyway
	../ctb -f program_visual

graph.out graph_cf.out output.preprocessed.cpp : graph.xml instab.csv ../ctb
	../ctb -f program

graph.out.png graph_cf.out.png : graph.out graph_cf.out
	cat graph.out | dot -Tpng > graph.out.png
	cat graph_cf.out | dot -Tpng > graph_cf.out.png

run : graph.out test
	./check_output.sh 
	#diff graph_cf.out graph_cf.out.expected

clean :
	-rm  graph.out graph_cf.out *.png *output*cpp output*.h



