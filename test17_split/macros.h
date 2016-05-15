#include <iostream>
#include <stdint.h>
#include "context.h"
#include <assert.h>
#include <stdlib.h>
#include "random_eng.h"


#define DEBUG_SSE(var, name)\
{\
  int r[4];\
  _mm_storeu_si128((__m128i*)&r, (__m128i)(var));\
  /*std::cout << #name << " = ";*/\
  for(int i = 3; i >= 0; i--)\
    std::cout << r[i] << ":";\
  /*write(r[i]);*/\
  std::cout << ":";\
}


#define DEBUG_SSE_BOOL(var, name)\
{\
  int r[4];\
  _mm_storeu_si128((__m128i*)&r, (__m128i)(var));\
  /*std::cout << #name << " = ";*/\
  for(int i = 15; i >= 0; i--)\
  {\
      if(i % 4 == 0)\
        std::cout << ":";\
      std::cout << ((bool*)r)[i] ;\
  }\
  /*write(r[i]);*/\
  std::cout << ":";\
}


typedef context<ctx_indices<0,1,2>, ctx_types<bool, uint32_t, uint32_t>, bool, uint32_t,uint32_t> ctxin;
typedef context<ctx_indices<0,1>, ctx_types<uint32_t,uint32_t>, uint32_t,uint32_t> ctxout;

#define COUNT 10000
#define ALLOC 10050

void fill_data(ctxin& ci, ctxout& co)
{
  bool simple = true;
  random_engine rand;
  rand.reset();
    for(int i = 0; i < ALLOC; i++)
    {
      ci.data_0[i] = rand.get() % 3 == 0;
      ci.data_1[i] = rand.get() % 100;
      ci.data_2[i] = rand.get() % 100;
    }
}

void handwritten(int remaining, ctxin& ci, ctxout& co)
{
  for(;remaining > 0; remaining--)
  {
    if(ci.data_0[ci.index_0])
    {
      co.data_0[co.index_0] = ci.data_1[ci.index_1];
      ci.index_1++;
    }
    else
    {
      ci.index_2++;
      co.data_0[co.index_0] = ci.data_2[ci.index_2];
    }
    ci.index_0++;
    co.index_0++;
  }
}

#define START time=clock();
#define STOP(a) \
  time2=clock();\
  std::cout << a << " took " << time2 - time << " ticks" << std::endl;

  template <template <typename...> class BOX>
void test()
{
  typedef BOX<ctxin, ctxout> box;


  bool* i0 = (bool*)aligned_alloc(16, ALLOC*sizeof(bool));
  uint32_t* i1 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* i2 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* o0 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* o1 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));

  ctxin ci;
  ctxout co;

  int time, time2;

test0:
  ci.data_0 = i0;
  ci.data_1 = i1;
  ci.data_2 = i2;
  co.data_0 = o0;
  co.data_1 = o1;
  ci.reset_indices();
  co.reset_indices();
  fill_data(ci, co);
  START
  handwritten(COUNT, ci, co);
  STOP("handwritten(nonvectorized)")
test4:
  ci.reset_indices();
  co.reset_indices();
  fill_data(ci, co);
  START
  box::process_single(COUNT, ci, co);
  STOP("nonvectorized")
test1:
  ci.reset_indices();
  co.reset_indices();
  fill_data(ci, co);
  START
  box::process_aligned(COUNT, ci, co);
  STOP("aligned")
test2:
  ci.data_0 = &i0[0];
  ci.data_1 = &i1[1];
  ci.data_2 = &i2[2];
  co.data_0 = &o0[1];
  ci.reset_indices();
  co.reset_indices();
  fill_data(ci, co);
  START
  box::process_unaligned(COUNT, ci, co);
  STOP("unaligned")
test3:
  ci.reset_indices();
  co.reset_indices();
  fill_data(ci, co);
  START
  box::process_single(3, ci, co);
  ci.recalculate_offsets();
  box::process_shifted(COUNT-3, ci, co);
  STOP("shifted")
}

#define RUN_BOX(name, ina, inb, inc, outa, outb, outc) \
  int main()\
{\
  test<name>();\
  return 0; \
}


