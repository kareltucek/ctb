
#ifndef INSTRUCTION_GUARD
#define INSTRUCTION_GUARD

#include "writer.h"
#include "graph.h"

#include <map>
#include <vector>

namespace ctb
{
  /**
   * This class holds information for conversion of instructions from graph op codes to actual code.
   *
   * The logical structure is the following:
   * \code{txt}
   * - instruction table
   *   - operations
   *     operation represents a logical operation such as addition
   *     - instructions
   *       instruction structure represents realization of its parent operation on data of some length
   *     e.g. '$arg1 + $arg2'
   *   - types
   *     type represents again a logical type sucha as an integer
   *     - type versions
   *       specifies data type of a particular width, such as 'int' or '__m128i'
   *     - type conversion
   *       provides a means of splitting or gluing two registers together 
   *       two code lines are provided for split of an instruction
   *       e.g. _mm_unpacklo_epi32($arg1,$arg2)
   * \endcode
   *
   * All 'code' fields are supposed to be in form of rhs expressions, which are later substitued into abbreviations specified by the model_empty class (or its descendants). For special purposes there may later be a 'custom_code' field introduced, which will allow specification of the full code on user side. All code fields have a shell like expansion format defined by the writer class with abbreviations provided by the model hierarchy. At the time of writting this, the following abbreviations are available in code generation:
   *  - $type      - e.g. 'int'
   *  - $name      - name generated for the variable
   *  - $operation - operation::instruction::code
   *  - $cindex    - for input or output node this is the serial number of this node (e.g. id of the input list which is associated with the instruction in question)
   *  - $arg1      - access code obtained 'from the input edges'...
   *  - $arg2
   *  - $arg3
   *  - $vindex    - index of the generated code. E.g. for instruction width = 4 and data granularity 16 the generator will be called in order with this set to 0, 4, 8 and 12
   *
   *  The input interface consists of the instruction_table::addtype and the instruction_table::add_operation create new operation/type with specified id and return a reference to it, which then can be used to fill in code snippets for realization of the code/snippets.
   *
   *  The output interface consists of two 'decode' methods, which return either type of operation. The data retrieval interface is the interface of the operation class, which provides retrieval of all codes, including the type-related stuff.
   * */

  template <class T>
    class instruction_table //provides transition from opcode to abstract operation
    {
      private:
        //holds hardware type info (if needed, theoreticaly is instruction specific and thus may not be neccessary)
        //holds information for changing breadth of data
        struct type
        {
          typedef graph_generic<dummy, int, false, type> graph_distance_t;
          struct type_version
          {
            int width;
            std::string code;
            type_version(int w, const std::string& c);
          }
          ;
          struct conversion
          {
            int width_in;
            int width_out;
            std::string code1;
            std::string code2;
            conversion(int in, int out, const std::string& c1, const std::string& c2);
          }
          ;
          //TODO index this a bit intelligently...
          std::vector<type_version> versions;
          std::vector<conversion> conversions;
          mutable graph_distance_t distances;
          void addcode_type(int w, const std::string& c) ;
          void addcode_conversion(int from, int to, const std::string& c1, const std::string& c2);
        }
        ;
        typedef std::map<typename T::tid_t, type*> typetable_t;
        class operation //holds general operation traits
        {
          private:
            mutable int imbued_width;
            struct instruction //holds information for generation
            {
              int width;
              int width_in;
              int width_out;
              std::string code;
              instruction(int wi, int wo, const std::string& c);
            }
            ;
            std::vector<instruction> versions;
            type* mytype;
            ;//std::vector<typename T::tid_t> in_types
            typename T::tid_t out_type;
            typename T::flag_t flags;
            typename T::opid_t opid;
          public:
            operation(typename T::opid_t i, typename T::tid_t ot, typename T::flag_t f, type* t);
            void addcode(int wi, int wo, const std::string& c);
            bool is(typename T::flag_t f) const ;
            int get_max_width(int bound = 1000000000, int* in = NULL, int* out = NULL)const;
            void imbue_width(int w)const;
            std::string get_type_string(int w = -1)const;
            std::string get_op_string(int w = -1)const;
            bool get_conv_string(int from, int to, std::string& c1, std::string& c2, std::string& type)const;
            const typename type::graph_distance_t& get_conversion_graph() const;
        }
        ;
        std::map<typename T::opid_t, operation*> instab;
        typetable_t typetab;
      public:
        typedef operation operation_t; 
        typedef type type_t; 
        const operation_t& dec(typename T::opid_t type) const ;
        const type_t& dectype(typename T::tid_t type) const ;
        operation_t& addoperation(typename T::opid_t op, typename T::tid_t t, typename T::flag_t f) ;
        type_t& addtype(typename T::tid_t t) ;
        void clear() ; 
    } ;

