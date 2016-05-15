#include <iostream>
#include <stdint.h>
#include "context.h"
#include <assert.h>
#include <stdlib.h>
#include "random_eng.h"


typedef context<ctx_indices<0,1,2,3,4,5,6,7>, ctx_types<uint32_t, uint32_t, uint32_t, uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t>, uint32_t,uint32_t,uint32_t,uint32_t, uint32_t, uint32_t, uint32_t,uint32_t> ctxin;
typedef context<ctx_indices<0,1,2,3>, ctx_types<uint32_t,uint32_t,uint32_t,uint32_t>, uint32_t,uint32_t,uint32_t,uint32_t> ctxout;

#define COUNT 10000
#define ALLOC 10050

void fill_data(ctxin& ci, ctxout& co)
{
  bool simple = true;
  random_engine rand;
  rand.reset();
  for(int j = 0; j < 8; j++)
  {
    for(int i = 0; i < ALLOC; i++)
    {
      (*((uint32_t**)ci.data[j]))[i] = rand.get();
    }
  }
}

#define O(i) ((*((uint32_t**)co.data[i]))[*co.index[i]])
#define I(i) ((*((uint32_t**)ci.data[i]))[*ci.index[i]])
#define OI(i) (*co.index[i])++
#define II(i) (*ci.index[i])++

void handwritten(int remaining, ctxin& ci, ctxout& co)
{
  for(;remaining > 0; remaining--)
  {
    for(int i = 0; i < 2; i++)
    {
      for(int j = 0; j < 2; j++)
      {
        O(i+2*j) = 0;
        for(int k = 0; k < 2; k++)
        {
          O(i+2*j) += I(2*j+k)*I(i+k+4);
        }
      }
    }
    for(int i = 0; i < 8; i++)
      II(i);
    for(int i = 0; i < 4; i++)
      OI(i);
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


  uint32_t* i0 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* i1 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* i2 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* i3 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* i4 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* i5 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* i6 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* i7 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* o0 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* o1 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* o2 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));
  uint32_t* o3 = (uint32_t*)aligned_alloc(16, ALLOC*sizeof(uint32_t));

  ctxin ci;
  ctxout co;

  int time, time2;

test0:
  ci.data_0 = i0;
  ci.data_1 = i1;
  ci.data_2 = i2;
  ci.data_3 = i3;
  ci.data_4 = i4;
  ci.data_5 = i5;
  ci.data_6 = i6;
  ci.data_7 = i7;
  co.data_0 = o0;
  co.data_1 = o1;
  co.data_2 = o2;
  co.data_3 = o3;
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
  ci.data_3 = &i3[2];
  ci.data_4 = &i4[0];
  ci.data_5 = &i5[1];
  ci.data_6 = &i6[2];
  ci.data_7 = &i7[1];
  co.data_0 = &o0[1];
  co.data_1 = &o1[1];
  co.data_2 = &o2[1];
  co.data_3 = &o3[1];
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


