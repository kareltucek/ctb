INCLUDE=-I../sse_cf_macros

all : run

run : macrotest 
	./macrotest

buffer_test_preprocess :
	gcc -E buffer_test.h ${INCLUDE} | sed 's/case/\r&/g' | grep -v '^#' > buffer_test_preprocessed.h

macrotest : main.cpp buffer_test_preprocess
	g++ -ggdb main.cpp -o macrotest ${INCLUDE}

clean :
	-rm macrotest buffer_test_preprocessed.h
