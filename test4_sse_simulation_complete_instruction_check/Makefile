FLAGS= -DTESTOVANI -Wall -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-reorder -Wno-sign-compare -ggdb --std=c++0x  -g -gdwarf-2 -O0  -fmax-errors=5 -Wno-deprecated-increment-bool 
#FLAGS= -DTESTOVANI -O0 


all : ../ctb sse test run


sse :
	make -C ../sse_set

output.h : ../ctb ../sse_set/C_table_bobox.csv Makefile
	../ctb -f ./program -v

test : output.h simu.cpp macros.h random_eng.h
	${CXX} ${FLAGS} simu.cpp -ggdb -o test -l tinyxml2 -msse4.1

run : test
	./test


clean :
	-rm test output.h 

