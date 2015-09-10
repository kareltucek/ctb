#include <cstdlib>
#include <iostream>
#include <ctime>
#include <assert.h>
#include <stdint.h>
#include <limits>
#include <emmintrin.h>
#include <smmintrin.h>


void test_body() 
{
  for(int r = 0; r < 10000; ++r)
  {
    std::srand(std::time(0));
    unsigned bsum = 0;

    $4

      for (int i = 0; i < 100 ;i++)
      {
        $5

          std::size_t batch_size;
        batch_size = $6;

        std::size_t j = 0;
        for ( ; j + $10 <= batch_size; j += $10)
        {
          $7
        }
        for (; j < batch_size; ++j)
        {
          $8
        }

        $9
      }
  }
}

