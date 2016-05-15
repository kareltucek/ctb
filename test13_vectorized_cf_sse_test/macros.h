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


void write(uint32_t i)
{
  uint32_t m = 0x80000000;
  while(m>0)
  {
    std::cout << ((m&i) > 0 ? "1" : "0");
    m = m/2;
  }
  std::cout << " aka " << i << " ";
}

typedef context<ctx_indices<0,1,2>, ctx_types<int32_t, int32_t, bool>, int32_t, int32_t, bool> ctxin;
typedef context<ctx_indices<0>, ctx_types<int32_t>, int32_t> ctxout;

void check_inputs(ctxin& ci, ctxout& co, bool simple)
{
  random_engine rand;
  rand.reset();
  for(int i = 0; i < 150; i++)
  {
    int s = simple ? i : rand.get();
    if(ci.data_0[i] != s % 100)
      std::cout << "//======= corrupted input data at index " << i <<std::endl;
    if(ci.data_1[i] != s % 100)
      std::cout << "//======= corrupted input data at index " << i <<std::endl;
    if(ci.data_2[i] != s % 3 == 0)
      std::cout << "//======= corrupted input data at index " << i <<std::endl;
  }
}


void fill_data(ctxin& ci, ctxout& co)
{
  bool simple = true;
  random_engine rand;
  rand.reset();
  for(int i = 0; i < 150; i++)
  {
    int s = simple ? i : rand.get();
    ci.data_0[i] = s % 100;
    ci.data_1[i] = s % 100;
    ci.data_2[i] = s % 3 == 0;
  }
  check_inputs(ci, co, simple);
}

void check_results(ctxin& ci, ctxout& co)
{
  bool failed = false;
  for(int i = 0; i < 100; i++)
  {
    int expected = 3*(ci.data_0[i] + ci.data_1[i]);
    if(co.data_0[i] != expected)
    {
      failed = true;
      std::cout << "//====== at index " << i << " obtained " << co.data_0[i] << " expected " << expected << std::endl;
    }
    else
      std::cout << "//====== at index " << i << " obtained " << co.data_0[i] << " ok " << std::endl;
  }
  if(failed)
    exit(1);
}

  template <template <typename...> class BOX>
void test()
{
  typedef BOX<ctxin, ctxout> box;

  std::cout << "//====== allocating space for test" << std::endl;

  int32_t* i0 = (int32_t*)aligned_alloc(16, 150*sizeof(int32_t));
  int32_t* i1 = (int32_t*)aligned_alloc(16, 150*sizeof(int32_t));
  int32_t* o0 = (int32_t*)aligned_alloc(16, 150*sizeof(int32_t));
  bool*    i2 = (bool*)aligned_alloc(16, 150*sizeof(bool));

  ctxin ci;
  ctxout co;

test0:
  std::cout << "//====== setting up single test" << std::endl;
  ci.data_0 = i0;
  ci.data_1 = i1;
  ci.data_2 = i2;
  co.data_0 = o0;
  ci.reset_indices();
  co.reset_indices();
  fill_data(ci, co);
  std::cout << "//====== running test" << std::endl;
  box::process_single(100, ci, co);
  std::cout << "//====== checking results" << std::endl;
  check_results(ci, co);
  std::cout << "//====== ok" << std::endl;
test1:
  std::cout << "//====== setting up aligned test" << std::endl;
  ci.reset_indices();
  co.reset_indices();
  fill_data(ci, co);
  std::cout << "//====== running test" << std::endl;
  box::process_aligned(100, ci, co);
  std::cout << "//====== checking results" << std::endl;
  check_results(ci, co);
  std::cout << "//====== ok" << std::endl;
test2:
  std::cout << "//====== setting up unaligned test" << std::endl;
  ci.reset_indices();
  co.reset_indices();
  fill_data(ci, co);
  std::cout << "//====== running test" << std::endl;
  box::process_unaligned(100, ci, co);
  std::cout << "//====== checking results" << std::endl;
  check_results(ci, co);
  std::cout << "//====== ok" << std::endl;
test3:
  std::cout << "//====== setting up shifted test" << std::endl;
  ci.data_0 = &i0[0];
  ci.data_1 = &i1[1];
  ci.data_2 = &i2[2];
  co.data_0 = &o0[1];
  ci.reset_indices();
  co.reset_indices();
  fill_data(ci, co);
  std::cout << "//====== processing first records" << std::endl;
  box::process_single(3, ci, co);
  std::cout << "//====== running test" << std::endl;
  ci.recalculate_offsets();
  box::process_shifted(97, ci, co);
  std::cout << "//====== checking results" << std::endl;
  check_results(ci, co);
  std::cout << "//====== ok" << std::endl;
}

#define RUN_BOX(name, ina, inb, inc, outa, outb, outc) \
  int main()\
{\
  test<name>();\
  return 0; \
}


