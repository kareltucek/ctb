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
  std::vector<int32_t> data_check_0;
  void test_body1()
  {
    std::cout << "processing width " << 1 << std::endl;
    reset();
    int cycles = 0;
    const int packsize = 1;
    for(int r = 0;r < 1;++r)
    {
      /*decl for input 2 with name a_in*/;
      unsigned pos_in_2 = 0;
      unsigned size_in_2 = 0;
      int32_t * data_in_2 = NULL;
      /*decl for input 1 with name d_in*/;
      unsigned pos_in_1 = 0;
      unsigned size_in_1 = 0;
      int32_t * data_in_1 = NULL;
      /*decl for output 0 with name c_out*/;
      unsigned pos_out_0 = 0;
      unsigned size_out_0 = 0;
      unsigned processed_out_0 = 0;
      int32_t * data_out_0 = NULL;
      for (int i = 0;i < 10 ;i++)
      {
        /*std::cout << "at cycle " << cycles << std::endl*/;
        if ( pos_in_2 == size_in_2 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_2 = (pull()%20+10)*32;
          }
          else
          {
            size_in_2 = 50+pull()%50;
          }
          if(data_in_2 != NULL)
          {
            delete [] data_in_2;
          }
          data_in_2 = new int32_t[size_in_2];
          pos_in_2 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_2;++i)data_in_2[i] = (pull() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_2;++i) /*this will serve for both vector and bool*/data_in_2[i] = pull();
          }
          else
          {
            for(int i = 0;i < size_in_2;++i)
            {
              data_in_2[i] = (int32_t)pull();
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
            size_in_1 = (pull()%20+10)*32;
          }
          else
          {
            size_in_1 = 50+pull()%50;
          }
          if(data_in_1 != NULL)
          {
            delete [] data_in_1;
          }
          data_in_1 = new int32_t[size_in_1];
          pos_in_1 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_1;++i)data_in_1[i] = (pull() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_1;++i) /*this will serve for both vector and bool*/data_in_1[i] = pull();
          }
          else
          {
            for(int i = 0;i < size_in_1;++i)
            {
              data_in_1[i] = (int32_t)pull();
              if(data_in_1[i] == 0) /*division by zero*/
              {
                data_in_1[i]++;
              }
            }
          }
        }
        if ( pos_out_0 == size_out_0 )
        {
          /*would be nices as a macro*/cycles -= size_out_0;
          if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_out_0;++i)
            {
              if(1 == 1)
              {
                data_check_0.push_back(load_bool(data_out_0,i));
                processed_out_0++;
              }
              else
              {
                if(load_bool(data_out_0,i) != data_check_0[processed_out_0])
                {
                  DEBUG_STD((int)load_bool(data_out_0,i), "c_out OUTPUT_0");
                  DEBUG_STD((int)data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  std::cout << "failed in cycle " << cycles << std::endl;
                  throw (int)cycles;
                }
                ;
                processed_out_0++;
              }
              cycles++;
            }
          }
          else
          {
            for(int i = 0;i < size_out_0;++i)
            {
              if(1 == 1)
              {
                data_check_0.push_back(data_out_0[i]);
                processed_out_0++;
              }
              else
              {
                if(data_out_0[i] != data_check_0[processed_out_0])
                {
                  if(std::string("int32_t") == "ssebool")
                  {
                    DEBUG_STD((int)data_out_0[i], "c_out OUTPUT_0");
                    DEBUG_STD((int)data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  }
                  else
                  {
                    DEBUG_STD(data_out_0[i], "c_out OUTPUT_0");
                    DEBUG_STD(data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  }
                  std::cout << "failed in cycle " << cycles << std::endl;
                  throw (int)cycles;
                }
                ;
                processed_out_0++;
              }
              cycles++;
            }
          }
          if(std::string("int32_t") == "bool")
          {
            size_out_0 = (10+pull()%16)*32;
          }
          else
          {
            size_out_0 = 50+pull()%100;
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
          int32_t var_LD_int32_t_id2_tw1_0_ata_in = (int32_t)(data_in_2[pos_in_2+j+0]);
          DEBUG_STD(var_LD_int32_t_id2_tw1_0_ata_in,"var_LD_int32_t_id2_tw1_0_ata_in") /*var_DEBUG_int32_t_id3_tw1_0_atDEBUG1_DEBUG_int32_t*/;
          int32_t var_LD_int32_t_id4_tw1_0_atd_in = (int32_t)(data_in_1[pos_in_1+j+0]);
          int32_t var_ADD_int32_t_id5_tw1_0_atb_pl = (int32_t)(var_LD_int32_t_id2_tw1_0_ata_in + var_LD_int32_t_id4_tw1_0_atd_in);
          int32_t var_MUL_int32_t_id6_tw1_0_atb_ml = (int32_t)(var_LD_int32_t_id4_tw1_0_atd_in * var_LD_int32_t_id2_tw1_0_ata_in);
          DEBUG_STD(var_LD_int32_t_id4_tw1_0_atd_in,"var_LD_int32_t_id4_tw1_0_atd_in") /*var_DEBUG_int32_t_id7_tw1_0_atDEBUG4_DEBUG_int32_t*/;
          int32_t var_ADD_int32_t_id8_tw1_0_ate_pl = (int32_t)(var_MUL_int32_t_id6_tw1_0_atb_ml + var_ADD_int32_t_id5_tw1_0_atb_pl);
          DEBUG_STD(var_ADD_int32_t_id5_tw1_0_atb_pl,"var_ADD_int32_t_id5_tw1_0_atb_pl") /*var_DEBUG_int32_t_id9_tw1_0_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_STD(var_MUL_int32_t_id6_tw1_0_atb_ml,"var_MUL_int32_t_id6_tw1_0_atb_ml") /*var_DEBUG_int32_t_id10_tw1_0_atDEBUG2_DEBUG_int32_t*/;
          data_out_0[pos_out_0+j+0] = var_ADD_int32_t_id8_tw1_0_ate_pl /*var_ST_int32_t_id11_tw1_0_atc_out*/;
          DEBUG_STD(var_ADD_int32_t_id8_tw1_0_ate_pl,"var_ADD_int32_t_id8_tw1_0_ate_pl") /*var_DEBUG_int32_t_id12_tw1_0_atDEBUG5_DEBUG_int32_t*/;
          cycles += 1;
        }
        for (;j < batch_size;++j)
        {
          cycles++;
        }
        pos_in_2 += batch_size;
        pos_in_1 += batch_size;
        pos_out_0 += batch_size;
      }
    }
  }
  void test_body2()
  {
    std::cout << "processing width " << 2 << std::endl;
    reset();
    int cycles = 0;
    const int packsize = 2;
    for(int r = 0;r < 1;++r)
    {
      /*decl for input 2 with name a_in*/;
      unsigned pos_in_2 = 0;
      unsigned size_in_2 = 0;
      int32_t * data_in_2 = NULL;
      /*decl for input 1 with name d_in*/;
      unsigned pos_in_1 = 0;
      unsigned size_in_1 = 0;
      int32_t * data_in_1 = NULL;
      /*decl for output 0 with name c_out*/;
      unsigned pos_out_0 = 0;
      unsigned size_out_0 = 0;
      unsigned processed_out_0 = 0;
      int32_t * data_out_0 = NULL;
      for (int i = 0;i < 10 ;i++)
      {
        /*std::cout << "at cycle " << cycles << std::endl*/;
        if ( pos_in_2 == size_in_2 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_2 = (pull()%20+10)*32;
          }
          else
          {
            size_in_2 = 50+pull()%50;
          }
          if(data_in_2 != NULL)
          {
            delete [] data_in_2;
          }
          data_in_2 = new int32_t[size_in_2];
          pos_in_2 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_2;++i)data_in_2[i] = (pull() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_2;++i) /*this will serve for both vector and bool*/data_in_2[i] = pull();
          }
          else
          {
            for(int i = 0;i < size_in_2;++i)
            {
              data_in_2[i] = (int32_t)pull();
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
            size_in_1 = (pull()%20+10)*32;
          }
          else
          {
            size_in_1 = 50+pull()%50;
          }
          if(data_in_1 != NULL)
          {
            delete [] data_in_1;
          }
          data_in_1 = new int32_t[size_in_1];
          pos_in_1 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_1;++i)data_in_1[i] = (pull() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_1;++i) /*this will serve for both vector and bool*/data_in_1[i] = pull();
          }
          else
          {
            for(int i = 0;i < size_in_1;++i)
            {
              data_in_1[i] = (int32_t)pull();
              if(data_in_1[i] == 0) /*division by zero*/
              {
                data_in_1[i]++;
              }
            }
          }
        }
        if ( pos_out_0 == size_out_0 )
        {
          /*would be nices as a macro*/cycles -= size_out_0;
          if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_out_0;++i)
            {
              if(2 == 1)
              {
                data_check_0.push_back(load_bool(data_out_0,i));
                processed_out_0++;
              }
              else
              {
                if(load_bool(data_out_0,i) != data_check_0[processed_out_0])
                {
                  DEBUG_STD((int)load_bool(data_out_0,i), "c_out OUTPUT_0");
                  DEBUG_STD((int)data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  std::cout << "failed in cycle " << cycles << std::endl;
                  throw (int)cycles;
                }
                ;
                processed_out_0++;
              }
              cycles++;
            }
          }
          else
          {
            for(int i = 0;i < size_out_0;++i)
            {
              if(2 == 1)
              {
                data_check_0.push_back(data_out_0[i]);
                processed_out_0++;
              }
              else
              {
                if(data_out_0[i] != data_check_0[processed_out_0])
                {
                  if(std::string("int32_t") == "ssebool")
                  {
                    DEBUG_STD((int)data_out_0[i], "c_out OUTPUT_0");
                    DEBUG_STD((int)data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  }
                  else
                  {
                    DEBUG_STD(data_out_0[i], "c_out OUTPUT_0");
                    DEBUG_STD(data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  }
                  std::cout << "failed in cycle " << cycles << std::endl;
                  throw (int)cycles;
                }
                ;
                processed_out_0++;
              }
              cycles++;
            }
          }
          if(std::string("int32_t") == "bool")
          {
            size_out_0 = (10+pull()%16)*32;
          }
          else
          {
            size_out_0 = 50+pull()%100;
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
          int32_t var_LD_int32_t_id2_tw1_0_ata_in = (int32_t)(data_in_2[pos_in_2+j+0]);
          int32_t var_LD_int32_t_id2_tw1_1_ata_in = (int32_t)(data_in_2[pos_in_2+j+1]);
          DEBUG_STD(var_LD_int32_t_id2_tw1_0_ata_in,"var_LD_int32_t_id2_tw1_0_ata_in") /*var_DEBUG_int32_t_id3_tw1_0_atDEBUG1_DEBUG_int32_t*/;
          DEBUG_STD(var_LD_int32_t_id2_tw1_1_ata_in,"var_LD_int32_t_id2_tw1_1_ata_in") /*var_DEBUG_int32_t_id3_tw1_1_atDEBUG1_DEBUG_int32_t*/;
          int32_t var_LD_int32_t_id4_tw1_0_atd_in = (int32_t)(data_in_1[pos_in_1+j+0]);
          int32_t var_LD_int32_t_id4_tw1_1_atd_in = (int32_t)(data_in_1[pos_in_1+j+1]);
          __m128i var_LD_int32_t_id6_tconv_w2_0_atd_in = (__m128i)(_mm_set_epi32(0,var_LD_int32_t_id4_tw1_1_atd_in,0,var_LD_int32_t_id4_tw1_0_atd_in));
          __m128i var_LD_int32_t_id7_tconv_w2_0_ata_in = (__m128i)(_mm_set_epi32(0,var_LD_int32_t_id2_tw1_1_ata_in,0,var_LD_int32_t_id2_tw1_0_ata_in));
          __m128i var_ADD_int32_t_id5_tw2_0_atb_pl = (__m128i)(_mm_add_epi32(var_LD_int32_t_id7_tconv_w2_0_ata_in,var_LD_int32_t_id6_tconv_w2_0_atd_in));
          __m128i var_MUL_int32_t_id8_tw2_0_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id6_tconv_w2_0_atd_in,var_LD_int32_t_id7_tconv_w2_0_ata_in));
          DEBUG_STD(var_LD_int32_t_id4_tw1_0_atd_in,"var_LD_int32_t_id4_tw1_0_atd_in") /*var_DEBUG_int32_t_id9_tw1_0_atDEBUG4_DEBUG_int32_t*/;
          DEBUG_STD(var_LD_int32_t_id4_tw1_1_atd_in,"var_LD_int32_t_id4_tw1_1_atd_in") /*var_DEBUG_int32_t_id9_tw1_1_atDEBUG4_DEBUG_int32_t*/;
          __m128i var_ADD_int32_t_id10_tw2_0_ate_pl = (__m128i)(_mm_add_epi32(var_MUL_int32_t_id8_tw2_0_atb_ml,var_ADD_int32_t_id5_tw2_0_atb_pl));
          DEBUG_SSE(var_ADD_int32_t_id5_tw2_0_atb_pl,"var_ADD_int32_t_id5_tw2_0_atb_pl") /*var_DEBUG_int32_t_id11_tw2_0_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id8_tw2_0_atb_ml,"var_MUL_int32_t_id8_tw2_0_atb_ml") /*var_DEBUG_int32_t_id12_tw2_0_atDEBUG2_DEBUG_int32_t*/;
          int32_t var_ADD_int32_t_id14_tconv_w1_0_ate_pl = (int32_t)((int32_t)_mm_extract_epi32((__m128i)var_ADD_int32_t_id10_tw2_0_ate_pl,0));
          int32_t var_ADD_int32_t_id14_tconv_w1_1_ate_pl = (int32_t)((int32_t)_mm_extract_epi32((__m128i)var_ADD_int32_t_id10_tw2_0_ate_pl,2));
          data_out_0[pos_out_0+j+0] = var_ADD_int32_t_id14_tconv_w1_0_ate_pl /*var_ST_int32_t_id13_tw1_0_atc_out*/;
          data_out_0[pos_out_0+j+1] = var_ADD_int32_t_id14_tconv_w1_1_ate_pl /*var_ST_int32_t_id13_tw1_1_atc_out*/;
          DEBUG_SSE(var_ADD_int32_t_id10_tw2_0_ate_pl,"var_ADD_int32_t_id10_tw2_0_ate_pl") /*var_DEBUG_int32_t_id15_tw2_0_atDEBUG5_DEBUG_int32_t*/;
          cycles += 2;
        }
        for (;j < batch_size;++j)
        {
          int32_t var_LD_int32_t_id2_tw1_0_ata_in = (int32_t)(data_in_2[pos_in_2+j+0]);
          DEBUG_STD(var_LD_int32_t_id2_tw1_0_ata_in,"var_LD_int32_t_id2_tw1_0_ata_in") /*var_DEBUG_int32_t_id3_tw1_0_atDEBUG1_DEBUG_int32_t*/;
          int32_t var_LD_int32_t_id4_tw1_0_atd_in = (int32_t)(data_in_1[pos_in_1+j+0]);
          int32_t var_ADD_int32_t_id5_tw1_0_atb_pl = (int32_t)(var_LD_int32_t_id2_tw1_0_ata_in + var_LD_int32_t_id4_tw1_0_atd_in);
          int32_t var_MUL_int32_t_id6_tw1_0_atb_ml = (int32_t)(var_LD_int32_t_id4_tw1_0_atd_in * var_LD_int32_t_id2_tw1_0_ata_in);
          DEBUG_STD(var_LD_int32_t_id4_tw1_0_atd_in,"var_LD_int32_t_id4_tw1_0_atd_in") /*var_DEBUG_int32_t_id7_tw1_0_atDEBUG4_DEBUG_int32_t*/;
          int32_t var_ADD_int32_t_id8_tw1_0_ate_pl = (int32_t)(var_MUL_int32_t_id6_tw1_0_atb_ml + var_ADD_int32_t_id5_tw1_0_atb_pl);
          DEBUG_STD(var_ADD_int32_t_id5_tw1_0_atb_pl,"var_ADD_int32_t_id5_tw1_0_atb_pl") /*var_DEBUG_int32_t_id9_tw1_0_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_STD(var_MUL_int32_t_id6_tw1_0_atb_ml,"var_MUL_int32_t_id6_tw1_0_atb_ml") /*var_DEBUG_int32_t_id10_tw1_0_atDEBUG2_DEBUG_int32_t*/;
          data_out_0[pos_out_0+j+0] = var_ADD_int32_t_id8_tw1_0_ate_pl /*var_ST_int32_t_id11_tw1_0_atc_out*/;
          DEBUG_STD(var_ADD_int32_t_id8_tw1_0_ate_pl,"var_ADD_int32_t_id8_tw1_0_ate_pl") /*var_DEBUG_int32_t_id12_tw1_0_atDEBUG5_DEBUG_int32_t*/;
          cycles++;
        }
        pos_in_2 += batch_size;
        pos_in_1 += batch_size;
        pos_out_0 += batch_size;
      }
    }
  }
  void test_body4()
  {
    std::cout << "processing width " << 4 << std::endl;
    reset();
    int cycles = 0;
    const int packsize = 4;
    for(int r = 0;r < 1;++r)
    {
      /*decl for input 2 with name a_in*/;
      unsigned pos_in_2 = 0;
      unsigned size_in_2 = 0;
      int32_t * data_in_2 = NULL;
      /*decl for input 1 with name d_in*/;
      unsigned pos_in_1 = 0;
      unsigned size_in_1 = 0;
      int32_t * data_in_1 = NULL;
      /*decl for output 0 with name c_out*/;
      unsigned pos_out_0 = 0;
      unsigned size_out_0 = 0;
      unsigned processed_out_0 = 0;
      int32_t * data_out_0 = NULL;
      for (int i = 0;i < 10 ;i++)
      {
        /*std::cout << "at cycle " << cycles << std::endl*/;
        if ( pos_in_2 == size_in_2 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_2 = (pull()%20+10)*32;
          }
          else
          {
            size_in_2 = 50+pull()%50;
          }
          if(data_in_2 != NULL)
          {
            delete [] data_in_2;
          }
          data_in_2 = new int32_t[size_in_2];
          pos_in_2 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_2;++i)data_in_2[i] = (pull() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_2;++i) /*this will serve for both vector and bool*/data_in_2[i] = pull();
          }
          else
          {
            for(int i = 0;i < size_in_2;++i)
            {
              data_in_2[i] = (int32_t)pull();
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
            size_in_1 = (pull()%20+10)*32;
          }
          else
          {
            size_in_1 = 50+pull()%50;
          }
          if(data_in_1 != NULL)
          {
            delete [] data_in_1;
          }
          data_in_1 = new int32_t[size_in_1];
          pos_in_1 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_1;++i)data_in_1[i] = (pull() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_1;++i) /*this will serve for both vector and bool*/data_in_1[i] = pull();
          }
          else
          {
            for(int i = 0;i < size_in_1;++i)
            {
              data_in_1[i] = (int32_t)pull();
              if(data_in_1[i] == 0) /*division by zero*/
              {
                data_in_1[i]++;
              }
            }
          }
        }
        if ( pos_out_0 == size_out_0 )
        {
          /*would be nices as a macro*/cycles -= size_out_0;
          if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_out_0;++i)
            {
              if(4 == 1)
              {
                data_check_0.push_back(load_bool(data_out_0,i));
                processed_out_0++;
              }
              else
              {
                if(load_bool(data_out_0,i) != data_check_0[processed_out_0])
                {
                  DEBUG_STD((int)load_bool(data_out_0,i), "c_out OUTPUT_0");
                  DEBUG_STD((int)data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  std::cout << "failed in cycle " << cycles << std::endl;
                  throw (int)cycles;
                }
                ;
                processed_out_0++;
              }
              cycles++;
            }
          }
          else
          {
            for(int i = 0;i < size_out_0;++i)
            {
              if(4 == 1)
              {
                data_check_0.push_back(data_out_0[i]);
                processed_out_0++;
              }
              else
              {
                if(data_out_0[i] != data_check_0[processed_out_0])
                {
                  if(std::string("int32_t") == "ssebool")
                  {
                    DEBUG_STD((int)data_out_0[i], "c_out OUTPUT_0");
                    DEBUG_STD((int)data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  }
                  else
                  {
                    DEBUG_STD(data_out_0[i], "c_out OUTPUT_0");
                    DEBUG_STD(data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  }
                  std::cout << "failed in cycle " << cycles << std::endl;
                  throw (int)cycles;
                }
                ;
                processed_out_0++;
              }
              cycles++;
            }
          }
          if(std::string("int32_t") == "bool")
          {
            size_out_0 = (10+pull()%16)*32;
          }
          else
          {
            size_out_0 = 50+pull()%100;
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
          __m128i var_LD_int32_t_id2_tw4_0_ata_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_2[pos_in_2+j+0]));
          DEBUG_SSE(var_LD_int32_t_id2_tw4_0_ata_in,"var_LD_int32_t_id2_tw4_0_ata_in") /*var_DEBUG_int32_t_id3_tw4_0_atDEBUG1_DEBUG_int32_t*/;
          __m128i var_LD_int32_t_id4_tw4_0_atd_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_1[pos_in_1+j+0]));
          __m128i var_ADD_int32_t_id5_tw4_0_atb_pl = (__m128i)(_mm_add_epi32(var_LD_int32_t_id2_tw4_0_ata_in,var_LD_int32_t_id4_tw4_0_atd_in));
          __m128i var_LD_int32_t_id7_tconv_w2_0_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_0_ata_in,0x50)));
          __m128i var_LD_int32_t_id7_tconv_w2_2_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_0_ata_in,0xFA)));
          __m128i var_LD_int32_t_id8_tconv_w2_0_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_0_atd_in,0x50)));
          __m128i var_LD_int32_t_id8_tconv_w2_2_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_0_atd_in,0xFA)));
          __m128i var_MUL_int32_t_id6_tw2_0_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_0_atd_in,var_LD_int32_t_id7_tconv_w2_0_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_2_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_2_atd_in,var_LD_int32_t_id7_tconv_w2_2_ata_in));
          DEBUG_SSE(var_LD_int32_t_id4_tw4_0_atd_in,"var_LD_int32_t_id4_tw4_0_atd_in") /*var_DEBUG_int32_t_id9_tw4_0_atDEBUG4_DEBUG_int32_t*/;
          __m128i var_MUL_int32_t_id11_tconv_w4_0_atb_ml = (__m128i)(_mm_movelh_ps((__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_0_atb_ml,0x08),(__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_2_atb_ml,0x08)));
          __m128i var_ADD_int32_t_id10_tw4_0_ate_pl = (__m128i)(_mm_add_epi32(var_MUL_int32_t_id11_tconv_w4_0_atb_ml,var_ADD_int32_t_id5_tw4_0_atb_pl));
          DEBUG_SSE(var_ADD_int32_t_id5_tw4_0_atb_pl,"var_ADD_int32_t_id5_tw4_0_atb_pl") /*var_DEBUG_int32_t_id12_tw4_0_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_0_atb_ml,"var_MUL_int32_t_id6_tw2_0_atb_ml") /*var_DEBUG_int32_t_id13_tw2_0_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_2_atb_ml,"var_MUL_int32_t_id6_tw2_2_atb_ml") /*var_DEBUG_int32_t_id13_tw2_2_atDEBUG2_DEBUG_int32_t*/;
          _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j+0],var_ADD_int32_t_id10_tw4_0_ate_pl) /*var_ST_int32_t_id14_tw4_0_atc_out*/;
          DEBUG_SSE(var_ADD_int32_t_id10_tw4_0_ate_pl,"var_ADD_int32_t_id10_tw4_0_ate_pl") /*var_DEBUG_int32_t_id15_tw4_0_atDEBUG5_DEBUG_int32_t*/;
          cycles += 4;
        }
        for (;j < batch_size;++j)
        {
          int32_t var_LD_int32_t_id2_tw1_0_ata_in = (int32_t)(data_in_2[pos_in_2+j+0]);
          DEBUG_STD(var_LD_int32_t_id2_tw1_0_ata_in,"var_LD_int32_t_id2_tw1_0_ata_in") /*var_DEBUG_int32_t_id3_tw1_0_atDEBUG1_DEBUG_int32_t*/;
          int32_t var_LD_int32_t_id4_tw1_0_atd_in = (int32_t)(data_in_1[pos_in_1+j+0]);
          int32_t var_ADD_int32_t_id5_tw1_0_atb_pl = (int32_t)(var_LD_int32_t_id2_tw1_0_ata_in + var_LD_int32_t_id4_tw1_0_atd_in);
          int32_t var_MUL_int32_t_id6_tw1_0_atb_ml = (int32_t)(var_LD_int32_t_id4_tw1_0_atd_in * var_LD_int32_t_id2_tw1_0_ata_in);
          DEBUG_STD(var_LD_int32_t_id4_tw1_0_atd_in,"var_LD_int32_t_id4_tw1_0_atd_in") /*var_DEBUG_int32_t_id7_tw1_0_atDEBUG4_DEBUG_int32_t*/;
          int32_t var_ADD_int32_t_id8_tw1_0_ate_pl = (int32_t)(var_MUL_int32_t_id6_tw1_0_atb_ml + var_ADD_int32_t_id5_tw1_0_atb_pl);
          DEBUG_STD(var_ADD_int32_t_id5_tw1_0_atb_pl,"var_ADD_int32_t_id5_tw1_0_atb_pl") /*var_DEBUG_int32_t_id9_tw1_0_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_STD(var_MUL_int32_t_id6_tw1_0_atb_ml,"var_MUL_int32_t_id6_tw1_0_atb_ml") /*var_DEBUG_int32_t_id10_tw1_0_atDEBUG2_DEBUG_int32_t*/;
          data_out_0[pos_out_0+j+0] = var_ADD_int32_t_id8_tw1_0_ate_pl /*var_ST_int32_t_id11_tw1_0_atc_out*/;
          DEBUG_STD(var_ADD_int32_t_id8_tw1_0_ate_pl,"var_ADD_int32_t_id8_tw1_0_ate_pl") /*var_DEBUG_int32_t_id12_tw1_0_atDEBUG5_DEBUG_int32_t*/;
          cycles++;
        }
        pos_in_2 += batch_size;
        pos_in_1 += batch_size;
        pos_out_0 += batch_size;
      }
    }
  }
  void test_body8()
  {
    std::cout << "processing width " << 8 << std::endl;
    reset();
    int cycles = 0;
    const int packsize = 8;
    for(int r = 0;r < 1;++r)
    {
      /*decl for input 2 with name a_in*/;
      unsigned pos_in_2 = 0;
      unsigned size_in_2 = 0;
      int32_t * data_in_2 = NULL;
      /*decl for input 1 with name d_in*/;
      unsigned pos_in_1 = 0;
      unsigned size_in_1 = 0;
      int32_t * data_in_1 = NULL;
      /*decl for output 0 with name c_out*/;
      unsigned pos_out_0 = 0;
      unsigned size_out_0 = 0;
      unsigned processed_out_0 = 0;
      int32_t * data_out_0 = NULL;
      for (int i = 0;i < 10 ;i++)
      {
        /*std::cout << "at cycle " << cycles << std::endl*/;
        if ( pos_in_2 == size_in_2 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_2 = (pull()%20+10)*32;
          }
          else
          {
            size_in_2 = 50+pull()%50;
          }
          if(data_in_2 != NULL)
          {
            delete [] data_in_2;
          }
          data_in_2 = new int32_t[size_in_2];
          pos_in_2 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_2;++i)data_in_2[i] = (pull() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_2;++i) /*this will serve for both vector and bool*/data_in_2[i] = pull();
          }
          else
          {
            for(int i = 0;i < size_in_2;++i)
            {
              data_in_2[i] = (int32_t)pull();
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
            size_in_1 = (pull()%20+10)*32;
          }
          else
          {
            size_in_1 = 50+pull()%50;
          }
          if(data_in_1 != NULL)
          {
            delete [] data_in_1;
          }
          data_in_1 = new int32_t[size_in_1];
          pos_in_1 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_1;++i)data_in_1[i] = (pull() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_1;++i) /*this will serve for both vector and bool*/data_in_1[i] = pull();
          }
          else
          {
            for(int i = 0;i < size_in_1;++i)
            {
              data_in_1[i] = (int32_t)pull();
              if(data_in_1[i] == 0) /*division by zero*/
              {
                data_in_1[i]++;
              }
            }
          }
        }
        if ( pos_out_0 == size_out_0 )
        {
          /*would be nices as a macro*/cycles -= size_out_0;
          if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_out_0;++i)
            {
              if(8 == 1)
              {
                data_check_0.push_back(load_bool(data_out_0,i));
                processed_out_0++;
              }
              else
              {
                if(load_bool(data_out_0,i) != data_check_0[processed_out_0])
                {
                  DEBUG_STD((int)load_bool(data_out_0,i), "c_out OUTPUT_0");
                  DEBUG_STD((int)data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  std::cout << "failed in cycle " << cycles << std::endl;
                  throw (int)cycles;
                }
                ;
                processed_out_0++;
              }
              cycles++;
            }
          }
          else
          {
            for(int i = 0;i < size_out_0;++i)
            {
              if(8 == 1)
              {
                data_check_0.push_back(data_out_0[i]);
                processed_out_0++;
              }
              else
              {
                if(data_out_0[i] != data_check_0[processed_out_0])
                {
                  if(std::string("int32_t") == "ssebool")
                  {
                    DEBUG_STD((int)data_out_0[i], "c_out OUTPUT_0");
                    DEBUG_STD((int)data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  }
                  else
                  {
                    DEBUG_STD(data_out_0[i], "c_out OUTPUT_0");
                    DEBUG_STD(data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  }
                  std::cout << "failed in cycle " << cycles << std::endl;
                  throw (int)cycles;
                }
                ;
                processed_out_0++;
              }
              cycles++;
            }
          }
          if(std::string("int32_t") == "bool")
          {
            size_out_0 = (10+pull()%16)*32;
          }
          else
          {
            size_out_0 = 50+pull()%100;
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
        for ( ;j + 8 <= batch_size;j += 8)
        {
          __m128i var_LD_int32_t_id2_tw4_0_ata_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_2[pos_in_2+j+0]));
          __m128i var_LD_int32_t_id2_tw4_4_ata_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_2[pos_in_2+j+4]));
          DEBUG_SSE(var_LD_int32_t_id2_tw4_0_ata_in,"var_LD_int32_t_id2_tw4_0_ata_in") /*var_DEBUG_int32_t_id3_tw4_0_atDEBUG1_DEBUG_int32_t*/;
          DEBUG_SSE(var_LD_int32_t_id2_tw4_4_ata_in,"var_LD_int32_t_id2_tw4_4_ata_in") /*var_DEBUG_int32_t_id3_tw4_4_atDEBUG1_DEBUG_int32_t*/;
          __m128i var_LD_int32_t_id4_tw4_0_atd_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_1[pos_in_1+j+0]));
          __m128i var_LD_int32_t_id4_tw4_4_atd_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_1[pos_in_1+j+4]));
          __m128i var_ADD_int32_t_id5_tw4_0_atb_pl = (__m128i)(_mm_add_epi32(var_LD_int32_t_id2_tw4_0_ata_in,var_LD_int32_t_id4_tw4_0_atd_in));
          __m128i var_ADD_int32_t_id5_tw4_4_atb_pl = (__m128i)(_mm_add_epi32(var_LD_int32_t_id2_tw4_4_ata_in,var_LD_int32_t_id4_tw4_4_atd_in));
          __m128i var_LD_int32_t_id7_tconv_w2_0_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_0_ata_in,0x50)));
          __m128i var_LD_int32_t_id7_tconv_w2_2_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_0_ata_in,0xFA)));
          __m128i var_LD_int32_t_id7_tconv_w2_4_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_4_ata_in,0x50)));
          __m128i var_LD_int32_t_id7_tconv_w2_6_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_4_ata_in,0xFA)));
          __m128i var_LD_int32_t_id8_tconv_w2_0_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_0_atd_in,0x50)));
          __m128i var_LD_int32_t_id8_tconv_w2_2_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_0_atd_in,0xFA)));
          __m128i var_LD_int32_t_id8_tconv_w2_4_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_4_atd_in,0x50)));
          __m128i var_LD_int32_t_id8_tconv_w2_6_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_4_atd_in,0xFA)));
          __m128i var_MUL_int32_t_id6_tw2_0_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_0_atd_in,var_LD_int32_t_id7_tconv_w2_0_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_2_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_2_atd_in,var_LD_int32_t_id7_tconv_w2_2_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_4_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_4_atd_in,var_LD_int32_t_id7_tconv_w2_4_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_6_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_6_atd_in,var_LD_int32_t_id7_tconv_w2_6_ata_in));
          DEBUG_SSE(var_LD_int32_t_id4_tw4_0_atd_in,"var_LD_int32_t_id4_tw4_0_atd_in") /*var_DEBUG_int32_t_id9_tw4_0_atDEBUG4_DEBUG_int32_t*/;
          DEBUG_SSE(var_LD_int32_t_id4_tw4_4_atd_in,"var_LD_int32_t_id4_tw4_4_atd_in") /*var_DEBUG_int32_t_id9_tw4_4_atDEBUG4_DEBUG_int32_t*/;
          __m128i var_MUL_int32_t_id11_tconv_w4_0_atb_ml = (__m128i)(_mm_movelh_ps((__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_0_atb_ml,0x08),(__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_2_atb_ml,0x08)));
          __m128i var_MUL_int32_t_id11_tconv_w4_4_atb_ml = (__m128i)(_mm_movelh_ps((__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_4_atb_ml,0x08),(__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_6_atb_ml,0x08)));
          __m128i var_ADD_int32_t_id10_tw4_0_ate_pl = (__m128i)(_mm_add_epi32(var_MUL_int32_t_id11_tconv_w4_0_atb_ml,var_ADD_int32_t_id5_tw4_0_atb_pl));
          __m128i var_ADD_int32_t_id10_tw4_4_ate_pl = (__m128i)(_mm_add_epi32(var_MUL_int32_t_id11_tconv_w4_4_atb_ml,var_ADD_int32_t_id5_tw4_4_atb_pl));
          DEBUG_SSE(var_ADD_int32_t_id5_tw4_0_atb_pl,"var_ADD_int32_t_id5_tw4_0_atb_pl") /*var_DEBUG_int32_t_id12_tw4_0_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_SSE(var_ADD_int32_t_id5_tw4_4_atb_pl,"var_ADD_int32_t_id5_tw4_4_atb_pl") /*var_DEBUG_int32_t_id12_tw4_4_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_0_atb_ml,"var_MUL_int32_t_id6_tw2_0_atb_ml") /*var_DEBUG_int32_t_id13_tw2_0_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_2_atb_ml,"var_MUL_int32_t_id6_tw2_2_atb_ml") /*var_DEBUG_int32_t_id13_tw2_2_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_4_atb_ml,"var_MUL_int32_t_id6_tw2_4_atb_ml") /*var_DEBUG_int32_t_id13_tw2_4_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_6_atb_ml,"var_MUL_int32_t_id6_tw2_6_atb_ml") /*var_DEBUG_int32_t_id13_tw2_6_atDEBUG2_DEBUG_int32_t*/;
          _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j+0],var_ADD_int32_t_id10_tw4_0_ate_pl) /*var_ST_int32_t_id14_tw4_0_atc_out*/;
          _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j+4],var_ADD_int32_t_id10_tw4_4_ate_pl) /*var_ST_int32_t_id14_tw4_4_atc_out*/;
          DEBUG_SSE(var_ADD_int32_t_id10_tw4_0_ate_pl,"var_ADD_int32_t_id10_tw4_0_ate_pl") /*var_DEBUG_int32_t_id15_tw4_0_atDEBUG5_DEBUG_int32_t*/;
          DEBUG_SSE(var_ADD_int32_t_id10_tw4_4_ate_pl,"var_ADD_int32_t_id10_tw4_4_ate_pl") /*var_DEBUG_int32_t_id15_tw4_4_atDEBUG5_DEBUG_int32_t*/;
          cycles += 8;
        }
        for (;j < batch_size;++j)
        {
          int32_t var_LD_int32_t_id2_tw1_0_ata_in = (int32_t)(data_in_2[pos_in_2+j+0]);
          DEBUG_STD(var_LD_int32_t_id2_tw1_0_ata_in,"var_LD_int32_t_id2_tw1_0_ata_in") /*var_DEBUG_int32_t_id3_tw1_0_atDEBUG1_DEBUG_int32_t*/;
          int32_t var_LD_int32_t_id4_tw1_0_atd_in = (int32_t)(data_in_1[pos_in_1+j+0]);
          int32_t var_ADD_int32_t_id5_tw1_0_atb_pl = (int32_t)(var_LD_int32_t_id2_tw1_0_ata_in + var_LD_int32_t_id4_tw1_0_atd_in);
          int32_t var_MUL_int32_t_id6_tw1_0_atb_ml = (int32_t)(var_LD_int32_t_id4_tw1_0_atd_in * var_LD_int32_t_id2_tw1_0_ata_in);
          DEBUG_STD(var_LD_int32_t_id4_tw1_0_atd_in,"var_LD_int32_t_id4_tw1_0_atd_in") /*var_DEBUG_int32_t_id7_tw1_0_atDEBUG4_DEBUG_int32_t*/;
          int32_t var_ADD_int32_t_id8_tw1_0_ate_pl = (int32_t)(var_MUL_int32_t_id6_tw1_0_atb_ml + var_ADD_int32_t_id5_tw1_0_atb_pl);
          DEBUG_STD(var_ADD_int32_t_id5_tw1_0_atb_pl,"var_ADD_int32_t_id5_tw1_0_atb_pl") /*var_DEBUG_int32_t_id9_tw1_0_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_STD(var_MUL_int32_t_id6_tw1_0_atb_ml,"var_MUL_int32_t_id6_tw1_0_atb_ml") /*var_DEBUG_int32_t_id10_tw1_0_atDEBUG2_DEBUG_int32_t*/;
          data_out_0[pos_out_0+j+0] = var_ADD_int32_t_id8_tw1_0_ate_pl /*var_ST_int32_t_id11_tw1_0_atc_out*/;
          DEBUG_STD(var_ADD_int32_t_id8_tw1_0_ate_pl,"var_ADD_int32_t_id8_tw1_0_ate_pl") /*var_DEBUG_int32_t_id12_tw1_0_atDEBUG5_DEBUG_int32_t*/;
          cycles++;
        }
        pos_in_2 += batch_size;
        pos_in_1 += batch_size;
        pos_out_0 += batch_size;
      }
    }
  }
  void test_body16()
  {
    std::cout << "processing width " << 16 << std::endl;
    reset();
    int cycles = 0;
    const int packsize = 16;
    for(int r = 0;r < 1;++r)
    {
      /*decl for input 2 with name a_in*/;
      unsigned pos_in_2 = 0;
      unsigned size_in_2 = 0;
      int32_t * data_in_2 = NULL;
      /*decl for input 1 with name d_in*/;
      unsigned pos_in_1 = 0;
      unsigned size_in_1 = 0;
      int32_t * data_in_1 = NULL;
      /*decl for output 0 with name c_out*/;
      unsigned pos_out_0 = 0;
      unsigned size_out_0 = 0;
      unsigned processed_out_0 = 0;
      int32_t * data_out_0 = NULL;
      for (int i = 0;i < 10 ;i++)
      {
        /*std::cout << "at cycle " << cycles << std::endl*/;
        if ( pos_in_2 == size_in_2 )
        {
          if(std::string("int32_t") == "bool")
          {
            size_in_2 = (pull()%20+10)*32;
          }
          else
          {
            size_in_2 = 50+pull()%50;
          }
          if(data_in_2 != NULL)
          {
            delete [] data_in_2;
          }
          data_in_2 = new int32_t[size_in_2];
          pos_in_2 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_2;++i)data_in_2[i] = (pull() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_2;++i) /*this will serve for both vector and bool*/data_in_2[i] = pull();
          }
          else
          {
            for(int i = 0;i < size_in_2;++i)
            {
              data_in_2[i] = (int32_t)pull();
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
            size_in_1 = (pull()%20+10)*32;
          }
          else
          {
            size_in_1 = 50+pull()%50;
          }
          if(data_in_1 != NULL)
          {
            delete [] data_in_1;
          }
          data_in_1 = new int32_t[size_in_1];
          pos_in_1 = 0;
          if(std::string("int32_t") == "ssebool")
          {
            for(int i = 0;i < size_in_1;++i)data_in_1[i] = (pull() % 2) * 0xFF;
          }
          else if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_in_1;++i) /*this will serve for both vector and bool*/data_in_1[i] = pull();
          }
          else
          {
            for(int i = 0;i < size_in_1;++i)
            {
              data_in_1[i] = (int32_t)pull();
              if(data_in_1[i] == 0) /*division by zero*/
              {
                data_in_1[i]++;
              }
            }
          }
        }
        if ( pos_out_0 == size_out_0 )
        {
          /*would be nices as a macro*/cycles -= size_out_0;
          if(std::string("int32_t") == "bool")
          {
            for(int i = 0;i < size_out_0;++i)
            {
              if(16 == 1)
              {
                data_check_0.push_back(load_bool(data_out_0,i));
                processed_out_0++;
              }
              else
              {
                if(load_bool(data_out_0,i) != data_check_0[processed_out_0])
                {
                  DEBUG_STD((int)load_bool(data_out_0,i), "c_out OUTPUT_0");
                  DEBUG_STD((int)data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  std::cout << "failed in cycle " << cycles << std::endl;
                  throw (int)cycles;
                }
                ;
                processed_out_0++;
              }
              cycles++;
            }
          }
          else
          {
            for(int i = 0;i < size_out_0;++i)
            {
              if(16 == 1)
              {
                data_check_0.push_back(data_out_0[i]);
                processed_out_0++;
              }
              else
              {
                if(data_out_0[i] != data_check_0[processed_out_0])
                {
                  if(std::string("int32_t") == "ssebool")
                  {
                    DEBUG_STD((int)data_out_0[i], "c_out OUTPUT_0");
                    DEBUG_STD((int)data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  }
                  else
                  {
                    DEBUG_STD(data_out_0[i], "c_out OUTPUT_0");
                    DEBUG_STD(data_check_0[processed_out_0], "c_out OUTPUT_CHECK_0");
                  }
                  std::cout << "failed in cycle " << cycles << std::endl;
                  throw (int)cycles;
                }
                ;
                processed_out_0++;
              }
              cycles++;
            }
          }
          if(std::string("int32_t") == "bool")
          {
            size_out_0 = (10+pull()%16)*32;
          }
          else
          {
            size_out_0 = 50+pull()%100;
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
        for ( ;j + 16 <= batch_size;j += 16)
        {
          __m128i var_LD_int32_t_id2_tw4_0_ata_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_2[pos_in_2+j+0]));
          __m128i var_LD_int32_t_id2_tw4_4_ata_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_2[pos_in_2+j+4]));
          __m128i var_LD_int32_t_id2_tw4_8_ata_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_2[pos_in_2+j+8]));
          __m128i var_LD_int32_t_id2_tw4_12_ata_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_2[pos_in_2+j+12]));
          DEBUG_SSE(var_LD_int32_t_id2_tw4_0_ata_in,"var_LD_int32_t_id2_tw4_0_ata_in") /*var_DEBUG_int32_t_id3_tw4_0_atDEBUG1_DEBUG_int32_t*/;
          DEBUG_SSE(var_LD_int32_t_id2_tw4_4_ata_in,"var_LD_int32_t_id2_tw4_4_ata_in") /*var_DEBUG_int32_t_id3_tw4_4_atDEBUG1_DEBUG_int32_t*/;
          DEBUG_SSE(var_LD_int32_t_id2_tw4_8_ata_in,"var_LD_int32_t_id2_tw4_8_ata_in") /*var_DEBUG_int32_t_id3_tw4_8_atDEBUG1_DEBUG_int32_t*/;
          DEBUG_SSE(var_LD_int32_t_id2_tw4_12_ata_in,"var_LD_int32_t_id2_tw4_12_ata_in") /*var_DEBUG_int32_t_id3_tw4_12_atDEBUG1_DEBUG_int32_t*/;
          __m128i var_LD_int32_t_id4_tw4_0_atd_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_1[pos_in_1+j+0]));
          __m128i var_LD_int32_t_id4_tw4_4_atd_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_1[pos_in_1+j+4]));
          __m128i var_LD_int32_t_id4_tw4_8_atd_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_1[pos_in_1+j+8]));
          __m128i var_LD_int32_t_id4_tw4_12_atd_in = (__m128i)(_mm_loadu_si128((__m128i*)&data_in_1[pos_in_1+j+12]));
          __m128i var_ADD_int32_t_id5_tw4_0_atb_pl = (__m128i)(_mm_add_epi32(var_LD_int32_t_id2_tw4_0_ata_in,var_LD_int32_t_id4_tw4_0_atd_in));
          __m128i var_ADD_int32_t_id5_tw4_4_atb_pl = (__m128i)(_mm_add_epi32(var_LD_int32_t_id2_tw4_4_ata_in,var_LD_int32_t_id4_tw4_4_atd_in));
          __m128i var_ADD_int32_t_id5_tw4_8_atb_pl = (__m128i)(_mm_add_epi32(var_LD_int32_t_id2_tw4_8_ata_in,var_LD_int32_t_id4_tw4_8_atd_in));
          __m128i var_ADD_int32_t_id5_tw4_12_atb_pl = (__m128i)(_mm_add_epi32(var_LD_int32_t_id2_tw4_12_ata_in,var_LD_int32_t_id4_tw4_12_atd_in));
          __m128i var_LD_int32_t_id7_tconv_w2_0_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_0_ata_in,0x50)));
          __m128i var_LD_int32_t_id7_tconv_w2_2_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_0_ata_in,0xFA)));
          __m128i var_LD_int32_t_id7_tconv_w2_4_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_4_ata_in,0x50)));
          __m128i var_LD_int32_t_id7_tconv_w2_6_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_4_ata_in,0xFA)));
          __m128i var_LD_int32_t_id7_tconv_w2_8_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_8_ata_in,0x50)));
          __m128i var_LD_int32_t_id7_tconv_w2_10_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_8_ata_in,0xFA)));
          __m128i var_LD_int32_t_id7_tconv_w2_12_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_12_ata_in,0x50)));
          __m128i var_LD_int32_t_id7_tconv_w2_14_ata_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id2_tw4_12_ata_in,0xFA)));
          __m128i var_LD_int32_t_id8_tconv_w2_0_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_0_atd_in,0x50)));
          __m128i var_LD_int32_t_id8_tconv_w2_2_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_0_atd_in,0xFA)));
          __m128i var_LD_int32_t_id8_tconv_w2_4_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_4_atd_in,0x50)));
          __m128i var_LD_int32_t_id8_tconv_w2_6_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_4_atd_in,0xFA)));
          __m128i var_LD_int32_t_id8_tconv_w2_8_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_8_atd_in,0x50)));
          __m128i var_LD_int32_t_id8_tconv_w2_10_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_8_atd_in,0xFA)));
          __m128i var_LD_int32_t_id8_tconv_w2_12_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_12_atd_in,0x50)));
          __m128i var_LD_int32_t_id8_tconv_w2_14_atd_in = (__m128i)(_mm_and_si128(_mm_set_epi32(0,0xFFFFFFFF,0,0xFFFFFFFF),_mm_shuffle_epi32(var_LD_int32_t_id4_tw4_12_atd_in,0xFA)));
          __m128i var_MUL_int32_t_id6_tw2_0_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_0_atd_in,var_LD_int32_t_id7_tconv_w2_0_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_2_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_2_atd_in,var_LD_int32_t_id7_tconv_w2_2_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_4_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_4_atd_in,var_LD_int32_t_id7_tconv_w2_4_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_6_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_6_atd_in,var_LD_int32_t_id7_tconv_w2_6_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_8_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_8_atd_in,var_LD_int32_t_id7_tconv_w2_8_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_10_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_10_atd_in,var_LD_int32_t_id7_tconv_w2_10_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_12_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_12_atd_in,var_LD_int32_t_id7_tconv_w2_12_ata_in));
          __m128i var_MUL_int32_t_id6_tw2_14_atb_ml = (__m128i)(_mm_mul_epi32(var_LD_int32_t_id8_tconv_w2_14_atd_in,var_LD_int32_t_id7_tconv_w2_14_ata_in));
          DEBUG_SSE(var_LD_int32_t_id4_tw4_0_atd_in,"var_LD_int32_t_id4_tw4_0_atd_in") /*var_DEBUG_int32_t_id9_tw4_0_atDEBUG4_DEBUG_int32_t*/;
          DEBUG_SSE(var_LD_int32_t_id4_tw4_4_atd_in,"var_LD_int32_t_id4_tw4_4_atd_in") /*var_DEBUG_int32_t_id9_tw4_4_atDEBUG4_DEBUG_int32_t*/;
          DEBUG_SSE(var_LD_int32_t_id4_tw4_8_atd_in,"var_LD_int32_t_id4_tw4_8_atd_in") /*var_DEBUG_int32_t_id9_tw4_8_atDEBUG4_DEBUG_int32_t*/;
          DEBUG_SSE(var_LD_int32_t_id4_tw4_12_atd_in,"var_LD_int32_t_id4_tw4_12_atd_in") /*var_DEBUG_int32_t_id9_tw4_12_atDEBUG4_DEBUG_int32_t*/;
          __m128i var_MUL_int32_t_id11_tconv_w4_0_atb_ml = (__m128i)(_mm_movelh_ps((__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_0_atb_ml,0x08),(__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_2_atb_ml,0x08)));
          __m128i var_MUL_int32_t_id11_tconv_w4_4_atb_ml = (__m128i)(_mm_movelh_ps((__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_4_atb_ml,0x08),(__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_6_atb_ml,0x08)));
          __m128i var_MUL_int32_t_id11_tconv_w4_8_atb_ml = (__m128i)(_mm_movelh_ps((__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_8_atb_ml,0x08),(__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_10_atb_ml,0x08)));
          __m128i var_MUL_int32_t_id11_tconv_w4_12_atb_ml = (__m128i)(_mm_movelh_ps((__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_12_atb_ml,0x08),(__m128) _mm_shuffle_epi32(var_MUL_int32_t_id6_tw2_14_atb_ml,0x08)));
          __m128i var_ADD_int32_t_id10_tw4_0_ate_pl = (__m128i)(_mm_add_epi32(var_MUL_int32_t_id11_tconv_w4_0_atb_ml,var_ADD_int32_t_id5_tw4_0_atb_pl));
          __m128i var_ADD_int32_t_id10_tw4_4_ate_pl = (__m128i)(_mm_add_epi32(var_MUL_int32_t_id11_tconv_w4_4_atb_ml,var_ADD_int32_t_id5_tw4_4_atb_pl));
          __m128i var_ADD_int32_t_id10_tw4_8_ate_pl = (__m128i)(_mm_add_epi32(var_MUL_int32_t_id11_tconv_w4_8_atb_ml,var_ADD_int32_t_id5_tw4_8_atb_pl));
          __m128i var_ADD_int32_t_id10_tw4_12_ate_pl = (__m128i)(_mm_add_epi32(var_MUL_int32_t_id11_tconv_w4_12_atb_ml,var_ADD_int32_t_id5_tw4_12_atb_pl));
          DEBUG_SSE(var_ADD_int32_t_id5_tw4_0_atb_pl,"var_ADD_int32_t_id5_tw4_0_atb_pl") /*var_DEBUG_int32_t_id12_tw4_0_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_SSE(var_ADD_int32_t_id5_tw4_4_atb_pl,"var_ADD_int32_t_id5_tw4_4_atb_pl") /*var_DEBUG_int32_t_id12_tw4_4_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_SSE(var_ADD_int32_t_id5_tw4_8_atb_pl,"var_ADD_int32_t_id5_tw4_8_atb_pl") /*var_DEBUG_int32_t_id12_tw4_8_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_SSE(var_ADD_int32_t_id5_tw4_12_atb_pl,"var_ADD_int32_t_id5_tw4_12_atb_pl") /*var_DEBUG_int32_t_id12_tw4_12_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_0_atb_ml,"var_MUL_int32_t_id6_tw2_0_atb_ml") /*var_DEBUG_int32_t_id13_tw2_0_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_2_atb_ml,"var_MUL_int32_t_id6_tw2_2_atb_ml") /*var_DEBUG_int32_t_id13_tw2_2_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_4_atb_ml,"var_MUL_int32_t_id6_tw2_4_atb_ml") /*var_DEBUG_int32_t_id13_tw2_4_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_6_atb_ml,"var_MUL_int32_t_id6_tw2_6_atb_ml") /*var_DEBUG_int32_t_id13_tw2_6_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_8_atb_ml,"var_MUL_int32_t_id6_tw2_8_atb_ml") /*var_DEBUG_int32_t_id13_tw2_8_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_10_atb_ml,"var_MUL_int32_t_id6_tw2_10_atb_ml") /*var_DEBUG_int32_t_id13_tw2_10_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_12_atb_ml,"var_MUL_int32_t_id6_tw2_12_atb_ml") /*var_DEBUG_int32_t_id13_tw2_12_atDEBUG2_DEBUG_int32_t*/;
          DEBUG_SSE(var_MUL_int32_t_id6_tw2_14_atb_ml,"var_MUL_int32_t_id6_tw2_14_atb_ml") /*var_DEBUG_int32_t_id13_tw2_14_atDEBUG2_DEBUG_int32_t*/;
          _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j+0],var_ADD_int32_t_id10_tw4_0_ate_pl) /*var_ST_int32_t_id14_tw4_0_atc_out*/;
          _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j+4],var_ADD_int32_t_id10_tw4_4_ate_pl) /*var_ST_int32_t_id14_tw4_4_atc_out*/;
          _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j+8],var_ADD_int32_t_id10_tw4_8_ate_pl) /*var_ST_int32_t_id14_tw4_8_atc_out*/;
          _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j+12],var_ADD_int32_t_id10_tw4_12_ate_pl) /*var_ST_int32_t_id14_tw4_12_atc_out*/;
          DEBUG_SSE(var_ADD_int32_t_id10_tw4_0_ate_pl,"var_ADD_int32_t_id10_tw4_0_ate_pl") /*var_DEBUG_int32_t_id15_tw4_0_atDEBUG5_DEBUG_int32_t*/;
          DEBUG_SSE(var_ADD_int32_t_id10_tw4_4_ate_pl,"var_ADD_int32_t_id10_tw4_4_ate_pl") /*var_DEBUG_int32_t_id15_tw4_4_atDEBUG5_DEBUG_int32_t*/;
          DEBUG_SSE(var_ADD_int32_t_id10_tw4_8_ate_pl,"var_ADD_int32_t_id10_tw4_8_ate_pl") /*var_DEBUG_int32_t_id15_tw4_8_atDEBUG5_DEBUG_int32_t*/;
          DEBUG_SSE(var_ADD_int32_t_id10_tw4_12_ate_pl,"var_ADD_int32_t_id10_tw4_12_ate_pl") /*var_DEBUG_int32_t_id15_tw4_12_atDEBUG5_DEBUG_int32_t*/;
          cycles += 16;
        }
        for (;j < batch_size;++j)
        {
          int32_t var_LD_int32_t_id2_tw1_0_ata_in = (int32_t)(data_in_2[pos_in_2+j+0]);
          DEBUG_STD(var_LD_int32_t_id2_tw1_0_ata_in,"var_LD_int32_t_id2_tw1_0_ata_in") /*var_DEBUG_int32_t_id3_tw1_0_atDEBUG1_DEBUG_int32_t*/;
          int32_t var_LD_int32_t_id4_tw1_0_atd_in = (int32_t)(data_in_1[pos_in_1+j+0]);
          int32_t var_ADD_int32_t_id5_tw1_0_atb_pl = (int32_t)(var_LD_int32_t_id2_tw1_0_ata_in + var_LD_int32_t_id4_tw1_0_atd_in);
          int32_t var_MUL_int32_t_id6_tw1_0_atb_ml = (int32_t)(var_LD_int32_t_id4_tw1_0_atd_in * var_LD_int32_t_id2_tw1_0_ata_in);
          DEBUG_STD(var_LD_int32_t_id4_tw1_0_atd_in,"var_LD_int32_t_id4_tw1_0_atd_in") /*var_DEBUG_int32_t_id7_tw1_0_atDEBUG4_DEBUG_int32_t*/;
          int32_t var_ADD_int32_t_id8_tw1_0_ate_pl = (int32_t)(var_MUL_int32_t_id6_tw1_0_atb_ml + var_ADD_int32_t_id5_tw1_0_atb_pl);
          DEBUG_STD(var_ADD_int32_t_id5_tw1_0_atb_pl,"var_ADD_int32_t_id5_tw1_0_atb_pl") /*var_DEBUG_int32_t_id9_tw1_0_atDEBUG3_DEBUG_int32_t*/;
          DEBUG_STD(var_MUL_int32_t_id6_tw1_0_atb_ml,"var_MUL_int32_t_id6_tw1_0_atb_ml") /*var_DEBUG_int32_t_id10_tw1_0_atDEBUG2_DEBUG_int32_t*/;
          data_out_0[pos_out_0+j+0] = var_ADD_int32_t_id8_tw1_0_ate_pl /*var_ST_int32_t_id11_tw1_0_atc_out*/;
          DEBUG_STD(var_ADD_int32_t_id8_tw1_0_ate_pl,"var_ADD_int32_t_id8_tw1_0_ate_pl") /*var_DEBUG_int32_t_id12_tw1_0_atDEBUG5_DEBUG_int32_t*/;
          cycles++;
        }
        pos_in_2 += batch_size;
        pos_in_1 += batch_size;
        pos_out_0 += batch_size;
      }
    }
  }
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
    test_body1();
    test_body2();
    test_body4();
    test_body8();
    test_body16();
  }
}
;
