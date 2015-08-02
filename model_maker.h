
#ifndef MODEL_GUARD
#define MODEL_GUARD

#include "languages.h"
#include "writer.h"
#include <map>

namespace ctb
{

  /**
   * This is a generalization of the model_generator class. Model maker produces dynamicly constructable model, which can be used for creating a general-purpose writers with custom alias tables.
   * */


  template < typename T, typename L>
    class model_maker
    {
      protected:
        typedef std::map<std::string, std::string> aliastab_t;
        static aliastab_t aliases;
        static void init();
      public:
        typedef L language;
        static std::string alias(const std::string& a);
        static void clear();
        static std::string& access(const std::string&);
    };

  struct empty_tag{};
  typedef model_maker<empty_tag, language_empty> model_empty;
  typedef model_maker<empty_tag, language_cpp> model_empty_cpp;
  typedef writer<model_empty_cpp> writer_default;
  typedef writer<model_empty> writer_plain;

  template <typename T, typename L> std::map<std::string, std::string> model_maker<T,L>::aliases;

  template<bool dolars = false, typename ... Types> std::string print (const Types&... params)
  {
    return writer<model_empty>().print(params...).write_str();
  }

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  template <typename T, typename L>
    std::string model_maker<T,L>::alias(const std::string& a)
    {
      auto itr = aliases.find(a);
      if(itr == aliases.end())
        throw std::string("alias not found: ").append(a);
      return itr->second;
    }

  template <typename T, typename L>
    void model_maker<T,L>::init()
    {

    }

  template <typename T, typename L>
    void model_maker<T,L>::clear()
    {
      aliases.clear();
    }

  template <typename T, typename L>
    std::string& model_maker<T,L>::access(const std::string& str)
    {
      return aliases[str];
    }


};

#endif

