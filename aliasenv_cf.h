
#ifndef ALIASENV_CF_GUARD
#define ALIASENV_CF_GUARD

#include "split.h"
#include "aliasenv_maker.h"
#include "defines.h"
#include "writer.h"
#include <map>

namespace ctb
{

  /**
   * This cf serves as a custom macro preprocessor. Reason is that it turns out that C preprocessor is too weak language for this complex macros. Not that it would not be cappable of arithmetics and recurse, but it is a real pain.
   * */

  class aliasenv_cf 
  {
    protected:
      static void init();
      static bool interpret(const string&, string&);
      static bool split_call(const string&, string&, stringlist&);
    public:
      typedef language_cpp language;
      static string alias(const string& a, bool* s = NULL, int n = -1);
      static string get_name();
      template <class G> static writer<aliasenv_cf> generate(int m,  G& graph, string name) ;
      static void self_test();
  };

  //map<string, string> aliasenv_cf::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  string aliasenv_cf::get_name()
  {
    return "cfmacros";
  }

  bool aliasenv_cf::split_call(const string& call, string& name, stringlist& args)
  {
    int pb = call.find('(');
    int pe = call.find(')');
    if(pb == string::npos || pe == string::npos)
      return false;
    string myname = call.substr(0, pb);
    string list = call.substr(pb+1, pe - pb -1);
    name = trim(myname);
    args = split(trim(list), ',');
    return true;

  }
  
#define S(a) ++i; aliasenv_subtab::access(a) = string("$")+ctb::to_string(i);
#define TOINT(a) ctb::stoi(writer<aliasenv_subtab>().print(a,args, i ).write_str())
#define P(s) w.print(s, args, i); 
#define C() if(args.size() != i-1) warning(string("expected ") + ctb::to_string(i-1) + " arguments at " + name + ", but got " + ctb::to_string(args.size()));
  bool aliasenv_cf::interpret(const string& a, string& ret)
  {
    string name;
    stringlist args;
    if(!split_call(a, name, args))
    {
      warning(string("aliasenv_cf did recognize its macro pattern in ") + a );
      return false;
    }

    typedef aliasenv_maker<aliasenv_cf, language_empty> aliasenv_subtab;
    int i = 0;
    writer<aliasenv_subtab> w;
    aliasenv_subtab::push();

    w.print("/*expanded from '$1'*/", name);
    if(name == "BUFFER_DECL")
    {
      S("size") S("vsize") S("ingranularity") S("outgranularity") S("id") S("type") S("i") C();
      for(int i = 0; i < TOINT("$[$size/$vsize]"); ++i)
        P("$type gendecl_${id}_${i};"); 
      P("int gendecl_readat_${id} = 0;"); 
      P("int gendecl_contains_${id} = 0;"); 
      P("int gendecl_size_${id} = $size;"); 
      P("static const int gendecl_outgran_${id} = $outgranularity;"); 
      P("static const int gendecl_ingran_${id} = $ingranularity;"); 
      P("static const int gendecl_vsize_${id} = $vsize;"); 
    }
    else if(name == "BUFFER_FULL_GRANULAR")
    {
      S("id") S("i") C();
      P("(gendecl_size_${id} - gendecl_ingran_${id} < gendecl_contains_${id} )"); 
    }
    else if(name == "BUFFER_EMPTY_GRANULAR")
    {
      S("id") S("i") C();
      P(" (gendecl_contains_${id} < gendecl_outgran_${id}) "); 
    }
    else if(name == "BUFFER_EMPTY")
    {
      S("id") S("i") C();
      P(" (gendecl_contains_${id} == 0) "); 
    }
    else if(name == "BUFFER_FULL")
    {
      S("id") S("i") C();
      P(" (gendecl_contains_${id} == gendecl_size_${id}) "); 
    }
    else if(name == "BUFFER_PUSH_SIMPLE")
    {
      S("size") S("vsize") S("id") S("val") S("i") C();
      P("switch((gendecl_readat_${id} + gendecl_contains_${id}) % ${size}){ "); 
      for (auto i = 0; i < TOINT("$size"); i++) 
        P("case $i: gendecl_$id_$i = $val; break;");
      P("};"); 
      P("gendecl_contains_$id += $vsize;"); 
    }
    else if(name == "BUFFER_PUSH_ONE")
    {
      S("size") S("vsize") S("id") S("typeabbrev") S("val") S("i") C();
      P("switch((gendecl_readat_${id} + gendecl_contains_${id}) % ${size}){ "); 
      for (auto i = 0; i < TOINT("$size"); i++) 
        P("case $i: gendecl_$id_$[$i/$vsize] = _mm_insert_$typeabbrev(gendecl_$id_$[$i / $vsize], $val, $i % gendecl_vsize_$id); break;");
      P("};"); 
      P("gendecl_contains_$id++;"); 
    }
    else if(name == "BUFFER_CONSUME_ONE")
    {
      S("id") S("i") C();
      P("gendecl_readat_$id = ((gendecl_readat_$id + 1) % gendecl_size_$id);");
      P("gendecl_contains_$id--;");
    }
    else if(name == "BUFFER_PEEK_SIMPLE")
    {
      S("size") S("vsize") S("id") S("to") S("i") C();
      P("switch(gendecl_readat_$id){ ");
      for (auto i = 0; i < TOINT("$size"); i++) 
        P("case $i: $to = gendecl_$id_$[$i / $vsize]; break;");
      P("}");
    }
    else if(name == "BUFFER_PEEK_ONE")
    {
      S("size") S("vsize") S("id") S("typeabbrev") S("to") S("i") C();
      P("switch(gendecl_readat_$id){ ");
      for (auto i = 0; i < TOINT("$size"); i++) 
        P("case $i: $to = _mm_extract_$typeabbrev(gendecl_$id_$[$i / $vsize], $i % gendecl_vsize_$id); break;");
      P("}");
    }
    else if(name == "BUFFER_CONSUME_VECTOR")
    {
       S("id") S("i") C();
       P("gendecl_readat_$id = ((gendecl_readat_$id + gendecl_vsize_$id) % gendecl_size_$id);");
       P("gendecl_contains_$id -= gendecl_vsize_$id;");
    }
    else if(name == "BUFFER_PEEK_VECTOR")
    {
      S("size") S("vsize") S("id") S("typeabbrev") S("to") S("i") C();
      P("switch(gendecl_readat_$id){ ");
      for (auto i = 0; i < TOINT("$size"); i++) 
        P("case $i: $to = _mm_alignr_epi8(gendecl_$id_$[(1 + ($i/$vsize)) % ($size/$vsize)], gendecl_$id_$[$i / $vsize], ($i%gendecl_vsize_$id)*(16/gendecl_vsize_$id)); break;");
      P("}");
    }
    else
    {
      warning(string("aliasenv_cf did not match string: ") + a + " with name parsed as " + name);
      aliasenv_subtab::pop();
      return false;
    }
    aliasenv_subtab::pop();

    ret = w.write_str();
    return true;
  }
#undef S
#undef P
#undef TOINT
  
  string aliasenv_cf::alias(const string& a, bool* s, int n)
  {
    string ret;
    bool succ;
    try
    {
      succ = interpret(a, ret);
    }
    RETHROW(string("in aliasenv_cf while interpretting ")+a);
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


  void aliasenv_cf::init()
  {
    static bool initialized = false;
    if(initialized)
      return;


    initialized = true;
  }

  template <class G>
    writer<aliasenv_cf> aliasenv_cf::generate(int max_granularity, G& graph, string name)
    {
      error( "empty aliasenv generator used");
      return writer<aliasenv_cf>();
    }

    void aliasenv_cf::self_test()
    {
      writer<aliasenv_cf> w;
      w.print("${BUFFER_EMPTY_GRANULAR(myid)}");
      //assert(w.write_str() == " (gendecl_contains_myid < gendecl_outgran_myid) "); 
    }

};

#endif

