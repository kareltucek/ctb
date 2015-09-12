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
#define load_bool(a) true
#define store_bool(a) true
class tester
{
private:
  std::vector<int32_t> data_check_0;
  void test_body1()
  {
    for(int r = 0;r < 10000;++r)
    {
      std::srand(std::time(0));
      /*decl for input 2*/;
      unsigned pos_in_2 = 0;
      unsigned size_in_2 = 0;
      int32_t * data_in_2 = NULL;
      /*decl for input 1*/;
      unsigned pos_in_1 = 0;
      unsigned size_in_1 = 0;
      int32_t * data_in_1 = NULL;
      /*decl for output 0*/;
      unsigned pos_out_0 = 0;
      unsigned size_out_0 = 0;
      unsigned processed_out_0 = 0;
      int32_t * data_out_0 = NULL;
      for (int i = 0;i < 100 ;i++)
      {
        if ( pos_in_2 == size_in_2 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_2 = (rand()%16)*32;
          }
          else
          {
            size_in_2 = rand()%100;
          }
          if(data_in_2 != NULL)
          {
            delete [] data_in_2;
          }
          data_in_2 = new int32_t[size_in_2];
          pos_in_2 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_2;++i)data_in_2[i] = (rand() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_2/32;++i)data_in_2[i] = rand();
          }
          else
          {
            for(int i = 0;i < size_in_2;++i)
            {
              data_in_2[i] = (int32_t)rand();
              if(data_in_2[i] == 0) /*division by zero*/
              {
                data_in_2[i]++;
              }
            }
          }
        }
        if ( pos_in_1 == size_in_1 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_1 = (rand()%16)*32;
          }
          else
          {
            size_in_1 = rand()%100;
          }
          if(data_in_1 != NULL)
          {
            delete [] data_in_1;
          }
          data_in_1 = new int32_t[size_in_1];
          pos_in_1 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_1;++i)data_in_1[i] = (rand() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_1/32;++i)data_in_1[i] = rand();
          }
          else
          {
            for(int i = 0;i < size_in_1;++i)
            {
              data_in_1[i] = (int32_t)rand();
              if(data_in_1[i] == 0) /*division by zero*/
              {
                data_in_1[i]++;
              }
            }
          }
        }
        if ( pos_out_0 == size_out_0 )
        {
          int mod = 1;
          if(std::string("int32_t") == "bool")
          {
            mod = 32;
          }
          for(int i = 0;i < size_out_0/mod;++i)
          {
            if(1 == 1)
            {
              data_check_0.push_back(data_out_0[i]);
              processed_out_0++;
            }
            else
            {
              assert(data_out_0[i] == data_check_0[processed_out_0]);
              processed_out_0++;
            }
          }
          if(std::string("int32_t") == "bool")
          {
            size_out_0 = (rand()%16)*32;
          }
          else
          {
            size_out_0 = rand()%100;
          }
          if(data_out_0 != NULL)
          {
            delete [] data_out_0;
          }
          data_out_0 = new int32_t[size_out_0];
          pos_out_0 = 0;
        }
        std::size_t batch_size;
        batch_size = std::min(std::min(std::min(std::numeric_limits<unsigned>::max(), size_in_2 - pos_in_2), size_in_1 - pos_in_1), size_out_0 - pos_out_0);
        std::size_t j = 0;
        for ( ;j + 1 <= batch_size;j += 1)
        {
          int32_t var_LD_int32_t_0_w1_0 = (int32_t)(data_in_2[pos_in_2+j]);
          int32_t var_LD_int32_t_1_w1_0 = (int32_t)(data_in_1[pos_in_1+j]);
          int32_t var_ADD_int32_t_2_w1_0 = (int32_t)(var_LD_int32_t_0_w1_0 + var_LD_int32_t_1_w1_0);
          int32_t var_MUL_int32_t_3_w1_0 = (int32_t)(var_LD_int32_t_1_w1_0 * var_LD_int32_t_0_w1_0);
          int32_t var_ADD_int32_t_4_w1_0 = (int32_t)(var_MUL_int32_t_3_w1_0 + var_ADD_int32_t_2_w1_0);
          data_out_0[pos_out_0+j] = var_ADD_int32_t_4_w1_0 /*var_ST_int32_t_5_w1_0*/;
        }
        for (;j < batch_size;++j)
        {
        }
        pos_in_2 += batch_size;
        pos_in_1 += batch_size;
        pos_out_0 += batch_size;
      }
    }
  }
  void test_body2()
  {
    for(int r = 0;r < 10000;++r)
    {
      std::srand(std::time(0));
      /*decl for input 2*/;
      unsigned pos_in_2 = 0;
      unsigned size_in_2 = 0;
      int32_t * data_in_2 = NULL;
      /*decl for input 1*/;
      unsigned pos_in_1 = 0;
      unsigned size_in_1 = 0;
      int32_t * data_in_1 = NULL;
      /*decl for output 0*/;
      unsigned pos_out_0 = 0;
      unsigned size_out_0 = 0;
      unsigned processed_out_0 = 0;
      int32_t * data_out_0 = NULL;
      for (int i = 0;i < 100 ;i++)
      {
        if ( pos_in_2 == size_in_2 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_2 = (rand()%16)*32;
          }
          else
          {
            size_in_2 = rand()%100;
          }
          if(data_in_2 != NULL)
          {
            delete [] data_in_2;
          }
          data_in_2 = new int32_t[size_in_2];
          pos_in_2 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_2;++i)data_in_2[i] = (rand() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_2/32;++i)data_in_2[i] = rand();
          }
          else
          {
            for(int i = 0;i < size_in_2;++i)
            {
              data_in_2[i] = (int32_t)rand();
              if(data_in_2[i] == 0) /*division by zero*/
              {
                data_in_2[i]++;
              }
            }
          }
        }
        if ( pos_in_1 == size_in_1 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_1 = (rand()%16)*32;
          }
          else
          {
            size_in_1 = rand()%100;
          }
          if(data_in_1 != NULL)
          {
            delete [] data_in_1;
          }
          data_in_1 = new int32_t[size_in_1];
          pos_in_1 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_1;++i)data_in_1[i] = (rand() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_1/32;++i)data_in_1[i] = rand();
          }
          else
          {
            for(int i = 0;i < size_in_1;++i)
            {
              data_in_1[i] = (int32_t)rand();
              if(data_in_1[i] == 0) /*division by zero*/
              {
                data_in_1[i]++;
              }
            }
          }
        }
        if ( pos_out_0 == size_out_0 )
        {
          int mod = 1;
          if(std::string("int32_t") == "bool")
          {
            mod = 32;
          }
          for(int i = 0;i < size_out_0/mod;++i)
          {
            if(2 == 1)
            {
              data_check_0.push_back(data_out_0[i]);
              processed_out_0++;
            }
            else
            {
              assert(data_out_0[i] == data_check_0[processed_out_0]);
              processed_out_0++;
            }
          }
          if(std::string("int32_t") == "bool")
          {
            size_out_0 = (rand()%16)*32;
          }
          else
          {
            size_out_0 = rand()%100;
          }
          if(data_out_0 != NULL)
          {
            delete [] data_out_0;
          }
          data_out_0 = new int32_t[size_out_0];
          pos_out_0 = 0;
        }
        std::size_t batch_size;
        batch_size = std::min(std::min(std::min(std::numeric_limits<unsigned>::max(), size_in_2 - pos_in_2), size_in_1 - pos_in_1), size_out_0 - pos_out_0);
        std::size_t j = 0;
        for ( ;j + 2 <= batch_size;j += 2)
        {
          int32_t var_LD_int32_t_6_w1_0 = (int32_t)(data_in_2[pos_in_2+j]);
          int32_t var_LD_int32_t_6_w1_1 = (int32_t)(data_in_2[pos_in_2+j]);
          int32_t var_LD_int32_t_7_w1_0 = (int32_t)(data_in_1[pos_in_1+j]);
          int32_t var_LD_int32_t_7_w1_1 = (int32_t)(data_in_1[pos_in_1+j]);
          __m128i var_LD_int32_t_9_conv_w2_0 = (__m128i)(_mm_set_epi32(0,var_LD_int32_t_7_w1_1,0,var_LD_int32_t_7_w1_0));
          __m128i var_LD_int32_t_10_conv_w2_0 = (__m128i)(_mm_set_epi32(0,var_LD_int32_t_6_w1_1,0,var_LD_int32_t_6_w1_0));
          __m128i var_ADD_int32_t_8_w2_0 = (__m128i)(_mm_add_epi32(var_LD_int32_t_10_conv_w2_0,var_LD_int32_t_9_conv_w2_0));
          __m128i var_MUL_int32_t_11_w2_0 = (__m128i)(_mm_mul_epi32(var_LD_int32_t_9_conv_w2_0,var_LD_int32_t_10_conv_w2_0));
          __m128i var_ADD_int32_t_12_w2_0 = (__m128i)(_mm_add_epi32(var_MUL_int32_t_11_w2_0,var_ADD_int32_t_8_w2_0));
          int32_t var_ADD_int32_t_14_conv_w1_0 = (int32_t)((int32_t)_mm_extract_epi32((__m128i)var_ADD_int32_t_12_w2_0,0));
          int32_t var_ADD_int32_t_14_conv_w1_1 = (int32_t)((int32_t)_mm_extract_epi32((__m128i)var_ADD_int32_t_12_w2_0,2));
          data_out_0[pos_out_0+j] = var_ADD_int32_t_14_conv_w1_0 /*var_ST_int32_t_13_w1_0*/;
          data_out_0[pos_out_0+j] = var_ADD_int32_t_14_conv_w1_1 /*var_ST_int32_t_13_w1_1*/;
        }
        for (;j < batch_size;++j)
        {
          int32_t var_LD_int32_t_15_w1_0 = (int32_t)(data_in_2[pos_in_2+j]);
          int32_t var_LD_int32_t_16_w1_0 = (int32_t)(data_in_1[pos_in_1+j]);
          int32_t var_ADD_int32_t_17_w1_0 = (int32_t)(var_LD_int32_t_15_w1_0 + var_LD_int32_t_16_w1_0);
          int32_t var_MUL_int32_t_18_w1_0 = (int32_t)(var_LD_int32_t_16_w1_0 * var_LD_int32_t_15_w1_0);
          int32_t var_ADD_int32_t_19_w1_0 = (int32_t)(var_MUL_int32_t_18_w1_0 + var_ADD_int32_t_17_w1_0);
          data_out_0[pos_out_0+j] = var_ADD_int32_t_19_w1_0 /*var_ST_int32_t_20_w1_0*/;
        }
        pos_in_2 += batch_size;
        pos_in_1 += batch_size;
        pos_out_0 += batch_size;
      }
    }
  }
  void test_body4()
  {
    for(int r = 0;r < 10000;++r)
    {
      std::srand(std::time(0));
      /*decl for input 2*/;
      unsigned pos_in_2 = 0;
      unsigned size_in_2 = 0;
      int32_t * data_in_2 = NULL;
      /*decl for input 1*/;
      unsigned pos_in_1 = 0;
      unsigned size_in_1 = 0;
      int32_t * data_in_1 = NULL;
      /*decl for output 0*/;
      unsigned pos_out_0 = 0;
      unsigned size_out_0 = 0;
      unsigned processed_out_0 = 0;
      int32_t * data_out_0 = NULL;
      for (int i = 0;i < 100 ;i++)
      {
        if ( pos_in_2 == size_in_2 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_2 = (rand()%16)*32;
          }
          else
          {
            size_in_2 = rand()%100;
          }
          if(data_in_2 != NULL)
          {
            delete [] data_in_2;
          }
          data_in_2 = new int32_t[size_in_2];
          pos_in_2 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_2;++i)data_in_2[i] = (rand() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_2/32;++i)data_in_2[i] = rand();
          }
          else
          {
            for(int i = 0;i < size_in_2;++i)
            {
              data_in_2[i] = (int32_t)rand();
              if(data_in_2[i] == 0) /*division by zero*/
              {
                data_in_2[i]++;
              }
            }
          }
        }
        if ( pos_in_1 == size_in_1 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_1 = (rand()%16)*32;
          }
          else
          {
            size_in_1 = rand()%100;
          }
          if(data_in_1 != NULL)
          {
            delete [] data_in_1;
          }
          data_in_1 = new int32_t[size_in_1];
          pos_in_1 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_1;++i)data_in_1[i] = (rand() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_1/32;++i)data_in_1[i] = rand();
          }
          else
          {
            for(int i = 0;i < size_in_1;++i)
            {
              data_in_1[i] = (int32_t)rand();
              if(data_in_1[i] == 0) /*division by zero*/
              {
                data_in_1[i]++;
              }
            }
          }
        }
        if ( pos_out_0 == size_out_0 )
        {
          int mod = 1;
          if(std::string("int32_t") == "bool")
          {
            mod = 32;
          }
          for(int i = 0;i < size_out_0/mod;++i)
          {
            if(4 == 1)
            {
              data_check_0.push_back(data_out_0[i]);
              processed_out_0++;
            }
            else
            {
              assert(data_out_0[i] == data_check_0[processed_out_0]);
              processed_out_0++;
            }
          }
          if(std::string("int32_t") == "bool")
          {
            size_out_0 = (rand()%16)*32;
          }
          else
          {
            size_out_0 = rand()%100;
          }
          if(data_out_0 != NULL)
          {
            delete [] data_out_0;
          }
          data_out_0 = new int32_t[size_out_0];
          pos_out_0 = 0;
        }
        std::size_t batch_size;
        batch_size = std::min(std::min(std::min(std::numeric_limits<unsigned>::max(), size_in_2 - pos_in_2), size_in_1 - pos_in_1), size_out_0 - pos_out_0);
        std::size_t j = 0;
        for ( ;j + 4 <= batch_size;j += 4)
        {
          __m128i var_LD_int32_t_21_w4_0 = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_2[pos_in_2+j]));
          __m128i var_LD_int32_t_22_w4_0 = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_1[pos_in_1+j]));
          __m128i var_ADD_int32_t_23_w4_0 = (__m128i)(_mm_add_epi32(var_LD_int32_t_21_w4_0,var_LD_int32_t_22_w4_0));
          __m128i var_LD_int32_t_25_conv_w2_0 = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_21_w4_0,0x50)));
          __m128i var_LD_int32_t_25_conv_w2_2 = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_21_w4_0,0xFA)));
          __m128i var_LD_int32_t_26_conv_w2_0 = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_22_w4_0,0x50)));
          __m128i var_LD_int32_t_26_conv_w2_2 = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_22_w4_0,0xFA)));
          __m128i var_MUL_int32_t_24_w2_0 = (__m128i)(_mm_mul_epi32(var_LD_int32_t_26_conv_w2_0,var_LD_int32_t_25_conv_w2_0));
          __m128i var_MUL_int32_t_24_w2_2 = (__m128i)(_mm_mul_epi32(var_LD_int32_t_26_conv_w2_2,var_LD_int32_t_25_conv_w2_2));
          __m128i var_MUL_int32_t_28_conv_w4_0 = (__m128i)(_mm_movelh_ps((__m128) _mm_shuffle_epi32(var_MUL_int32_t_24_w2_0,0x08),(__m128) _mm_shuffle_epi32(var_MUL_int32_t_24_w2_2,0x08)));
          __m128i var_ADD_int32_t_27_w4_0 = (__m128i)(_mm_add_epi32(var_MUL_int32_t_28_conv_w4_0,var_ADD_int32_t_23_w4_0));
          _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j],var_ADD_int32_t_27_w4_0) /*var_ST_int32_t_29_w4_0*/;
        }
        for (;j < batch_size;++j)
        {
          int32_t var_LD_int32_t_30_w1_0 = (int32_t)(data_in_2[pos_in_2+j]);
          int32_t var_LD_int32_t_31_w1_0 = (int32_t)(data_in_1[pos_in_1+j]);
          int32_t var_ADD_int32_t_32_w1_0 = (int32_t)(var_LD_int32_t_30_w1_0 + var_LD_int32_t_31_w1_0);
          int32_t var_MUL_int32_t_33_w1_0 = (int32_t)(var_LD_int32_t_31_w1_0 * var_LD_int32_t_30_w1_0);
          int32_t var_ADD_int32_t_34_w1_0 = (int32_t)(var_MUL_int32_t_33_w1_0 + var_ADD_int32_t_32_w1_0);
          data_out_0[pos_out_0+j] = var_ADD_int32_t_34_w1_0 /*var_ST_int32_t_35_w1_0*/;
        }
        pos_in_2 += batch_size;
        pos_in_1 += batch_size;
        pos_out_0 += batch_size;
      }
    }
  }
public:
  void test()
  {
    srand(0);
    test_body1();
    test_body2();
    test_body4();
  }
}
;
