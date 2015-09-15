#include "output.h"

int main()
{
  try
  {
    tester t;
    t.test(2000000);
  }
  catch(int& e)
  {
    std::cout << "running again with breakpoint set" << std::endl;
    try
    {
    tester t;
    t.test(e);
    }
    catch(int& e)
    {
      return 1;
    }
    return 1;
  }
  return 0;
}
