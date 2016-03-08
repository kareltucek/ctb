#include "buffer.h"

void buffer_test()
{
  std::cout << "testing vector buffer" << std::endl;
  BUFFER_DECL(4, 0, int);

  int inside = 0; 

  assert(!BUFFER_FILLED(4, 0));
  assert(BUFFER_EMPTY(4, 0));

  for(int i = 0; !BUFFER_FILLED(4, 0); i++)
  {
    BUFFER_PUSH(4, 0, i);
    ++inside;
  }

  assert(inside == 4);
  assert(BUFFER_FILLED(4, 0));
  assert(!BUFFER_EMPTY(4, 0));

  for(int i = 0; !BUFFER_EMPTY(4, 0); i++)
  {
    int a;
    BUFFER_POP(4, 0, a);
    assert(a == i);
  }
  assert(!BUFFER_FILLED(4, 0));
  assert(BUFFER_EMPTY(4, 0));
}

