
#ifndef INSTRUCTION_GUARD
#define INSTRUCTION_GUARD

#include "defines.h"
#include "writer.h"
#include "graph_factor.h"
#include "datatypes.h"
#include "tagmaster.h"
#include "taghandler.h"

#include <map>
#include <vector>

namespace ctb
{
  /**
   * Where to search
   * ---------------
   * Note that this doc is a bit deprecated. For up-to-date information please see relevant files:
   * - loader_xml.h - for formal description of fields
   * - aliasenv_generator.h - for up-to-date aliases which are available *inside* the generation environment (i.e. $name like abbreviations which may be used inside of code/code1/code2/code_custom fields.
   * - writer.h - for full syntax supported by preprocessor
   * - ctb.h on how instruction table is supposed to be loaded and used
   *
   * General description
   * -------------------
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
   *       Instruction code may be given in the following forms. Only one of {code|code custom} may be specified.
   *       'code' - A rhs expression. Will be substitued into '$type $name = $code;'
   *       'code custom' - A custom expression. Will be substitued into '$code'
   *       'code declaration' - declaration code will be expanded just as code_custom, but result will be put into a special code container, so the handling environment can put these into global namespace.
   *   - types
   *     type represents again a logical type sucha as an integer
   *     - type versions
   *       specifies data type of a particular width, such as 'int' or '__m128i'
   *     - type conversion
   *       provides a means of splitting or gluing two registers together
   *       one of 3 code types may be specified:
   *       + two code lines are provided for split of an instruction into two halves; semantics of this is {'$type $name = $code1;', '$type $name = $code2'}(names are different in each call) (generated ntimes (with different names and vindexes)
   *         e.g. _mm_unpacklo_epi32($arg1,$arg2)
   *       + code custom - here you get full list of names and it's your problem to cope with them; semantics is '$code_custom' (-||-)
   *       + code generic - this is called once for every instance of split or join with a vector indices as a parameters; semantics is thus {'$code_generic'(with vvindex = 0*w), '$code_generic' (with vvindex = 1*w), '$code_generic'...} (-||-)
   *
   * \endcode
   * 
   * All relevant structures have also a 'note', 'tags' and 'rating' fields. Tags serve for switching hardware-supported/unsupported sets of instructions on and off. Rating serves for choosing better variant when multiple are available (such as a new, mostly unsupported instruction versus an old slow workaround). Note is an arbitrary user note.
   *
   * All 'code' fields are supposed to be in form of rhs expressions, which are later substitued into abbreviations specified by the aliasenv_generator class (or its descendants). For special purposes there may later be a 'code_custom' field introduced, which will allow specification of the full code on user side. All code fields have a shell like expansion format defined by the writer class with abbreviations provided by the aliasenv_maker.hierarchy. At the time of writting this, the following abbreviations are available in code generation:
   * - $type      - e.g. 'int'
   * - $name      - name generated for the variable
   * - $basename  - name of the first variable in vector - e.g. for the purpose of generating one unique variable for every pipe (e.g. for preloads)
   * - $operation - operation::instruction::code
   * - $ioindex    - for input or output node this is the serial number of this node (e.g. id of the input list which is associated with the instruction in question)
   * - $arg1      - access code obtained 'from the input edges'...
   * - $arg2
   * - $arg3
   * - $arg...
   * - $iindex    - input 'vector' index - index of the generated code. E.g. for standard instruction of width = 4 and data granularity 16 the generator will be called in order with this set to 0, 4, 8 and 12;
   * - $oindex    - output 'vector' index - differs for split and join operations
   * - $vindex    - 'vector vector' index - internal index for generic joins and splits. E.g. for split from width 8 to 4*2 at granularity 16 this will be called in two series with 0,2,4,8,0,2,4,8 (and iindexes 0,0,0,0,8,8,8,8). This should always be the difference of iindex and oindex
   *
   * The flag field specifies special semantics for some operations:
   * - input - allows aliasenv direct access to input nodes, has special semantics for managed 'code' field (i.e. is expanded through '$inputcode' instead of '$innercode')
   * - output - -||-
   * - debug - Operations with 'debug' flag set serve for debug. They may be easily used for getting values from a specific part of graph for a specified frame. Debug operation has semantics similar to the output flag except for a few details - these node may be autogenerated using testloader for all or for specified nodes. Width is set to width of the debugged node - i.e. there is no width conversion.
   *
   * The input interface consists of the instruction_table::addtype and the instruction_table::add_operation create new operation/type with specified id and return a reference to it, which then can be used to fill in code snippets for realization of the code/snippets.
   *
   * The output interface consists of two 'decode' methods, which return either type of operation. The data retrieval interface is the interface of the operation class, which provides retrieval of all codes, including the type-related stuff.
   *
   * Instruction selection
   * ---------------------
   * Selection of instructions is driven by a serie of tag handlers* organized into layers. Curently there are two layers:
   * - 'select' layer - defines whether an instruction version can be selected in the run
   * - 'print' layer - defines whether code for an instruction should be printed or not
   * These allow higher layer layer of a program to generate only a part of a graph depending on tags of instructions. 
   *
   * Every layer can contain an arbitrary tag handlers. These are intersected separately for each layer and the result is cached into the instruction version data structure. (this happens in the update_tags() call). 
   *
   * *(Tag handler is basically a functor which receives a stringlist of masks and decides whether those masks satisfy some (user-defined) conditions)
   *
   * template arguments
   * ------------------
   * T - traits
   * */

