#include <fstream>
#include <utility>

#include "bobox_manager.hpp"
#include "bobox_results.hpp"
#include "bobox_bobolang.hpp"
#include "bobox_basic_object_factory.hpp"
#include <emmintrin.h>
#include <smmintrin.h>
#include <stdlib.h>

class $1_box : public bobox::basic_box {
public:
  typedef generic_model<$1_box> model;

  $1_box(const bobox::box_parameters_pack &box_params) : bobox::basic_box(box_params) {}

  BOBOX_BOX_INPUTS_LIST($2);
  BOBOX_BOX_OUTPUTS_LIST($3);
private:
  virtual void sync_body() BOBOX_OVERRIDE
  {
    $4

    for (;;)
    {
      $5

      auto batch_size = $6;

      $7

      $8

      $9
    }

    $10
  }
};
