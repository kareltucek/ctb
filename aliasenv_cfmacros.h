
#ifndef aliasenv_cfmacros_GUARD
#define aliasenv_cfmacros_GUARD

#include "split.h"
#include "aliasenv_maker.h"
#include "defines.h"
#include "writer.h"
#include <map>

namespace ctb
{

  /**
   * This cf serves as a custom macro preprocessor. Reason is that it turns out that C preprocessor is too weak language for this complex macros. Not that it would not be cappable of arithmetics and recurse, but it is a real pain.
   *
    decl("BUFFER_DECL", "capacity, vsize, ingranularity, outgranularity, id, type")

    decl("BUFFER_SIZE", "id")

    decl("BUFFER_FULL", "id")
    decl("BUFFER_FULL_GRANULAR", "id")

    decl("BUFFER_EMPTY_GRANULAR", "id")
    decl("BUFFER_EMPTY", "id")

    decl("BUFFER_PUSH_SIMPLE", "capacity, vsize, id, val")
    decl("BUFFER_PUSH_ONE", "capacity, vsize, id, typeabbrev, val")

    decl("BUFFER_CONSUME_ONE", "id")
    decl("BUFFER_CONSUME_VECTOR", "id")

    decl("BUFFER_PEEK_SIMPLE", "capacity, vsize, id, to")
    decl("BUFFER_PEEK_ONE", "capacity, vsize, id, typeabbrev, to")
    decl("BUFFER_PEEK_VECTOR", "capacity, vsize, id, typeabbrev, to")
   *
   *
   * */

  class aliasenv_cfmacros 
  {
    protected:
      typedef aliasenv_maker<aliasenv_cfmacros, language_empty> aliasenv_subtab;
      static void init();
      static bool interpret(const string&, string&);
      static bool split_call(const string&, string&, stringlist&);
      static bool declargs(const string& args, int args_got, const string& name_got);
    public:
      typedef language_cpp language;
      static string alias(const string& a, bool* s = NULL, int n = -1);
      static string get_name();
      template <class G> static writer<aliasenv_cfmacros> generate(int m,  G& graph, string name, stringlist args) ;
      static void self_test();
  };

  //map<string, string> aliasenv_cfmacros::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  string aliasenv_cfmacros::get_name()
  {
    return "cfmacros";
  }

  bool aliasenv_cfmacros::split_call(const string& call, string& name, stringlist& args)
  {
    int pb = call.find('(');
    int pe = call.find(')');
    if(pb == string::npos || pe == string::npos)
      return false;
    string myname = call.substr(0, pb);
    string list = call.substr(pb+1, pe - pb -1);
    name = trim(myname);
    args = splitlist(trim(list));
    return true;

  }

#define decl(dname, dargs) else if(name == dname && declargs(dargs, args.size(), name))
#define S(a) ++i; aliasenv_subtab::access(a) = string("$")+ctb::to_string(i);
#define TOINT(a) ctb::stoi(writer<aliasenv_subtab>().print(a,args, i ).write_str())
#define GET(a) writer<aliasenv_subtab>().print(a,args, i ).write_str()
#define P(s) w.print(s, args, i); 
#define C() if(args_present != i-1) warning(string("expected ") + ctb::to_string(i-1) + " arguments at " + name + ", but got " + ctb::to_string(args_present));

  bool aliasenv_cfmacros::declargs(const string& str, int args_present, const string& name)
  {
    auto arglist = splitlist(str);
    int i = 0;
    for(const string& arg : arglist) 
    {
      S(arg);
    }
    S("i");
    C();
    return true;
  }