   template <class T>
   class instruction_table //provides transition from opcode to abstract operation
   {
     public:
       typedef typename T::tid_t tid_t;
       typedef typename T::opid_t opid_t;
     private:
       class type;
       class operation;
       //holds hardware type info (if needed, theoreticaly is instruction specific and thus may not be neccessary)
       //holds information for changing breadth of data
       class type
       {
         private:
           friend instruction_table;
           struct conversion
           {
             const int width_in;
             const int width_out;
             const string code1;
             const string code2;
             const string code_custom;
             const string code_generic;
             const string note;
             const string tags;
             const int rating;
             mutable typename T::flag_t satisfactory;
           } ;
           MAKE(conversion);
           struct type_version
           {
             const int width;
             const string code;
             const string note;
           } ;
           MAKE(type_version);
           instruction_table* parent;
           void update_tags() const;
         public:
           type() = delete;
           type(instruction_table*, int bitwidth);
           typedef graph_general<dummy, int, false> graph_distance_t;
           typename T::opid_t debug_op;
           //TODO index this a bit intelligently...
           /*EAPI*/vector<type_version> versions;
           /*EAPI*/vector<conversion> conversions;
           /*EAPI*/tid_t tid;
           /*EAPI*/int bitwidth;
           /*IAPI*/void add_code_type(int w, const string& c,const string&) ;
           /*IAPI*/void add_code_conversion(int from, int to, const string& c1, const string& c2,const string& cc,const string& cg, const string& n, const string& t,int r);
           mutable graph_distance_t distances; //technically taken just a cache
       }
       ;
       struct expansion
       {
         const string name;
         const string transformer_name;
         const vector<opid_t> arguments;
         const string note;
         const vector<tid_t> in_types; //for possible type inference
         const vector<tid_t> out_types; //for possible type inference
       };
       MAKE(expansion);
       class operation //holds general operation traits
       {
         private:
           mutable int imbued_width;
           friend instruction_table;
           struct ccode_pair
           {
             string name;
             string code;
           };
           MAKE(ccode_pair);
           typedef vector<ccode_pair> ccode_cont;
           struct instruction //holds information for generation
           {
             const int width;
             const int width_in;
             const int width_out;
             const string code;
             const ccode_cont code_custom;
             const string note;
             const string tags;
             const int rating;
             mutable typename T::flag_t satisfactory;
           }
           ;
           MAKE(instruction);
           //vector<typename T::tid_t> in_types
           instruction_table* parent;
           void update_tags() const;
         public:
           typedef ccode_cont ccode_cont_t;
           /*TAPI*/vector<expansion> expansions;
           /*TAPI*/typedef expansion expansion_t;
           /*EAPI*/vector<instruction> versions;
           /*EAPI*/type* mytype;
           /*EAPI*/typename T::tid_t out_type;
           /*EAPI*/vector<typename T::tid_t> in_types;
           /*EAPI*/typename T::flag_t flags; 
           /*EAPI*/typename T::opid_t opid;
           /*IAPI*/void add_code(int wi, int wo, const string& c, const stringlist& cc, const string&,const string&, int r);
           /*IAPI*/void add_expansion(const string&, const string&, const vector<typename T::opid_t>& args, const string&, vector<typename T::tid_t>, vector<typename T::tid_t>);
           /*API*/bool is(typename T::flag_t f) const ;
           /*API*/int get_max_width(int bound = 1000000000, int* in = NULL, int* out = NULL)const;
           /*API*//*DEPRECATED*///void imbue_width(int w)const;
           /*API*/typename T::opid_t get_debug_opid() const;
           /*API*/bool get_type_string(int w, string&)const;
           /*API*/bool get_op_string(int w, string& c, ccode_cont_t& cc, size_t&)const;
           /*API*/bool get_conv_string(int from, int to, string& c1, string& c2, string&cc, string& type, size_t&)const;
           /*API*/const typename type::graph_distance_t& get_conversion_graph() const;
           operation(typename T::opid_t i, typename T::tid_t ot, const vector<typename T::tid_t>& it, typename T::flag_t f, type* t, instruction_table* parent);
           operation() = delete;
       }
       ;
       typedef vector<expansion> expansion_cont_t;
       typedef map<string, expansion_cont_t> expansion_name_cont_t;
       typedef map<string, expansion_name_cont_t> expansion_transformer_cont_t;
       typedef map<typename T::tid_t, type*> typetab_t;
       typedef map<typename T::opid_t, operation*> instab_t;
       typedef set<shared_ptr<taghandler_base>> taghandlerrec_t;
       typedef vector<taghandlerrec_t> taghandlertab_t;
       taghandlertab_t taglists;
       typename T::flag_t get_tag_mask(const string& tags);
       expansion_transformer_cont_t exptab;
     public:
       /*EAPI*/instab_t instab;
       /*EAPI*/typetab_t typetab;

