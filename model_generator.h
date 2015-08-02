
#ifndef MODEL_GENERATOR_GUARD
#define MODEL_GENERATOR_GUARD

#include "writer.h"
#include <map>

namespace ctb
{

  /**
   * Model represents an output environment. It provides a means of separating muptiple layers of code generation abstraction. This class should be an ancestor of every model since it defines an abstract wrapper around the internal implementation.
   *
   * First thing it provides is an alias mechanism, which serves as a plugin to the writer class (which takes care of composing all strings together). 
   *
   * Second thing defined in the model is a generation method. This one is supposed to direct the topmost level of code generation, which is taking care of composition of code templates. It is provided an instance of a generator for generation of the actual code realization.
   * */

  class model_generator 
  {
    protected:
      typedef std::map<std::string, std::string> aliastab_t;
      static aliastab_t aliases;
      static void init();
    public:
      typedef language_empty language;
      static std::string alias(const std::string& a);
      template <class G> static writer<model_generator> generate(int m,  G& graph, std::string name) ;
  };

  std::map<std::string, std::string> model_generator::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))
  std::string model_generator::alias(const std::string& a)
  {
    auto itr = aliases.find(a);
    if(itr == aliases.end())
      throw std::string("alias not found: ").append(a);
    return itr->second;
  }


  void model_generator::init()
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
    ADD("arg1", "$5");
    ADD("arg2", "$6");
    ADD("arg3", "$7");

    ADD("vindex", "$1");

    initialized = true;
  }

  template <class G>
    writer<model_generator> model_generator::generate(int max_granularity, G& graph, std::string name)
    {
      throw "empty model generator used";
    }

};

#endif