  bool aliasenv_cfmacros::interpret(const string& a, string& ret)
  {
    string name;
    stringlist args;
    if(!split_call(a, name, args))
    {
      //warning(string("aliasenv_cfmacros did not recognize its macro pattern in ") + a );
      return false;
    }

    int i = 0;
    writer<aliasenv_subtab> w;
    aliasenv_subtab::push();

    w.print("/*expanded from '$1'*/", name);
    if(false)
    {
      /*just for the decl macro syntax*/
    }
    decl("BUFFER_DECL", "capacity, vsize, ingranularity, outgranularity, id, type")
    {
      for(int i = 0; i < TOINT("$[$capacity/$vsize]"); ++i)
      {
        P("$type gendecl_${id}_${i};"); 
      }
      P("int gendecl_readat_${id} = 0;"); 
      P("int gendecl_contains_${id} = 0;"); 
      P("static const int gendecl_capacity_${id} = $capacity;"); 
      P("static const int gendecl_outgran_${id} = $outgranularity;"); 
      P("static const int gendecl_ingran_${id} = $ingranularity;"); 
      P("static const int gendecl_vsize_${id} = $vsize;"); 
    }
    decl("BUFFER_DUMP_VECTOR", "capacity, vsize, id")
    {
      P("printf(\"  BUFFER $id, of size %i\\n\", gendecl_contains_${id});"); 
      for(int i = 0; i < TOINT("$[$capacity/$vsize]"); ++i)
      {
        P("DEBUG_SSE( gendecl_${id}_${i}, gendecl_${id}_${i});"); 
        P("if($i == gendecl_readat_${id} / $vsize) printf(\" ->\");");
        P("if($i == ((gendecl_readat_${id} + gendecl_contains_${id}) % gendecl_capacity_${id}) / $vsize) printf(\" <-\");");
        P("printf(\"\\n\");");
      }
    }
    decl("BUFFER_SIZE", "id")
    {
      P("gendecl_contains_${id}"); 
    }
    decl("BUFFER_DUMP", "capacity, id")
    {
      P("printf(\"  BUFFER $id, of size %i\\n\", gendecl_contains_${id});"); 
      for(int i = 0; i < TOINT("$[$capacity]"); ++i)
      {
        P("printf(\"    %i\", gendecl_${id}_${i});"); 
        P("if($i == gendecl_readat_${id}) printf(\" ->\");");
        P("if($i == (gendecl_readat_${id} + gendecl_contains_${id}) % gendecl_capacity_${id}) printf(\" <-\");");
        P("printf(\"\\n\");");
      }
    }
    decl("BUFFER_FULL", "id")
    {
      P(" (gendecl_contains_${id} == gendecl_capacity_${id}) "); 
    }
    decl("BUFFER_FULL_GRANULAR", "id")
    {
      P("(gendecl_capacity_${id} - gendecl_ingran_${id} < gendecl_contains_${id} )"); 
    }
    decl("BUFFER_EMPTY_GRANULAR", "id")
    {
      P(" (gendecl_contains_${id} < gendecl_outgran_${id}) "); 
    }
    decl("BUFFER_EMPTY", "id")
    {
      P(" (gendecl_contains_${id} == 0) "); 
    }
    decl("BUFFER_PUSH_SIMPLE", "capacity, vsize, id, val")
    {
      P("switch((gendecl_readat_${id} + gendecl_contains_${id}) % ${capacity}){ "); 
      for (auto i = 0; i < TOINT("$capacity"); i++) 
        P("case $i: gendecl_$id_$i = $val; break;");
      P("};"); 
      P("gendecl_contains_$id += $vsize;"); 
    }
    decl("BUFFER_PUSH_ONE", "capacity, vsize, id, typeabbrev, val")
    {
      P("switch((gendecl_readat_${id} + gendecl_contains_${id}) % ${capacity}){ "); 
      for (auto i = 0; i < TOINT("$capacity"); i++) 
        P("case $i: gendecl_$id_$[$i/$vsize] = _mm_insert_$typeabbrev((__m128i)gendecl_$id_$[$i / $vsize], $val, $i % gendecl_vsize_$id); break;");
      P("};"); 
      P("gendecl_contains_$id++;"); 
    }
    decl("BUFFER_PEEK_SIMPLE", "capacity, vsize, id, to")
    {
      P("switch(gendecl_readat_$id){ ");
      for (auto i = 0; i < TOINT("$capacity"); i++) 
        P("case $i: $to = gendecl_$id_$[$i / $vsize]; break;");
      P("}");
    }
    decl("BUFFER_PEEK_ONE", "capacity, vsize, id, typeabbrev, to")
    {
      P("switch(gendecl_readat_$id){ ");
      for (auto i = 0; i < TOINT("$capacity"); i++) 
        P("case $i: $to = _mm_extract_$typeabbrev((__m128i)gendecl_$id_$[$i / $vsize], $i % gendecl_vsize_$id); break;");
      P("}");
    }
    decl("BUFFER_PEEK_VECTOR", "capacity, vsize, id, typeabbrev, to")
    {
      P("switch(gendecl_readat_$id){ ");
      for (auto i = 0; i < TOINT("$capacity"); i++) 
        P("case $i: $to = _mm_alignr_epi8(gendecl_$id_$[(1 + ($i/$vsize)) % ($capacity/$vsize)], gendecl_$id_$[$i / $vsize], ($i%gendecl_vsize_$id)*(16/gendecl_vsize_$id)); break;");
      P("}");
    }
    decl("BUFFER_CONSUME_VECTOR", "id")
    {
      P("gendecl_readat_$id = ((gendecl_readat_$id + gendecl_vsize_$id) % gendecl_capacity_$id);");
      P("gendecl_contains_$id -= gendecl_vsize_$id;");
    }
    decl("BUFFER_CONSUME_ONE", "id")
    {
      P("gendecl_readat_$id = ((gendecl_readat_$id + 1) % gendecl_capacity_$id);");
      P("gendecl_contains_$id--;");
    }
    else
    {
      warning(string("aliasenv_cfmacros did not match string: ") + a + " with name parsed as " + name);
      aliasenv_subtab::pop();
      return false;
    }
    aliasenv_subtab::pop();

    ret = w.write_str();
    return true;
  }
#undef S
#undef P
#undef C
#undef TOINT
#undef ARGS

  string aliasenv_cfmacros::alias(const string& a, bool* s, int n)
  {
    string ret;
    bool succ;
    try
    {
      succ = interpret(a, ret);
    }
    RETHROW(string("in aliasenv_cfmacros while interpretting ")+a);
    if(!succ)
    {
      if(s != NULL)
        *s = false;
      else
        warn(string("warning: alias not found: ").append(a));
      return "";
    }
    if(s != NULL)
      *s = true;
    return ret;
  }


  void aliasenv_cfmacros::init()
  {
    static bool initialized = false;
    if(initialized)
      return;


    initialized = true;
  }

  template <class G>
    writer<aliasenv_cfmacros> aliasenv_cfmacros::generate(int max_granularity, G& graph, string name, stringlist args)
    {
      error( "empty aliasenv generator used");
      return writer<aliasenv_cfmacros>();
    }

  void aliasenv_cfmacros::self_test()
  {
    writer<aliasenv_cfmacros> w;
    w.print("${BUFFER_EMPTY_GRANULAR(myid)}");
    //assert(w.write_str() == " (gendecl_contains_myid < gendecl_outgran_myid) "); 
  }

};

#endif

