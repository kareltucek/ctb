#include <fstream>
#include <utility>
#include "bobox_manager.hpp"
#include "bobox_results.hpp"
#include "bobox_bobolang.hpp"
#include "bobox_basic_object_factory.hpp"
class output_box :
public bobox::basic_box
{
public:
  typedef generic_model<output_box> model;
output_box(const bobox::box_parameters_pack &box_params) :
  bobox::basic_box(box_params)
  {
  }
  BOBOX_BOX_INPUTS_LIST(input_list_0, 0,input_list_1, 1);
  BOBOX_BOX_OUTPUTS_LIST(output_list_0, 0);
private:
  virtual void sync_body() BOBOX_OVERRIDE
  {
    /*decl in 0*/;
    static const bobox::column_index_type col_in_0( 0);
    unsigned pos_in_0 = 0;
    unsigned size_in_0 = 0;
    bobox::envelope_ptr_type env_in_0;
    const int32_t* data_in_0;
    /*decl in 1*/;
    static const bobox::column_index_type col_in_1( 1);
    unsigned pos_in_1 = 0;
    unsigned size_in_1 = 0;
    bobox::envelope_ptr_type env_in_1;
    const int32_t* data_in_1;
    /*decl out 0*/;
    static const bobox::column_index_type col_out_0( 0);
    unsigned pos_out_0 = 0;
    unsigned size_out_0 = 0;
    bobox::mutable_envelope_ptr_type env_out_0;
    int32_t * data_out_0;
    for (;;)
    {
      /*envelope in 0*/;
      if ( pos_in_0 == size_in_0 )
      {
        env_in_0.reset();
        env_in_0 = pop_envelope( inputs::input_list_0());
        if ( env_in_0->is_poisoned() )break;
        size_in_0 = env_in_0->get_size();
        pos_in_0 = 0;
        data_in_0 = env_in_0->get_column( col_in_0).get_data< int>();
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
      auto batch_size = std::min(std::min(std::min(std::numeric_limits<unsigned>::max(), size_in_0 - pos_in_0), size_in_1 - pos_in_1), size_out_0 - pos_out_0);
      __m128i var_LD_int32_t_id2_tw4_0_ata_in = (__m128i)(LDS(_mm_load_si128,(__m128i*)&data_in_0[pos_in_0+j],4));
      __m128i var_LD_int32_t_id3_tw4_0_atb_in = (__m128i)(LDS(_mm_load_si128,(__m128i*)&data_in_1[pos_in_1+j],4));
      __m128i var_ADD_int32_t_id4_tw4_0_atc_add = (__m128i)(_mm_add_epi32(var_LD_int32_t_id2_tw4_0_ata_in,var_LD_int32_t_id3_tw4_0_atb_in));
      STS(_mm_store_si128,(__m128i*)&data_out_0[pos_out_0+j],var_ADD_int32_t_id4_tw4_0_atc_add,4) /*var_ST_int32_t_id5_tw4_0_atd_out*/;
      pos_in_0 += batch_size;
      pos_in_1 += batch_size;
      pos_out_0 += batch_size;
      if ( pos_out_0 )
      {
        env_out_0->set_size( pos_out_0);
        send_envelope( outputs::output_list_0(), env_out_0);
      }
      send_poisoned( outputs::output_list_0());
    }
  }
}
;
