#include "ctb.h"

int main(int count, char**args)
{
  using namespace ctb;

  ctb_default b;
  try
  {
    b.parse_command_stream(std::cin);
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

