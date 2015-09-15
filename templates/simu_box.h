#include <vector>
#include <cstdlib>
#include <stdlib.h> 
#include <iostream>
#include <ctime>
#include <assert.h>
#include <stdint.h>
#include <limits>
#include <emmintrin.h>
#include <smmintrin.h>


#include "macros.h"
#include "random_eng.h"

class tester
{
  private:
  int dbgframe;
  random_engine data_source;

  $1
  $2

  uint32_t pull()
  {
    return data_source.get() % 100;
  }

  void reset()
  {
    data_source.reset();
  }

  public:
  void test(int frame = -1)
  {
    dbgframe = frame;

    $3
  }
};


