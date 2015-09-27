#include <fstream>
#include <utility>
#include "bobox_manager.hpp"
#include "bobox_results.hpp"
#include "bobox_bobolang.hpp"
#include "bobox_basic_object_factory.hpp"
class test_bobox_box :
public bobox::basic_box
{
public:
  typedef generic_model<test_bobox_box> model;
test_bobox_box(const bobox::box_parameters_pack &box_params) :
  bobox::basic_box(box_params)
  {
  }
  BOBOX_BOX_INPUTS_LIST(input_list_2, 2,input_list_1, 1);
  BOBOX_BOX_OUTPUTS_LIST(output_list_0, 0);
private:
  virtual void sync_body() BOBOX_OVERRIDE
  {
    /*decl in 2*/;
    static const bobox::column_index_type col_in_2( 2);
    unsigned pos_in_2 = 0;
    unsigned size_in_2 = 0;
    bobox::envelope_ptr_type env_in_2;
    const int* data_in_2;
    /*decl in 1*/;
    static const bobox::column_index_type col_in_1( 1);
    unsigned pos_in_1 = 0;
    unsigned size_in_1 = 0;
    bobox::envelope_ptr_type env_in_1;
    const int* data_in_1;
    /*decl out 0*/;
    static const bobox::column_index_type col_out_0( 0);
    unsigned pos_out_0 = 0;
    unsigned size_out_0 = 0;
    bobox::mutable_envelope_ptr_type env_out_0;
    int * data_out_0;
    for (;;)
    {
      /*envelope in 2*/;
      if ( pos_in_2 == size_in_2 )
      {
        env_in_2.reset();
        env_in_2 = pop_envelope( inputs::input_list_2());
        if ( env_in_2->is_poisoned() )break;
        size_in_2 = env_in_2->get_size();
        pos_in_2 = 0;
        data_in_2 = env_in_2->get_column( col_in_2).get_data< int>();
        continue;
        /* to skip empty envelopes*/;
      }
      /*envelope in 1*/;
      if ( pos_in_1 == size_in_1 )
      {
        env_in_1.reset();
        env_in_1 = pop_envelope( inputs::input_list_1());
        if ( env_in_1->is_poisoned() )break;
        size_in_1 = env_in_1->get_size();
        pos_in_1 = 0;
        data_in_1 = env_in_1->get_column( col_in_1).get_data< int>();
        continue;
        /* to skip empty envelopes*/;
      }
      /*envelope out 0*/;
      if ( pos_out_0 == size_out_0 )
      {
        if ( pos_out_0 )
        {
          env_out_0->set_size( pos_out_0);
          send_envelope( outputs::output_list_0(), env_out_0);
          wait();
        }
        env_out_0 = allocate( outputs::output_list_0());
        size_out_0 = env_out_0->get_capacity();
        pos_out_0 = 0;
        data_out_0 = env_out_0->get_column( col_out_0).get_data< int>();
      }
      auto batch_size = std::min(std::min(std::min(std::numeric_limits<unsigned>::max(), size_in_2 - pos_in_2), size_in_1 - pos_in_1), size_out_0 - pos_out_0);
      /*declare align*/;
      std::size_t align_offset = pos_in_2 % 4;
      std::size_t output_offset = pos_out_2 % 4;
      bool aligned = true;
      ;
      /*check alignment*/;
      aligned &= align_offset == pos_in_2 % 4;
      aligned &= align_offset == pos_in_1 % 4;
      aligned &= output_offset == pos_out_0 % 4;
      /*actual code*/;
      if(aligned)
      {
        for ( std::size_t j = 0;j < output_offset && j < batch_size;++ j)
        {
          int var_LDI_id2_tw1_0_ata_in = (int)(data_in_2[pos_in_2+j]);
          int var_LDI_id3_tw1_0_atd_in = (int)(data_in_1[pos_in_1+j]);
          int var_ADDI_id4_tw1_0_atb_pl = (int)(var_LDI_id2_tw1_0_ata_in + var_LDI_id3_tw1_0_atd_in);
          int var_MULI_id5_tw1_0_atb_ml = (int)(var_LDI_id3_tw1_0_atd_in * var_LDI_id2_tw1_0_ata_in);
          int var_ADDI_id6_tw1_0_ate_pl = (int)(var_MULI_id5_tw1_0_atb_ml + var_ADDI_id4_tw1_0_atb_pl);
          data_out_0[pos_out_0+j] = var_ADDI_id6_tw1_0_ate_pl /*var_STI_id7_tw1_0_atc_out*/;
        }
        switch(align_offset + output_offset)
        {
        case 0:
          {
            __m128i var_LDI_id2_tw4_0_ata_in = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_2[pos_in_2+j]));
            __m128i var_LDI_id3_tw4_0_atd_in = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_1[pos_in_1+j]));
            __m128i var_ADDI_id4_tw4_0_atb_pl = (__m128i)(_mm_add_epi32(var_LDI_id2_tw4_0_ata_in, var_LDI_id3_tw4_0_atd_in));
            __m128i var_LDI_id6_tconv_w2_0_ata_in = (__m128i)(var_LDI_id2_tw4_0_ata_in);
            __m128i var_LDI_id6_tconv_w2_2_ata_in = (__m128i)(_mm_srli_si128(var_LDI_id2_tw4_0_ata_in,4));
            __m128i var_LDI_id7_tconv_w2_0_atd_in = (__m128i)(var_LDI_id3_tw4_0_atd_in);
            __m128i var_LDI_id7_tconv_w2_2_atd_in = (__m128i)(_mm_srli_si128(var_LDI_id3_tw4_0_atd_in,4));
            __m128i var_MULI_id5_tw2_0_atb_ml = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_id7_tconv_w2_0_atd_in, var_LDI_id6_tconv_w2_0_ata_in), _MM_SHUFFLE (0,0,2,0)));
            __m128i var_MULI_id5_tw2_2_atb_ml = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_id7_tconv_w2_2_atd_in, var_LDI_id6_tconv_w2_2_ata_in), _MM_SHUFFLE (0,0,2,0)));
            __m128i var_MULI_id9_tconv_w4_0_atb_ml = (__m128i)(_mm_unpacklo_epi32(var_MULI_id5_tw2_0_atb_ml,var_MULI_id5_tw2_2_atb_ml));
            __m128i var_ADDI_id8_tw4_0_ate_pl = (__m128i)(_mm_add_epi32(var_MULI_id9_tconv_w4_0_atb_ml, var_ADDI_id4_tw4_0_atb_pl));
            _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j], var_ADDI_id8_tw4_0_ate_pl) /*var_STI_id10_tw4_0_atc_out*/;
          }
          ;
          break;
        case 1:
          {
            __m128i var_LDI_id2_tw4_0_ata_in = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_2[pos_in_2+j]));
            __m128i var_LDI_id3_tw4_0_atd_in = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_1[pos_in_1+j]));
            __m128i var_ADDI_id4_tw4_0_atb_pl = (__m128i)(_mm_add_epi32(var_LDI_id2_tw4_0_ata_in, var_LDI_id3_tw4_0_atd_in));
            __m128i var_LDI_id6_tconv_w2_0_ata_in = (__m128i)(var_LDI_id2_tw4_0_ata_in);
            __m128i var_LDI_id6_tconv_w2_2_ata_in = (__m128i)(_mm_srli_si128(var_LDI_id2_tw4_0_ata_in,4));
            __m128i var_LDI_id7_tconv_w2_0_atd_in = (__m128i)(var_LDI_id3_tw4_0_atd_in);
            __m128i var_LDI_id7_tconv_w2_2_atd_in = (__m128i)(_mm_srli_si128(var_LDI_id3_tw4_0_atd_in,4));
            __m128i var_MULI_id5_tw2_0_atb_ml = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_id7_tconv_w2_0_atd_in, var_LDI_id6_tconv_w2_0_ata_in), _MM_SHUFFLE (0,0,2,0)));
            __m128i var_MULI_id5_tw2_2_atb_ml = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_id7_tconv_w2_2_atd_in, var_LDI_id6_tconv_w2_2_ata_in), _MM_SHUFFLE (0,0,2,0)));
            __m128i var_MULI_id9_tconv_w4_0_atb_ml = (__m128i)(_mm_unpacklo_epi32(var_MULI_id5_tw2_0_atb_ml,var_MULI_id5_tw2_2_atb_ml));
            __m128i var_ADDI_id8_tw4_0_ate_pl = (__m128i)(_mm_add_epi32(var_MULI_id9_tconv_w4_0_atb_ml, var_ADDI_id4_tw4_0_atb_pl));
            _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j], var_ADDI_id8_tw4_0_ate_pl) /*var_STI_id10_tw4_0_atc_out*/;
          }
          break;
        )case 2:
          {
            __m128i var_LDI_id2_tw4_0_ata_in = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_2[pos_in_2+j]));
            __m128i var_LDI_id3_tw4_0_atd_in = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_1[pos_in_1+j]));
            __m128i var_ADDI_id4_tw4_0_atb_pl = (__m128i)(_mm_add_epi32(var_LDI_id2_tw4_0_ata_in, var_LDI_id3_tw4_0_atd_in));
            __m128i var_LDI_id6_tconv_w2_0_ata_in = (__m128i)(var_LDI_id2_tw4_0_ata_in);
            __m128i var_LDI_id6_tconv_w2_2_ata_in = (__m128i)(_mm_srli_si128(var_LDI_id2_tw4_0_ata_in,4));
            __m128i var_LDI_id7_tconv_w2_0_atd_in = (__m128i)(var_LDI_id3_tw4_0_atd_in);
            __m128i var_LDI_id7_tconv_w2_2_atd_in = (__m128i)(_mm_srli_si128(var_LDI_id3_tw4_0_atd_in,4));
            __m128i var_MULI_id5_tw2_0_atb_ml = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_id7_tconv_w2_0_atd_in, var_LDI_id6_tconv_w2_0_ata_in), _MM_SHUFFLE (0,0,2,0)));
            __m128i var_MULI_id5_tw2_2_atb_ml = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_id7_tconv_w2_2_atd_in, var_LDI_id6_tconv_w2_2_ata_in), _MM_SHUFFLE (0,0,2,0)));
            __m128i var_MULI_id9_tconv_w4_0_atb_ml = (__m128i)(_mm_unpacklo_epi32(var_MULI_id5_tw2_0_atb_ml,var_MULI_id5_tw2_2_atb_ml));
            __m128i var_ADDI_id8_tw4_0_ate_pl = (__m128i)(_mm_add_epi32(var_MULI_id9_tconv_w4_0_atb_ml, var_ADDI_id4_tw4_0_atb_pl));
            _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j], var_ADDI_id8_tw4_0_ate_pl) /*var_STI_id10_tw4_0_atc_out*/;
          }
          break;
        )case 3:
          {
            __m128i var_LDI_id2_tw4_0_ata_in = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_2[pos_in_2+j]));
            __m128i var_LDI_id3_tw4_0_atd_in = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_1[pos_in_1+j]));
            __m128i var_ADDI_id4_tw4_0_atb_pl = (__m128i)(_mm_add_epi32(var_LDI_id2_tw4_0_ata_in, var_LDI_id3_tw4_0_atd_in));
            __m128i var_LDI_id6_tconv_w2_0_ata_in = (__m128i)(var_LDI_id2_tw4_0_ata_in);
            __m128i var_LDI_id6_tconv_w2_2_ata_in = (__m128i)(_mm_srli_si128(var_LDI_id2_tw4_0_ata_in,4));
            __m128i var_LDI_id7_tconv_w2_0_atd_in = (__m128i)(var_LDI_id3_tw4_0_atd_in);
            __m128i var_LDI_id7_tconv_w2_2_atd_in = (__m128i)(_mm_srli_si128(var_LDI_id3_tw4_0_atd_in,4));
            __m128i var_MULI_id5_tw2_0_atb_ml = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_id7_tconv_w2_0_atd_in, var_LDI_id6_tconv_w2_0_ata_in), _MM_SHUFFLE (0,0,2,0)));
            __m128i var_MULI_id5_tw2_2_atb_ml = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_id7_tconv_w2_2_atd_in, var_LDI_id6_tconv_w2_2_ata_in), _MM_SHUFFLE (0,0,2,0)));
            __m128i var_MULI_id9_tconv_w4_0_atb_ml = (__m128i)(_mm_unpacklo_epi32(var_MULI_id5_tw2_0_atb_ml,var_MULI_id5_tw2_2_atb_ml));
            __m128i var_ADDI_id8_tw4_0_ate_pl = (__m128i)(_mm_add_epi32(var_MULI_id9_tconv_w4_0_atb_ml, var_ADDI_id4_tw4_0_atb_pl));
            _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j], var_ADDI_id8_tw4_0_ate_pl) /*var_STI_id10_tw4_0_atc_out*/;
          }
          break;
          )
        }
      }
      else
      {
        __m128i var_LDI_id2_tw4_0_ata_in = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_2[pos_in_2+j]));
        __m128i var_LDI_id3_tw4_0_atd_in = (__m128i)(_mm_loadu_si128((const __m128i*)&data_in_1[pos_in_1+j]));
        __m128i var_ADDI_id4_tw4_0_atb_pl = (__m128i)(_mm_add_epi32(var_LDI_id2_tw4_0_ata_in, var_LDI_id3_tw4_0_atd_in));
        __m128i var_LDI_id6_tconv_w2_0_ata_in = (__m128i)(var_LDI_id2_tw4_0_ata_in);
        __m128i var_LDI_id6_tconv_w2_2_ata_in = (__m128i)(_mm_srli_si128(var_LDI_id2_tw4_0_ata_in,4));
        __m128i var_LDI_id7_tconv_w2_0_atd_in = (__m128i)(var_LDI_id3_tw4_0_atd_in);
        __m128i var_LDI_id7_tconv_w2_2_atd_in = (__m128i)(_mm_srli_si128(var_LDI_id3_tw4_0_atd_in,4));
        __m128i var_MULI_id5_tw2_0_atb_ml = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_id7_tconv_w2_0_atd_in, var_LDI_id6_tconv_w2_0_ata_in), _MM_SHUFFLE (0,0,2,0)));
        __m128i var_MULI_id5_tw2_2_atb_ml = (__m128i)(_mm_shuffle_epi32(_mm_mul_epu32(var_LDI_id7_tconv_w2_2_atd_in, var_LDI_id6_tconv_w2_2_ata_in), _MM_SHUFFLE (0,0,2,0)));
        __m128i var_MULI_id9_tconv_w4_0_atb_ml = (__m128i)(_mm_unpacklo_epi32(var_MULI_id5_tw2_0_atb_ml,var_MULI_id5_tw2_2_atb_ml));
        __m128i var_ADDI_id8_tw4_0_ate_pl = (__m128i)(_mm_add_epi32(var_MULI_id9_tconv_w4_0_atb_ml, var_ADDI_id4_tw4_0_atb_pl));
        _mm_storeu_si128((__m128i*)&data_out_0[pos_out_0+j], var_ADDI_id8_tw4_0_ate_pl) /*var_STI_id10_tw4_0_atc_out*/;
      }
      for ( std::size_t j = 0;j < batch_size;++ j)
      {
        int var_LDI_id2_tw1_0_ata_in = (int)(data_in_2[pos_in_2+j]);
        int var_LDI_id3_tw1_0_atd_in = (int)(data_in_1[pos_in_1+j]);
        int var_ADDI_id4_tw1_0_atb_pl = (int)(var_LDI_id2_tw1_0_ata_in + var_LDI_id3_tw1_0_atd_in);
        int var_MULI_id5_tw1_0_atb_ml = (int)(var_LDI_id3_tw1_0_atd_in * var_LDI_id2_tw1_0_ata_in);
        int var_ADDI_id6_tw1_0_ate_pl = (int)(var_MULI_id5_tw1_0_atb_ml + var_ADDI_id4_tw1_0_atb_pl);
        data_out_0[pos_out_0+j] = var_ADDI_id6_tw1_0_ate_pl /*var_STI_id7_tw1_0_atc_out*/;
      }
      pos_in_2 += batch_size;
      pos_in_1 += batch_size;
      pos_out_0 += batch_size;
    }
    /*send envelope*/;
    if ( pos_out_0 )
    {
      env_out_0->set_size( pos_out_0);
      send_envelope( outputs::output_list_0(), env_out_0);
    }
    send_poisoned( outputs::output_list_0());
  }
}
;
