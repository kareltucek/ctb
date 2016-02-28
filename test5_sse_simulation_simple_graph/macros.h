#include <iostream>

#define DEBUG_STD(var, name)\
  if(dbgframe < 0 || (cycles <= dbgframe && dbgframe < cycles+packsize)) \
std::cout << cycles << " " << name << " = " << (var) << " "<< std::endl;

#define DEBUG_SSE(var, name)\
  if(dbgframe < 0 || (cycles <= dbgframe && dbgframe < cycles+packsize)) \
{\
  int r[4];\
  _mm_storeu_si128((__m128i*)&r, (__m128i)(var));\
  std::cout << name << " = ";\
  for(int i = 3; i >= 0; i--)\
  write(r[i]);\
  std::cout << " " << std::endl;\
}

#define load_bool(ptr,idx) \
  (0<(((uint32_t*)ptr)[(idx)/32] & (1 << ((idx)%32))))

#define store_bool(ptr,idx,val) \
  ptr[(idx)/32] = (ptr[(idx)/32] & (~(1<<((idx)%32)))) | ((val > 0 ? 1 : 0) << ((idx)%32))\


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

