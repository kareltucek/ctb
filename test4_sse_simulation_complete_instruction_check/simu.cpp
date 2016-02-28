#include "output.h"

int main()
{
  try
  {
    tester t;
    t.test(160000);
  }
  catch(int& e)
  {
    std::cout << "running again with breakpoint set" << std::endl;
    try
    {
    tester t;
    t.test(e);
    }
    catch(int& f)
    {
      return 1;
      if(e != f)
      {
        std::cout << "failed at different line" << std::endl;
        tester t;
        t.test(f);
      }
    }
    return 1;
  }
  return 0;
}