       typedef operation operation_t; 
       typedef type type_t;
       typedef expansion expansion_t;

       /*API*/ const expansion_cont_t& find_expansion(const string& transformer, const string& expansion); 
       /*API*/ const expansion_t& find_expansion(const string& transformer, const string& expansion, const vector<tid_t>& types); 
       /*API*/ void add_tags(shared_ptr<taghandler_base>,size_t layer = gSELECT); 
       /*API*/ void rm_tags(shared_ptr<taghandler_base>,size_t layer = gSELECT);
       /*API*/ bool is_tag_satisfactory(const string&) const;
       /*API*/ void update_tags() const;
       /*API*/ const operation_t& dec(typename T::opid_t type) const ;
       /*API*/ const type_t& dectype(typename T::tid_t type) const ;
       /*IAPI*/ operation_t& add_operation(typename T::opid_t op, typename T::tid_t t, const vector<typename T::tid_t>&it, typename T::flag_t f) ;
       /*IAPI*/ void add_expansion(typename T::opid_t id, const string& n, const string& t, const vector<opid_t>& a, const string& m, vector<tid_t> types, vector<tid_t> types_out);
       /*IAPI*/ type_t& addtype(typename T::tid_t t, int bitwidth = 0) ;
       void clear(bool tags = false) ;
       bool empty();
       ~instruction_table();
   } ;



   typedef instruction_table<traits> instruction_table_default;

   template <class T>
     instruction_table<T>::type::type(instruction_table<T>* p, int bw) : parent(p), bitwidth(bw)
   {
   }

   template <class T>
     void instruction_table<T>::type::update_tags() const
     {
       for(const auto& i : conversions)
         i.satisfactory = parent->get_tag_mask(i.tags);
     }
   template <class T>
     void instruction_table<T>::operation::update_tags() const
     {
       for(const auto& i : versions)
       {
         i.satisfactory = parent->get_tag_mask(i.tags);
       }
     }

   template <class T>
     typename T::opid_t instruction_table<T>::operation::get_debug_opid() const
     {
       return mytype->debug_op;
     }

   template <class T>
     bool instruction_table<T>::empty()
     {
       return instab.empty();
     }


   template <class T>
     void instruction_table<T>::type::add_code_type(int w, const string& c,const string& n)
     {
       versions.push_back(make_type_version({w, c,n}));
       distances.add_vert(w, false, false);
     }

   template <class T>
     void instruction_table<T>::type::add_code_conversion(int in, int out, const string& c1,const string& c2,const string& cc,const string& cg, const string& n,const string& t,int r)
     {
       bool s = parent->is_tag_satisfactory(t);
       conversions.push_back(make_conversion({in, out, c1, c2,cc,cg,n,t,r, s}));
       distances.add_vert(in, false, false);
       distances.add_vert(out, false, false);
       if(s)
         distances.addedge(in,out);
     }


   template <class T>
     instruction_table<T>::operation::operation(typename T::opid_t i, typename T::tid_t ot, const vector<typename T::tid_t>& it, typename T::flag_t f, type* t, instruction_table<T>* p) : opid(i), mytype(t), out_type(ot), flags(f), parent(p), in_types(it)
   {
   }

   template <class T>
     const typename instruction_table<T>::type::graph_distance_t& instruction_table<T>::operation::get_conversion_graph() const
     {
       (mytype)->distances.calculate_distances();
       return mytype->distances;
     }

