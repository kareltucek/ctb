#include "pp_macros.h"

/**
 * "Ye who enter here abandon all hope!" [Dante Alighieri]
 *
 * These macros were rewritten using our own preprocessor, in aliasenv_cf.h. The reason for this is the fact that C norm explicitly forbids recursive macros, which effectively means that any recursive macros are real pain to use due to frequent malfunctionalities.
 * */

/**
 * This is a simple implementation of vector buffer. This creates <size> variables (possibly vectors) and provides a push pull API. This is implemented as round robin queue.
 *
 * API:
 * BUFFER_DECL(size, id, type):
 * BUFFER_EMPTY(size, id):
 * BUFFER_FILLED(size, id):
 * BUFFER_CONSUME(size, id): increments the read counter and decrements the size counter
 * BUFFER_PEEK(size, id, to): assigns current head to the <to> variable
 * BUFFER_POP(size, id, to): abbreviation for PEEK && CONSUME
 * BUFFER_PUSH(size, id, val): pushes val to the queue and increments the size counter
 *
 * For the resulting code, see the test9. There are tests altogether with an explicit preprocess step, which shows the resulting code and allows simple debug.
 * */

#define FIRST(a, args...) a
#define SECOND(a, b, args...) b
#define SECOND_L(a, b) b
#define THIRD(a, b, c, args...) c
#define THIRD_L(a, b, c) c
#define FOURTH(a, b, c, d, args...) d
#define FOURTH_L(a, b, c, d) d

#define CAT2(a, b) a##b
#define CAT3(a, b, c) a##b##c
#define CAT4(a, b, c, d) a##b##c##d

#define CAT2_ID() CAT2
#define CAT3_ID() CAT3
#define CAT4_ID() CAT4

//serves for switch between BOOST and nonboost implementation of repeat
#define i a

//#define EMPTY()
//#define DEFER(id) id EMPTY()
//#define EXPAND(...) __VA_ARGS__

#define BUFFER_SIMPLE_DECL(i, type, id)\
  type CAT4(gendecl_, id, _, i);

#define BUFFER_SIMPLE_DECL_HELPER(i, z, data)\
  BUFFER_SIMPLE_DECL(i, FIRST data, SECOND_L data);
  //FIRST data gendecl_##SECOND_L data##_##i;

#define BUFFER_SIMPLE_RETURN(i, size, id, to) \
  case i: to = CAT4(gendecl_, id, _, i); break;

#define BUFFER_SIMPLE_RETURN_HELPER(i, z, data) \
  BUFFER_SIMPLE_RETURN(i, FIRST data, SECOND data, THIRD_L data)

#define BUFFER_SIMPLE_ASSIGN(i, size, id, val) \
  case i: CAT4(gendecl_,id,_,i) = val; break;

#define BUFFER_SIMPLE_ASSIGN_HELPER(i, z, data) \
  BUFFER_SIMPLE_ASSIGN(i, FIRST data, SECOND data, THIRD_L data)

#define BUFFER_DECL(size, id, type) \
  EVAL(REPEAT(size, BUFFER_SIMPLE_DECL_HELPER, 0, (type, id))) \
  int gendecl_readat_##id = 0; \
  int gendecl_contains_##id = 0 \

