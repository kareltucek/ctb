
#ifndef TUPLES_GUARD
#define TUPLES_GUARD

#include "defines.h"
#include <vector>
#include <tuple>
#include <assert.h>

//reinventing the wheel

#define MAKE(T) \
  T make_##T(T t){return t;};


#define STRUCT(NAME, T) \
  struct NAME##_struct { T }; \
    CONSTRUCTOR(NAME, NAME##_struct)
  

#define GLUE_PTR(T, t) \
      operator T*() { return t; } \
      T operator*() { return *t; }\
      T* operator->() { return t; }

#endif
