
#ifndef aliasenv_GUARD
#define aliasenv_GUARD

#include "defines.h"
#include "languages.h"
#include "writer.h"
#include <map>

namespace ctb
{

  /**
   * For general documentation see aliasenv_generator.h.
   *
   * This is a generalization of the aliasenv_generator class. aliasenv maker produces dynamicly constructable aliasenv, which can be used for creating general-purpose writers with custom alias tables.
   *
   * - T = tag (needed for distinguishing *static* alias tables)
   * - L = language
   * - R = report when not found (when handling strings on meta level we may want misses
   * */


  template < typename T, typename L, bool R = true>
    class aliasenv_maker
    {
      protected:
        typedef map<string, string> aliastab_t;
        static aliastab_t aliases;
        static void init();
      public:
        typedef aliasenv_maker<T,L,false> noreport;
        typedef L language;
        static string alias(const string& a, bool* success = NULL);
        static void clear();
        static string& access(const string&);
    };

  struct empty_tag{};

  template <typename T, typename L, bool R> map<string, string> aliasenv_maker<T,L,R>::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  template <typename T, typename L, bool R>
    string aliasenv_maker<T,L,R>::alias(const string& a, bool* s)
    {
      auto itr = aliases.find(a);
      if(itr == aliases.end())
      {
        if(s != NULL)
          *s = false;
        else if(R)
          warn( string("warning: alias not found: ").append(a));
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
    string& aliasenv_maker<T,L,R>::access(const string& str)
    {
      return aliases[str];
    }


};

#endif

