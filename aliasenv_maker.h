
#ifndef aliasenv_GUARD
#define aliasenv_GUARD

#include "languages.h"
#include "writer.h"
#include <map>

namespace ctb
{

  /**
   * For general documentation see aliasenv_generator.h.
   *
   * This is a generalization of the aliasenv_generator class. aliasenv maker produces dynamicly constructable aliasenv, which can be used for creating a general-purpose writers with custom alias tables.
   *
   * - T = tag (needed for distinguishing *static* alias tables)
   * - L = language
   * - R = report when not found (when handling strings on meta level we may want misses
   * */


  template < typename T, typename L, bool R = true>
    class aliasenv_maker
    {
      protected:
        typedef std::map<std::string, std::string> aliastab_t;
        static aliastab_t aliases;
        static void init();
      public:
        typedef aliasenv_maker<T,L,false> noreport;
        typedef L language;
        static std::string alias(const std::string& a, bool* success = NULL);
        static void clear();
        static std::string& access(const std::string&);
    };

  struct empty_tag{};

  template <typename T, typename L, bool R> std::map<std::string, std::string> aliasenv_maker<T,L,R>::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  template <typename T, typename L, bool R>
    std::string aliasenv_maker<T,L,R>::alias(const std::string& a, bool* s)
    {
      auto itr = aliases.find(a);
      if(itr == aliases.end())
      {
        if(s != NULL)
          *s = false;
        else if(R)
          warn( std::string("warning: alias not found: ").append(a));
        return "";
      }
      if(s != NULL)
        *s = true;
      return itr->second;
    }

  template <typename T, typename L, bool R>
    void aliasenv_maker<T,L,R>::init()
    {

    }

  template <typename T, typename L, bool R>
    void aliasenv_maker<T,L,R>::clear()
    {
      aliases.clear();
    }

  template <typename T, typename L, bool R>
    std::string& aliasenv_maker<T,L,R>::access(const std::string& str)
    {
      return aliases[str];
    }


};

#endif

