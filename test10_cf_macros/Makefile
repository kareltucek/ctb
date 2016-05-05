INCLUDE=-I../sse_cf_macros

all : run

run : macrotest 
	./macrotest

buffer_test_preprocessed.h : buffer_test.h 
	../ctb -f program

macrotest : main.cpp buffer_test_preprocessed.h
	${CXX} -ggdb main.cpp -o macrotest  -m32 -msse4.1

clean :
	-rm macrotest buffer_test_preprocessed.h
