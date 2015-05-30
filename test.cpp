
#include "ctb.h"

int main()
{
  graph_default::self_test();
  writer_default::self_test();
  loader_default::self_test();
  ctb_default::self_test();

  return 0;
}

