void buffer_test1()
{
  std::cout << "testing vector buffer" << std::endl;
  ${BUFFER_DECL(4, 1, 1, 1, 123, int)};

  int inside = 0; 

  assert(!${BUFFER_FULL(123)});
  assert(${BUFFER_EMPTY(123)});

  for(int i = 0; !${BUFFER_FULL(123)}; i++)
  {
    ${BUFFER_PUSH_SIMPLE(4, 1, 123, i)};
    ++inside;
  }

  assert(inside == 4);
  assert(${BUFFER_FULL(123)});
  assert(!${BUFFER_EMPTY(123)});

  for(int i = 0; !${BUFFER_EMPTY(123)}; i++)
  {
    int a;
    ${BUFFER_PEEK_SIMPLE(4,1,123,a)};
    ${BUFFER_CONSUME_ONE(123)};
    assert(a == i);
  }
  assert(!${BUFFER_FULL(123)});
  assert(${BUFFER_EMPTY(123)});
}

void buffer_test2()
{
  std::cout << "testing vector buffer" << std::endl;
  ${BUFFER_DECL(12, 4, 4, 4, 123, __m128i)};

  int inside = 0; 

  assert(!${BUFFER_FULL(123)});
  assert(${BUFFER_EMPTY(123)});

  for(int i = 0; !${BUFFER_FULL(123)}; i++)
  {
    ${BUFFER_PUSH_ONE(12, 4, 123, epi32, i)};
    ++inside;
  }

  assert(inside == 12);
  assert(${BUFFER_FULL(123)});
  assert(!${BUFFER_EMPTY(123)});

  for(int i = 0; !${BUFFER_EMPTY(123)}; i++)
  {
    int a;
    ${BUFFER_PEEK_ONE(12,4,123,epi32,a)};
    ${BUFFER_CONSUME_ONE(123)};
    assert(a == i);
    --inside;
  }
  assert(!${BUFFER_FULL(123)});
  assert(${BUFFER_EMPTY(123)});

  for(int i = 0; !${BUFFER_FULL(123)}; i++)
  {
    ${BUFFER_PUSH_ONE(12, 4, 123, epi32, i)};
    ++inside;
  }

  ${BUFFER_CONSUME_ONE(123)};
  ${BUFFER_CONSUME_ONE(123)};
  inside -= 2;
  
  assert(!${BUFFER_EMPTY_GRANULAR(123)});
  assert(${BUFFER_FULL_GRANULAR(123)});

  for(int i = 2; !${BUFFER_EMPTY_GRANULAR(123)}; )
  {
    __m128i a;
    ${BUFFER_PEEK_VECTOR(12, 4, 123, epi32, a)};
    uint32_t r[4];
    _mm_storeu_si128((__m128i*)&r, a);
    assert(r[0] == i++);
    assert(r[1] == i++);
    assert(r[2] == i++);
    assert(r[3] == i++);
    inside-=4;
    ${BUFFER_CONSUME_VECTOR(123)};
  }
  assert(inside == 2);
  assert(${BUFFER_EMPTY_GRANULAR(123)});
  assert(!${BUFFER_FULL_GRANULAR(123)});
  assert(!${BUFFER_EMPTY(123)});
  assert(!${BUFFER_FULL(123)});
}

void buffer_test()
{
  buffer_test1();
  buffer_test2();
}

