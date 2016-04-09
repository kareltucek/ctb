#include <iostream>
#include <stdint.h>

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