   template <class T>
     void instruction_table<T>::add_expansion(typename T::opid_t id, const string& n, const string& t, const vector<opid_t>& a, const string& m, vector<tid_t> types, vector<tid_t> types_out)
     {
       auto& op = add_operation(id, "", types, fEXPANSION);
       op.add_expansion(n, t, a, m, types, types_out);
     }

   template <class T>
     void instruction_table<T>::operation::add_expansion(const string& n, const string& t, const vector<opid_t>& a, const string& m, vector<tid_t> types, vector<tid_t> types_out)
     {
       expansions.push_back(make_expansion({n,t,a,m,types_out}));
       parent->exptab[t][n].push_back(make_expansion({n,t,a,m,types,types_out}));
     }

   template <class T>
     void instruction_table<T>::operation::add_code(int wi, int wo, const string& c, const stringlist& cc, const string& n,const string& t,int r)
     {
       ccode_cont_t ccode_split;
       for(const string& code: cc)
       {
         try
         {
           auto res = split_first(code, ':');
           ccode_split.push_back(make_ccode_pair({res[0], res[1]}));
         }
         RETHROW("in definition of instruction code: "+ ctb::to_string(this->opid) + ". Note that current format of code is '<output writer name>:<code itself>', e.g. 'default:$$name = add($$arg1, $$arg2);");
       }
       versions.push_back(make_instruction({wi > wo ? wi : wo, wi, wo, c,ccode_split,n,t,r,parent->is_tag_satisfactory(t)}));
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
       for(const auto& ins : versions)
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

   /*
      template <class T>
      void instruction_table<T>::operation::imbue_width(int w)const
      {
      imbued_width = w;
      }
      */

   template <class T>
     bool instruction_table<T>::operation::get_type_string(int w, string& c)const
     {
       if(w == -1)
         w = imbued_width;
       for( const auto& t : mytype->versions)
       {
         if ( t.width == w)
         {
           c = t.code;
           return true;
         }
       }
       error( string("type '") + ctb::to_string(mytype->tid) + "' of width = " + to_string(w) + " at operation " + opid + " not found");
       return false;
     }

   template <class T>
     bool instruction_table<T>::operation::get_op_string(int w, string& c, ccode_cont_t& cc, size_t& printability)const
     {
       if(w == -1)
         w = imbued_width;
       bool s = false;
       int r = 0;
       for( auto ins : versions)
       {
         if ( (ins.satisfactory & mSELECT) && ins.width_in == w && ins.rating >= r)
         {
           c = ins.code;
           cc = ins.code_custom;
           r = ins.rating;
           s = true;
           printability = (ins.satisfactory & mPRINT) ? (ins.satisfactory & mONCE) ? 1 : 100000 : 0;
         }
       }
       if(!s)
       {
         warn( string("instruction of width_in = ").append(to_string(w)).append(" at operation ").append(opid).append(" satisfying current tags not found"));
         printability = 0;
       }
       return s;
     }

   template <class T>
     bool instruction_table<T>::operation::get_conv_string(int from, int to, string& c1, string& c2, string& cc, string& t, size_t& printability) const
     {
       bool s = false;
       int r = 0;
       for( auto con : mytype->conversions)
       {
         if ((con.satisfactory & mPRINT) && con.width_in == from && con.width_out == to && con.rating >= r)
         {
           c1 = con.code1;
           c2 = con.code2;
           cc = con.code_custom;
           get_type_string(to, t);
           r = con.rating;
           s = true;
           printability = (con.satisfactory & mPRINT) ? (con.satisfactory & mONCE) ? 1 : 100000 : 0;
         }
       }
       //this error would prevent nontrivial path search from being used
       //if(!s)
       //  error( string("conversion from width ").append(to_string(from)).append(" to ").append(to_string(to)).append(" at operation ").append(opid).append(" not found"));
       return s;
     }

   template <class T>
     const typename instruction_table<T>::operation_t& instruction_table<T>::dec(typename T::opid_t type)   const
     {
       auto itr = instab.find(type);
       if(itr == instab.end())
         error(string("operation ")+type+" not found.");
       return (*itr->second);
     }

   template <class T>
     const typename instruction_table<T>::type_t& instruction_table<T>::dectype(typename T::tid_t type)   const
     {
       auto itr = typetab.find(type);
       if(itr == typetab.end())
         error(string("type ")+type+" not found.");
       return (*itr->second);
     }

   template <class T>
     typename instruction_table<T>::operation_t& instruction_table<T>::add_operation(typename T::opid_t op, typename T::tid_t t, const vector<typename T::tid_t>& it, typename T::flag_t f)
     {
       if(instab.find(op) != instab.end())
       {
         auto& myop = *instab.find(op)->second;
         if(myop.flags != f) 
           error(string("operation '") + op + "' already present with different characteristics! flags " + ctb::to_string(f) + " (new) != " + ctb::to_string(myop.flags) + " (present)" );
         if(!myop.is(fEXPANSION) && myop.out_type != t)
           error(string("operation '") + op + "' already present with different characteristics! out type does not match " + ctb::to_string(t) + " (new) != " + ctb::to_string(myop.out_type) + " (present)");
         if(!myop.is(fEXPANSION) && myop.in_types != it)
           error(string("operation '") + op + "' already present with different characteristics! in type does not match " + ctb::l_to_string(it) + " (new) != " + ctb::l_to_string(myop.in_types) + " (present)");
         return myop;
       }
       auto itr = typetab.find(t);
       if(itr == typetab.end())
         error(string("type '").append(t).append("' not found; at: '").append(op).append("' while constructing graph"));
       operation* ptr = new operation(op, t, it, f, itr->second, this);
       instab[op] = ptr;
       if((f & fDEBUG) > 0)
       {
         if(it.size() != 1)
           error("debug operation has to have exactly one input");
         typetab.find(it[0])->second->debug_op = op;
       }
       return *ptr;
     } 

   template <class T>
     typename instruction_table<T>::type_t& instruction_table<T>::addtype(typename T::tid_t t, int bitwidth)
     {
       if(typetab.find(t) != typetab.end())
       {
         type_t& ret = *typetab.find(t)->second;
         if(bitwidth != 0)
           ret.bitwidth = bitwidth;
         return ret;
       }
       type* ptr = new type(this, bitwidth);
       ptr->tid = t;
       typetab[t] = ptr;
       return *ptr;
     }

   template <class T>
     instruction_table<T>::~instruction_table()  
     {
       clear();
     }

   template <class T>
     void instruction_table<T>::rm_tags(shared_ptr<taghandler_base> ptr, size_t l)
     {
       taglists[l].erase(ptr);
     }

   template <class T>
     const typename instruction_table<T>::expansion_cont_t& instruction_table<T>::find_expansion(const string& transformer, const string& name)
     {
       return exptab[transformer][name];
     }

   template <class T>
     const typename instruction_table<T>::expansion_t& instruction_table<T>::find_expansion(const string& transformer, const string& name, const vector<tid_t>& types)
     {
       const auto& exps = find_expansion(transformer, name);
       for(const auto& exp : exps)
       {
         if(exp.in_types == types)
           return exp;
       }
       writer_plain buff;
       for( const auto& t: types)
         buff.push(t);
       error(string("expansion not found in global expansion table: ") + transformer + ":" + name + ":'" + buff.list_concat(",").write_str() + "'");
       throw "unreachable throw preventing no-return-value warnings";
     }

   template <class T>
     void instruction_table<T>::add_tags(shared_ptr<taghandler_base> ptr, size_t l)
     {
       while(taglists.size() < l + 1)
         taglists.push_back(taghandlerrec_t());
       taglists[l].insert(ptr);
     }

   template <class T>
     typename T::flag_t instruction_table<T>::get_tag_mask(const string& tags)
     {
       typename T::flag_t mask = 0; 
       typename T::flag_t def = mSELECT | mPRINT; 
       for(int i = 0; i < gmCOUNT; ++i)
       {
         bool sat = def & (1<<i);
         if(taglists.size() > i)
         {
           sat = true;
           for(auto th : taglists[i])
             sat &= th->is_satisfactory(tags);
         }
         if(sat)
           mask |= 1 << i;
       }
       return mask;
     }

   template <class T>
     bool instruction_table<T>::is_tag_satisfactory(const string& tags) const
     {
       if(taglists.empty())
         return true;
       bool sat = true;
       for(auto th : taglists[gSELECT])
         sat &= th->is_satisfactory(tags);
       return sat;
     }

   template <class T>
     void instruction_table<T>::update_tags() const
     {
       for(auto i : instab)
         i.second->update_tags();
       for(auto d : typetab)
         d.second->update_tags();
     }

   template <class T>
     void instruction_table<T>::clear(bool tags)
     {
       for(auto d : typetab)
       {
         delete d.second;
       }
       for(auto d : instab)
       {
         delete d.second;
       }
       instab.clear();
       typetab.clear();
       if(tags)
         taglists.clear();
     }

   template class instruction_table<traits> ;

}

#endif

