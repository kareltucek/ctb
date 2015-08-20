#include "ctb.h"

int main(int count, char**args)
{
  using namespace ctb;

  ctb_default b;
  try
  {
    b.command_stream_cmdline(count, args);
  }
  catch(const error_struct& err)
  {
    std::cerr << err.first << std::endl;
    return 1;
  }
#ifndef TESTOVANI
  catch(std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
#endif
  return 0;
}

