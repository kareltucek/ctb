#include "ctb.h"

int main(int count, char**args)
{
  ctb_default b;
  try
  {
  b.cmdline(count,args);
  }
  catch(const std::string& str)
  {
    std::cerr << str << std::endl;
  }
  catch(const char* str)
  {
    std::cerr << str << std::endl;
  }
  return 0;
}

