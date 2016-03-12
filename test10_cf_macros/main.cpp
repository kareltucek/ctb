#include <iostream>
#include <assert.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <x86intrin.h>
 #include <stdint.h>
#include <iostream>

#include "buffer_test_preprocessed.h"
//#include "fbuffer_test_preprocessed.h"

/*
 * Note that I do understand that this inclusion looks terribly, but there are good reasons for it since we deal here with pure macro code.
 * */

int main()
{
  buffer_test();
}

