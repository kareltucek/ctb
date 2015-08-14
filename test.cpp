#include "loader_csv.h"
#include "tagmaster.h"
#include "parser.h"
#include "ctb.h"

int main()
{
  using namespace ctb;

  graph_default::self_test();
  writer_default::self_test();
  loader_default::self_test();
  ctb_default::self_test();
  csvloader_default::self_test();
  tagmaster_default::self_test();
  parser::self_test();

  return 0;
}

