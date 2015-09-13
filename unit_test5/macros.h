#include <iostream>

#define DEBUG_STD(var, name)\
  if(dbgframe < 0 || (cycles <= dbgframe && dbgframe < cycles+packsize)) \
std::cout << name << " = " << var << std::endl;

#define DEBUG_SSE(var, name)\
  if(dbgframe < 0 || (cycles <= dbgframe && dbgframe < cycles+packsize)) \
{\
  int r[4];\
  _mm_storeu_si128((__m128i*)&r, (__m128i)var);\
  std::cout << name << " = ";\
  for(int i = 3; i >= 0; i--)\
  write(r[i]);\
  std::cout << std::endl;\
}

void write(uint32_t i)
{
  uint32_t m = 0x80000000;
  while(m>0)
  {
    std::cout << ((m&i) > 0 ? "1" : "0");
    m = m/2;
  }
  std::cout << " ";
}

