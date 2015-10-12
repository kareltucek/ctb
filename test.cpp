#include "cartesian_multiplier.h"
#include "loader_csv.h"
#include "tagmaster.h"
#include "parser.h"
#include "ctb.h"
#include "multicont.h"

int main(int c, char** cc)
{
  using namespace ctb;

  multicont_default::self_test();
  cartesian_multiplier_default::self_test(); 
  graph_basic_default::self_test();
  graph_factor_default::self_test();
  writer_default::self_test();
  loader_default::self_test();
  ctb_default::self_test();
  csvloader_default::self_test();
  tagmaster_default::self_test();
  parser::self_test();

  return 0;
}

