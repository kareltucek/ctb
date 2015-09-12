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
  for(int r = 0;r < 10000;++r)
  {
    std::srand(std::time(0));
    unsigned bsum = 0;
    /*decl for input 2*/;
    unsigned pos_in_2 = 0;
    unsigned size_in_2 = 0;
    unsigned processed_in_2 = 0;
    int * data_in_2 = NULL;
    bsum+=2;
    /*decl for input 1*/;
    unsigned pos_in_1 = 0;
    unsigned size_in_1 = 0;
    unsigned processed_in_1 = 0;
    int * data_in_1 = NULL;
    bsum+=1;
    /*decl for output 0*/;
    unsigned pos_out_0 = 0;
    unsigned size_out_0 = 0;
    unsigned processed_out_0 = 0;
    int * data_out_0 = NULL;
    for (int i = 0;i < 100 ;i++)
    {
      if ( pos_in_2 == size_in_2 )
      {
        size_in_2 = rand()%100;
        if(data_in_2 != NULL)delete [] data_in_2;
        data_in_2 = new int[size_in_2];
        pos_in_2 = 0;
        for(int i = 0;i < size_in_2;++i)
        {
          data_in_2[i] = processed_in_2;
          processed_in_2 += 2;
        }
      }
      if ( pos_in_1 == size_in_1 )
      {
        size_in_1 = rand()%100;
        if(data_in_1 != NULL)delete [] data_in_1;
        data_in_1 = new int[size_in_1];
        pos_in_1 = 0;
        for(int i = 0;i < size_in_1;++i)
        {
          data_in_1[i] = processed_in_1;
          processed_in_1 += 1;
        }
      }
      if ( pos_out_0 == size_out_0 )
      {
        for(int i = 0;i < size_out_0;++i)
        {
          //std::cout << 0 << " " << data_out_0[i] << std::endl;
          /*assert(data_out_0[i] == processed_out_0);
          */assert(data_out_0[i] == 2*processed_out_0*processed_out_0/3/3+processed_out_0);
          processed_out_0 += bsum;
        }
        size_out_0 = rand()%100;
        if(data_out_0 != NULL)delete [] data_out_0;
        data_out_0 = new int[size_out_0];
        pos_out_0 = 0;
      }
      std::size_t batch_size;
      batch_size = std::min(std::min(std::min(std::numeric_limits<unsigned>::max(), size_in_2 - pos_in_2), size_in_1 - pos_in_1), size_out_0 - pos_out_0);
      std::size_t j = 0;
      for ( ;j + 4 <= batch_size;j += 4)
      {
        __m128i var_LDI_0_w4_0 = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_2[pos_in_2+j]));
        __m128i var_LDI_1_w4_0 = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_1[pos_in_1+j]));
        __m128i var_ADDI_2_w4_0 = (__m128i)(_mm_add_epi32(var_LDI_0_w4_0, var_LDI_1_w4_0));
        __m128i var_LDI_4_conv_w2_0 = (__m128i)(var_LDI_0_w4_0);
        __m128i var_LDI_4_conv_w2_2 = (__m128i)(_mm_srli_si128(var_LDI_0_w4_0,4));
        __m128i var_LDI_5_conv_w2_0 = (__m128i)(var_LDI_1_w4_0);
        __m128i var_LDI_5_conv_w2_2 = (__m128i)(_mm_srli_si128(var_LDI_1_w4_0,4));
        __m128i var_MULI_3_w2_0 = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_5_conv_w2_0, var_LDI_4_conv_w2_0), _MM_SHUFFLE (0,0,2,0)));
        __m128i var_MULI_3_w2_2 = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_5_conv_w2_2, var_LDI_4_conv_w2_2), _MM_SHUFFLE (0,0,2,0)));
        __m128i var_MULI_7_conv_w4_0 = (__m128i)(_mm_unpacklo_epi32(var_MULI_3_w2_0,var_MULI_3_w2_2));
        __m128i var_ADDI_6_w4_0 = (__m128i)(_mm_add_epi32(var_MULI_7_conv_w4_0, var_ADDI_2_w4_0));
        _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j], var_ADDI_6_w4_0) /*var_STI_8_w4_0*/;
      }
      for (;j < batch_size;++j)
      {
        int var_LDI_9_w1_0 = (int)(data_in_2[pos_in_2+j]);
        int var_LDI_10_w1_0 = (int)(data_in_1[pos_in_1+j]);
        int var_ADDI_11_w1_0 = (int)(var_LDI_9_w1_0 + var_LDI_10_w1_0);
        int var_MULI_12_w1_0 = (int)(var_LDI_10_w1_0 * var_LDI_9_w1_0);
        int var_ADDI_13_w1_0 = (int)(var_MULI_12_w1_0 + var_ADDI_11_w1_0);
        data_out_0[pos_out_0+j] = var_ADDI_13_w1_0 /*var_STI_14_w1_0*/;
      }
      pos_in_2 += batch_size;
      pos_in_1 += batch_size;
      pos_out_0 += batch_size;
    }
  }
}
