
#ifndef MODEL_GUARD
#define MODEL_GUARD

#include "writer.h"
#include <map>

class model_empty
{
  protected:
    typedef std::map<std::string, std::string> aliastab_t;
    static aliastab_t aliases;
    static void init();
  public:
    static std::string alias(const std::string& a);
    template <class G> static writer<model_empty> generate(int m,  G& graph, std::string name) ;
};

template<bool dolars = false, typename ... Types> std::string print (const Types&... params)
{
  return writer<model_empty>().print(params...).write_str();
}

std::map<std::string, std::string> model_empty::aliases;

typedef writer<model_empty> writer_default;
typedef writer<model_empty> writer_default;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))
std::string model_empty::alias(const std::string& a)
{
  auto itr = aliases.find(a);
  if(itr == aliases.end())
    throw std::string("alias not found: ").append(a);
  return itr->second;
}


void model_empty::init()
{
  static bool initialized = false;
  if(initialized)
    return;

  //these are the basic defines for internal code generation i.e. obligatory ones

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
writer<model_empty> model_empty::generate(int max_granularity, G& graph, std::string name)
{
  throw "empty model used";
}


#endif

