#include <cstddef>
#include <limits>
#include <array>

/**
 * This structure provides a sample implementation of the context structure which the resulting code of the 'cf' alias environment uses. This structure altogether with data-filling algorithm may be created and passed via the 7 generated macros. The first 6 macros provide type and index lists of the graph. The last one (RUN_BOX) is expanded with these six parameters if it is defined. 
 *
 * The idea is that the RUN_BOX macro should create the context type structures and pass them to a processing function. This processing function then fills these contexts (either using static templates or dynamic data) and uses the created box to process them.
 *
 * The structure may be created as follows:
 *   context<ctx_indices<input_indices>, ctx_types<input_types>, input_types_il>;
 *
 * The context provides fields for the computation as well as all information the data-preparation may need in both statically and dynamically processable form.
 *
 * The input_types and input_indices are comma-separated lists of ioindices and of corresponding types. The input_types_il list is type list interleaved with dummy records in case that the sequence of indices is not continuous.
 * */

template< std::size_t ... idx >
struct ctx_indices
{
  static const std::size_t size = sizeof...(idx);
  static const std::array<std::size_t, sizeof...(idx)> data;
};

template< std::size_t ... idx >
const std::array<std::size_t, sizeof...(idx)> ctx_indices<idx...>::data = {idx...};

template< typename...T >
struct ctx_types
{
  static const std::size_t size = sizeof...(T);
};

template < typename IDCTX, typename TYPECTX, typename T0 = int, typename T1 = int, typename T2 = int, typename T3 = int, typename T4 = int, typename T5 = int, typename T6 = int, typename T7 = int, typename T8 = int, typename T9 = int, typename T10 = int, typename T11 = int, typename T12 = int, typename T13 = int, typename T14 = int, typename T15 = int >
struct context
{
  typedef TYPECTX types;

  T0* data_0;
  T1* data_1;
  T2* data_2;
  T3* data_3;
  T4* data_4;
  T5* data_5;
  T6* data_6;
  T7* data_7;
  T8* data_8;
  T9* data_9;
  T10* data_10;
  T11* data_11;
  T12* data_12;
  T13* data_13;
  T14* data_14;
  T14* data_15;

  uint8_t offset_0;
  uint8_t offset_1;
  uint8_t offset_2;
  uint8_t offset_3;
  uint8_t offset_4;
  uint8_t offset_5;
  uint8_t offset_6;
  uint8_t offset_7;
  uint8_t offset_8;
  uint8_t offset_9;
  uint8_t offset_10;
  uint8_t offset_11;
  uint8_t offset_12;
  uint8_t offset_13;
  uint8_t offset_14;
  uint8_t offset_15; 

  std::size_t index_0;
  std::size_t index_1;
  std::size_t index_2;
  std::size_t index_3;
  std::size_t index_4;
  std::size_t index_5;
  std::size_t index_6;
  std::size_t index_7;
  std::size_t index_8;
  std::size_t index_9;
  std::size_t index_10;
  std::size_t index_11;
  std::size_t index_12;
  std::size_t index_13;
  std::size_t index_14;
  std::size_t index_15; 

  std::size_t size_0;
  std::size_t size_1;
  std::size_t size_2;
  std::size_t size_3;
  std::size_t size_4;
  std::size_t size_5;
  std::size_t size_6;
  std::size_t size_7;
  std::size_t size_8;
  std::size_t size_9;
  std::size_t size_10;
  std::size_t size_11;
  std::size_t size_12;
  std::size_t size_13;
  std::size_t size_14;
  std::size_t size_15;

  void** data[16] = {
    (void**)&data_0
      , (void**)&data_1
      , (void**)&data_2
      , (void**)&data_3
      , (void**)&data_4
      , (void**)&data_5
      , (void**)&data_6
      , (void**)&data_7
      , (void**)&data_8
      , (void**)&data_9
      , (void**)&data_10
      , (void**)&data_11
      , (void**)&data_12
      , (void**)&data_13
      , (void**)&data_14
      , (void**)&data_15
  };

  std::size_t* index[16] = {
    &index_0
      , &index_1
      , &index_2
      , &index_3
      , &index_4
      , &index_5
      , &index_6
      , &index_7
      , &index_8
      , &index_9
      , &index_10
      , &index_11
      , &index_12
      , &index_13
      , &index_14
      , &index_15
  };

  uint8_t* offset[16] = {
    &offset_0
      , &offset_1
      , &offset_2
      , &offset_3
      , &offset_4
      , &offset_5
      , &offset_6
      , &offset_7
      , &offset_8
      , &offset_9
      , &offset_10
      , &offset_11
      , &offset_12
      , &offset_13
      , &offset_14
      , &offset_15
  };

  std::size_t* size[16] = {
    &size_0
      , &size_1
      , &size_2
      , &size_3
      , &size_4
      , &size_5
      , &size_6
      , &size_7
      , &size_8
      , &size_9
      , &size_10
      , &size_11
      , &size_12
      , &size_13
      , &size_14
      , &size_15
  };

  std::size_t typesize[16] = { sizeof(T0) 
    , sizeof(T1) 
      , sizeof(T2) 
      , sizeof(T3) 
      , sizeof(T4) 
      , sizeof(T5) 
      , sizeof(T6) 
      , sizeof(T7) 
      , sizeof(T8) 
      , sizeof(T9) 
      , sizeof(T10) 
      , sizeof(T11) 
      , sizeof(T12) 
      , sizeof(T13) 
      , sizeof(T14) 
      , sizeof(T15) 
  };


  const std::array<std::size_t, IDCTX::size> available = IDCTX::data;

  void recalculate_offsets()
  {
    for(int i : available)
    {
      std::size_t addr = (std::size_t)*data[i];
      *offset[i] = ((addr + typesize[i] * (*index[i])) % 16) / typesize[i];
    }
  }

  std::size_t calculate_processable()
  {
    std::size_t m = std::numeric_limits<std::size_t>::max();
    for(int i : available)
      m = std::min(m, *size[i] - *index[i]);
    return m;
  }

  void reset_indices()
  {
    for(int i : available)
      *index[i] = 0;
  }

  void clear()
  {
    for(int i = 0; i < 16; i++)
    {
      *data[i] = NULL;
      *index[i] = 0;
      *offset[i] = 0;
      *size[i] = 0;
    }
  }

  context() 
  {
    clear();
  }
};