#define BUFFER_FILLED(size, id) \
  (gendecl_contains_##id == size)

#define BUFFER_EMPTY(size, id) \
  (gendecl_contains_##id == 0)

#define BUFFER_NONEMPTY(size, id) \
  (gendecl_contains_##id > 0)

#define BUFFER_CONSUME(size, id) \
  gendecl_readat_##id = ((gendecl_readat_##id + 1) % size); \
  gendecl_contains_##id--;

#define BUFFER_INC(size, id) \
  gendecl_contains_##id++;

#define BUFFER_PEEK(size, id, to) \
  switch(gendecl_readat_##id){ \
    EVAL(REPEAT(size, BUFFER_SIMPLE_RETURN_HELPER, 0, (size, id, to))) \
  }\

#define BUFFER_POP(size, id, to) \
  BUFFER_PEEK(size,id,to) \
  BUFFER_CONSUME(size,id);

#define BUFFER_PUSH(size, id, val) \
  switch((gendecl_readat_##id + gendecl_contains_##id) % size){ \
    EVAL(REPEAT(size, BUFFER_SIMPLE_ASSIGN_HELPER, 0, (size, id, val)))\
  }\
  BUFFER_INC(size, id) \

/**
 * FBUFFER is a bit more complex buffer implementation. Unlike simple buffer, here we deal with single elements as well. In comparison to simple buffer, we cannot push entire vectors, but we can retrieve entire vectors.
 * */

#define FBUFFER_SIMPLE_DECL(i, type, id)\
  type CAT4(gendecl_,id,_,i);

#define FBUFFER_SIMPLE_DECL_HELPER(i, z, data)\
  FBUFFER_SIMPLE_DECL(i, FIRST data, SECOND_L data)

#define FBUFFER_DECL(size, vsize, vcount, ingranularity, outgranularity, id, type) \
  EVAL(REPEAT(vcount, FBUFFER_SIMPLE_DECL_HELPER, 0, (type, id))) \
  int gendecl_readat_##id = 0; \
  int gendecl_contains_##id = 0; \
  static const int gendecl_size_##id = size; \
  static const int gendecl_outgran_##id = outgranularity; \
  static const int gendecl_ingran_##id = ingranularity;  \
  static const int gendecl_vsize_##id = vsize; 

#define FBUFFER_FILLED_GRANULAR(id) \
  (gendecl_size_##id - gendecl_ingran_##id < gendecl_contains_##id )

#define FBUFFER_EMPTY_GRANULAR(id) \
  (gendecl_contains_##id < gendecl_outgran_##id)

#define FBUFFER_EMPTY(id) \
  (gendecl_contains_##id == 0)

#define FBUFFER_FILLED(id) \
  (gendecl_contains_##id == gendecl_size_##id)

#define FBUFFER_INC(id) \
  gendecl_contains_##id++;

#define FBUFFER_SIMPLE_ASSIGN(i, vcount, id, typeabbrev, val) \
  case i: DEFER(CAT4_ID)()(gendecl_,id,_,DIV(i,vcount)) = DEFER(CAT2_ID)()(_mm_insert_,typeabbrev)(DEFER(CAT4_ID)()(gendecl_,id,_,DIV(i,vcount)), val, i % CAT2(gendecl_vsize_,id)); break;

#define FBUFFER_SIMPLE_ASSIGN_HELPER(i, z, data) \
  FBUFFER_SIMPLE_ASSIGN(i, FIRST data, SECOND data, THIRD data, FOURTH_L data)

#define FBUFFER_PUSH_ONE(size,  vcount, id, typeabbrev, val) \
  switch((gendecl_readat_##id + gendecl_contains_##id) % size){ \
    REPEAT(size, FBUFFER_SIMPLE_ASSIGN_HELPER, 0, (id, type, typeabbrev, val))\
  }\
  FBUFFER_INC(id) \

#define FBUFFER_CONSUME_ONE(id) \
  gendecl_readat_##id = ((gendecl_readat_##id + 1) % gendecl_size_##id); \
  gendecl_contains_##id--;

#define BUFFER_SIMPLE_RETURN_ONE(i, vcount, id, typeabbrev, to) \
  case i: to = CAT2(_mm_extract_,typeabbrev)(CAT4(gendecl_,id,_,DIV(i,vcount)), val, MOD(i, CAT2(gendecl_vsize_,id))); break;

#define BUFFER_SIMPLE_RETURN_ONE_HELPER(i, z, data) \
  BUFFER_SIMPLE_RETURN_ONE(i, FIRST data, SECOND data, THIRD data, FOURTH_L data)

#define FBUFFER_PEEK_ONE(size, vcount, id, typeabbrev, to) \
  switch(gendecl_readat_##id){ \
    REPEAT(size, FBUFFER_SIMPLE_RETURN_ONE_HELPER, 0, (vcount, id, type, typeabbrev, to)) \
  }\

#define FBUFFER_CONSUME_VECTOR( id) \
  gendecl_readat_##id = ((gendecl_readat_##id + gendecl_vsize_##id) % gendecl_size_##id); \ gendecl_contains_##id -= gendecl_vsize_##id;

#define BUFFER_SIMPLE_RETURN_VECTOR(i, vcount, id, typeabbrev, to) \
  case i: to = _mm_alignr_##typeabbrev(gendecl_##id##_DIV(i,vcount), gendecl_readat_##id##_##MOD(INC(DIV(i,vcount)),vcount), (i%gendecl_vsize_##id)*(16/gendecl_vsize_##id)); break;

#define BUFFER_SIMPLE_RETURN_VECTOR_HELPER(i, z, data) \
  BUFFER_SIMPLE_RETURN_VECTOR(i, FIRST data, SECOND data, THIRD data, FOURTH_L data)

#define FBUFFER_PEEK_VECTOR(size, vcount, id, typeabbrev, to) \
  switch(gendecl_readat_##id){ \
   REPEAT(size, FBUFFER_SIMPLE_RETURN_VECTOR_HELPER, ( vcount, id, type, typeabbrev, to) ) \
  }\

#define FBUFFER_POP_ONE(size, vcount, id, typeabbrev, to) \
  FBUFFER_PEEK_ONE( size, vcount, id, typeabbrev, to) \
  FBUFFER_CONSUME_ONE( id );

#define FBUFFER_POP_VECTOR(size, vcount, id, typeabbrev, to) \
  FBUFFER_PEEK_VECTOR( size, vcount, id, typeabbrev, to) \
  FBUFFER_CONSUME(  id);