  typedef instruction_table<traits> instruction_table_default;

  template <class T>
    instruction_table<T>::type::type_version::type_version(int w, const std::string& c) : width(w), code(c)
  {
  }

  template <class T>
    instruction_table<T>::type::conversion::conversion(int i, int o, const std::string& c1, const std::string& c2) : width_in(i), width_out(o), code1(c1), code2(c2)
  {
  }

  template <class T>
    void instruction_table<T>::type::addcode_type(int w, const std::string& c)
    {
      versions.push_back(type_version(w, c));
      distances.addvert(w, false, false);
    }

  template <class T>
    void instruction_table<T>::type::addcode_conversion(int in, int out, const std::string& c1,const std::string& c2)
    {
      conversions.push_back(conversion(in, out, c1, c2));
      distances.addedge(in,out);
    }

  template <class T>
    instruction_table<T>::operation::instruction::instruction(int wi, int wo, const std::string& c) : code(c), width_in(wi), width_out(wo), width(std::max(wi, wo))
  {
  }

  template <class T>
    instruction_table<T>::operation::operation(typename T::opid_t i, typename T::tid_t ot, typename T::flag_t f, type* t) : opid(i), mytype(t), out_type(ot), flags(f)
  {
  }

  template <class T>
    const typename instruction_table<T>::type::graph_distance_t& instruction_table<T>::operation::get_conversion_graph() const
    {
      mytype->distances.calculate_distances();
      return mytype->distances;
    }

  template <class T>
    void instruction_table<T>::operation::addcode(int wi, int wo, const std::string& c)
    {
      versions.push_back(instruction(wi, wo, c));
    }

  template <class T>
    bool instruction_table<T>::operation::is(typename T::flag_t f) const 
    {
      return (f & flags) > 0;
    }

  template <class T>
    int instruction_table<T>::operation::get_max_width(int bound, int* in, int* out)const
    {
      int w = 0;
      for(auto ins : versions)
      {
        if(ins.width <= bound && ins.width > w)
        {
          w = ins.width;
          if(in != NULL) *in = ins.width_in;
          if(out != NULL) *out = ins.width_out;
        }
      }
      return w;
    }

  template <class T>
    void instruction_table<T>::operation::imbue_width(int w)const
    {
      imbued_width = w;
    }

  template <class T>
    std::string instruction_table<T>::operation::get_type_string(int w)const
    {
      if(w == -1)
        w = imbued_width;
      for( auto type : mytype->versions)
      {
        if ( type.width == w)
          return type.code;
      }
      throw std::string("type of width = ").append(std::to_string(w)).append(" at operation ").append(opid).append(" not found");
    }

  template <class T>
    std::string instruction_table<T>::operation::get_op_string(int w)const
    {
      if(w == -1)
        w = imbued_width;
      for( auto ins : versions)
      {
        if ( ins.width_in == w)
          return ins.code;
      }
      throw std::string("instruction of width_in = ").append(std::to_string(w)).append(" at operation ").append(opid).append(" not found");
    }

  template <class T>
    bool instruction_table<T>::operation::get_conv_string(int from, int to, std::string& c1, std::string& c2, std::string& t) const
    {
      for( auto con : mytype->conversions)
      {
        if ( con.width_in == from && con.width_out == to)
        {
          c1 = con.code1;
          c2 = con.code2;
          t = get_type_string(to);
          return true;
        }
      }
      return false;
    }

  template <class T>
    const typename instruction_table<T>::operation_t& instruction_table<T>::dec(typename T::opid_t type)   const
    {
      return *instab.find(type)->second;
    }

  template <class T>
    const typename instruction_table<T>::type_t& instruction_table<T>::dectype(typename T::tid_t type)   const
    {
      return *typetab.find(type)->second;
    }

  template <class T>
    typename instruction_table<T>::operation_t& instruction_table<T>::addoperation(typename T::opid_t op, typename T::tid_t t, typename T::flag_t f)
    {
      operation* ptr = new operation(op, t, f, typetab.find(t)->second);
      instab[op] = ptr;
      return *ptr;
    } 

  template <class T>
    typename instruction_table<T>::type_t& instruction_table<T>::addtype(typename T::tid_t t)
    {
      type* ptr = new type();
      typetab[t] = ptr;
      return *ptr;
    }

  template <class T>
    void instruction_table<T>::clear()  
    {
    }

  template class instruction_table<traits> ;

}

#endif

