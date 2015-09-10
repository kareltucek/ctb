
#ifndef aliasenv_GENERATOR_GUARD
#define aliasenv_GENERATOR_GUARD

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
      typedef std::map<std::string, std::string> aliastab_t;
      static aliastab_t aliases;
      static void init();
      static bool check_pos_alias(const std::string& name, const std::string& a, int offset);
    public:
      typedef language_empty language;
      static std::string alias(const std::string& a, bool* s = NULL, int n = -1);
      template <class G> static writer<aliasenv_generator> generate(int m,  G& graph, std::string name) ;
  };

  std::map<std::string, std::string> aliasenv_generator::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  bool aliasenv_generator::check_pos_alias(const std::string& name, const std::string& a, int offset)
  {
    bool aredigits = true;
    for(int i = name.length(); i < a.length(); ++i)
      aredigits &= isdigit(a[i]);
    if(aredigits && a.length() > name.length() && a.substr(0,name.length()) == name) // >3 ensures at least one digit to exist
    {
      ADD(a, std::string("$").append(std::to_string(offset+std::stoi(a.substr(name.length(),a.length()-name.length())))));
      return true;
    }
    return false;
  }


  std::string aliasenv_generator::alias(const std::string& a, bool* s, int n)
  {
    auto itr = aliases.find(a);
    if(itr == aliases.end())
    {
      if(check_pos_alias("arg", a, 7))
        return alias(a,s,n);
      if(check_pos_alias("name", a, 8))
        return alias(a,s,n);

      if(s != NULL)
        *s = false;
      else
        warn(std::string("warning: alias not found: ").append(a));
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
    ADD("innercode", "$type $name = $operation;");
    ADD("outputcode", "$operation;");
    ADD("inputcode", "$type $name = $operation;");
    ADD("conversioncode", "$type $name = $operation;");

    ADD("type", "$1");
    ADD("name", "$2");
    ADD("operation", "$3");

    ADD("cindex", "$4");
    ADD("iindex", "$5");
    ADD("oindex", "$6");
    ADD("vindex", "$7");
    ADD("arg1", "$8");
    ADD("arg2", "$9");
    ADD("arg3", "$10");


    initialized = true;
  }

  template <class G>
    writer<aliasenv_generator> aliasenv_generator::generate(int max_granularity, G& graph, std::string name)
    {
      error( "empty aliasenv generator used");
      return writer<aliasenv_generator>();
    }

};

#endif

