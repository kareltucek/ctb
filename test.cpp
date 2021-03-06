#include "cartesian_multiplier.h"
#include "loader_csv.h"
#include "tagmaster.h"
#include "parser.h"
#include "ctb.h"
#include "multicont.h"
#include "aliasenv_cfmacros.h"

int main(int c, char** cc)
{
  using namespace ctb;

  try
  {
    warn_as_error=true;
    allow_graphs=false;
    imp_cont_default::self_test();
    cartesian_multiplier_default::self_test(); 
    graph_basic_default::self_test();
    graph_factor_default::self_test();
    writer_default::self_test();
    loader_default::self_test();
    ctb_default::self_test();
    csvloader_default::self_test();
    tagmaster_default::self_test();
    parser::self_test();
    aliasenv_cfmacros::self_test();
  }
  catch(const error_struct& err)
  {
    std::cerr << err.first << std::endl;
    return 1;
  }

  return 0;
}

