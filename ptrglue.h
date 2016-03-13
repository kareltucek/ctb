
#ifndef TUPLES_GUARD
#define TUPLES_GUARD

#include "defines.h"
#include <vector>
#include <tuple>
#include <assert.h>

//reinventing the wheel

//AUTO lets as annotate the obscure auto call. The expected usage is at places where we know very well what type we are working with, but are in no need ofexplicitly writting its full path.
#define AUTO(t) auto

//MAKE creates an identity method which is meant to act as a constructor e.g. MAKE(node); will create method which may be called like make_node({1,2,3,"string member"})
#define MAKE(T) \
  static T make_##T(T t){return t;};


#define STRUCT(NAME, T) \
  struct NAME##_struct { T }; \
CONSTRUCTOR(NAME, NAME##_struct)


#define GLUE_PTR(T, t) \
  operator T*() { return t; } \
T operator*() { return *t; }\
T* operator->() { return t; }

#endif
