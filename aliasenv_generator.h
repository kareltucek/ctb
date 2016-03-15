
#ifndef aliasenv_GENERATOR_GUARD
#define aliasenv_GENERATOR_GUARD

#include "defines.h"
#include "writer.h"
#include <map>

namespace ctb
{

  /**
   * Alias environments
   * ------------------
   * aliasenv represents an output environment. It provides a means of separating muptiple layers of code generation abstraction. 
   *
   * First thing an aliasenv generally provides is an alias mechanism, which serves as a plugin to the writer class (which takes care of composing all strings together). 
   *
   * Second thing defined in an aliasenv is a generation method. This one is supposed to direct the topmost level of code generation, which is taking care of composition of code templates. It is provided an instance of a generator for generation of the actual code realization.
   *
   * More generally aliasenv defines only some preprocessor-like nevironment which may be used for import and export actions. See usages of aliasenv_maker.
   * 
   * Aliasenv_generator
   * ------------------
   * This concrete class describes internal workings of the instruction generator.
   *
   * This one should be an ancestor of every aliasenv used for code generation since it defines an abstract wrapper around the internal implementation.
   * */

  class aliasenv_generator 
  {
    protected:
      typedef map<string, string> aliastab_t;
      static map<string,string> const * params;
      static aliastab_t aliases;
      static void init();
      static bool check_pos_alias(const string& name, const string& a, int offset);
    public:
      typedef language_empty language;
      static string alias(const string& a, bool* s = NULL, int n = -1);
      static void setparammap(map<string,string>const * map);
      template <class G> static writer<aliasenv_generator> generate(int m,  G& graph, string name) ;
  };

  map<string, string> aliasenv_generator::aliases;
  map<string, string> const* aliasenv_generator::params = NULL;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  void aliasenv_generator::setparammap(map<string,string>const* map)
  {
    aliasenv_generator::params = map;
  }

  bool aliasenv_generator::check_pos_alias(const string& name, const string& a, int offset)
  {
    bool aredigits = true;
    for(int i = name.length(); i < a.length(); ++i)
      aredigits &= isdigit(a[i]);
    if(aredigits && a.length() > name.length() && a.substr(0,name.length()) == name) // >3 ensures at least one digit to exist
    {
      ADD(a, string("$").append(ctb::to_string(offset+ctb::stoi(a.substr(name.length(),a.length()-name.length())))));
      return true;
    }
    return false;
  }


  string aliasenv_generator::alias(const string& a, bool* s, int n)
  {
    if(params == NULL)
      warning("aliasenv_generator was invoked without specification of params. You are not supposed to use the generating aliasenv on its own.");
    auto itr = aliases.find(a);
    if(itr == aliases.end())
    {
      if(params != NULL)
      {
        auto itr2 = params->find(a);
        if(itr2 != params->end())
        {
          if(s!= NULL)
            *s = true;
          return itr2->second;
        }
      }
      //if(check_pos_alias("arg", a, 7))
      //  return alias(a,s,n);
      //if(check_pos_alias("name", a, 8))
      //  return alias(a,s,n);

      if(s != NULL)
        *s = false;
      else
        warn(string("warning: alias not found: ").append(a));
      return "";
    }
    if(s != NULL)
      *s = true;
    return itr->second;
  }


  void aliasenv_generator::init()
  {
    static bool initialized = false;
    if(initialized)
      return;

    //these are the basic defines for internal code generation i.e. obligatory ones
    //note that you may need to customize this first paragraph depending on the output language ==> you can override them from your own aliasenv easily
    ADD("innercode", "$type $name = ($type)($operation);");
    ADD("declcode", "$type $name;");
    ADD("outputcode", "$operation /*$name*/;"); //commented name serves for determining opcode for debug
    ADD("inputcode", "$type $name = ($type)($operation);");
    ADD("conversioncode", "$type $name = ($type)($operation);");

    ADD("type", "$1");
    ADD("name", "$2");
    ADD("basename", "$3");
    ADD("operation", "$4");

    ADD("iindex", "$5");
    ADD("oindex", "$6");
    ADD("vindex", "$7");
    ADD("classid", "$8");
    ADD("arg1", "$9");
    ADD("arg2", "$10");
    ADD("arg3", "$11");


    initialized = true;
  }

  template <class G>
    writer<aliasenv_generator> aliasenv_generator::generate(int max_granularity, G& graph, string name)
    {
      error( "empty aliasenv generator used");
      return writer<aliasenv_generator>();
    }

};

#endif

