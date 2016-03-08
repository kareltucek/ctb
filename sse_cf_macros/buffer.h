#include "pp_macros.h"

/**
 * This is a simple implementation of vector buffer. This creates <size> variables (possibly vectors) and provides a push pull API. This is implemented as round robin queue.
 *
 * API:
 * BUFFER_DECL(size, id, type):
 * BUFFER_EMPTY(size, id):
 * BUFFER_FILLED(size, id):
 * BUFFER_CONSUME(size, id): decrements the size counter
 * BUFFER_PEEK(size, id, to): assigns current head to the <to> variable
 * BUFFER_POP(size, id, to): abbreviation for PEEK && CONSUME
 * BUFFER_PUSH(size, id, val): pushes val to the queue and increments the size counter
 * */

#define BUFFER_SIMPLE_DECL(i, type, id)\
  type generic_declaration_##id##_##i;

#define BUFFER_SIMPLE_RETURN(i, size, id, to) \
  case i: to = generic_declaration_##id##_##i; break;

#define BUFFER_SIMPLE_ASSIGN(i, size, id, val) \
  case i: generic_declaration_##id##_##i = val; break;

#define BUFFER_DECL(size, id, type) \
  EVAL(REPEAT(size, BUFFER_SIMPLE_DECL, type, id)) \
  int generic_declaration_readat_##id = 0; \
  int generic_declaration_contains_##id = 0 \

#define BUFFER_FILLED(size, id) \
  (generic_declaration_contains_##id == size)

#define BUFFER_EMPTY(size, id) \
  (generic_declaration_contains_##id == 0)

#define BUFFER_CONSUME(size, id) \
  generic_declaration_readat_##id = ((generic_declaration_readat_##id + 1) % size); \
  generic_declaration_contains_##id--;

#define BUFFER_INC(size, id) \
  generic_declaration_contains_##id++;

#define BUFFER_PEEK(size, id, to) \
  switch(generic_declaration_readat_##id){ \
    EVAL(REPEAT(size, BUFFER_SIMPLE_RETURN, size, id, to)) \
  }\

#define BUFFER_POP(size, id, to) \
  BUFFER_PEEK(size,id,to) \
  BUFFER_CONSUME(size,id);

#define BUFFER_PUSH(size, id, val) \
  switch((generic_declaration_readat_##id + generic_declaration_contains_##id) % size){ \
    EVAL(REPEAT(size, BUFFER_SIMPLE_ASSIGN, size, id, val))\
  }\
  BUFFER_INC(size, id) \

