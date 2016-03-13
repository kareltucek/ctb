
#ifndef aliasenv_GUARD
#define aliasenv_GUARD

#include "defines.h"
#include "languages.h"
#include "writer.h"
#include <map>
#include <stack>
#include "aliasenv_empty.h"

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

  template < typename T, typename L, bool R = true, typename F = aliasenv_empty>
    class aliasenv_maker
    {
      protected:
        T tag;
        typedef map<string, string> aliastab_t;
        static aliastab_t& aliases(); 
        static stack<aliastab_t> my_aliases;
        static void init();
      public:
        typedef aliasenv_maker<T,L,false,F> noreport;
        typedef L language;
        static string alias(const string& a, bool* success = NULL);
        static void clear();
        static string& access(const string&);
        static string get_name();
        static string get_child_name();
        static void push();
        static void pop();
    };



  struct empty_tag{};

  template <typename T, typename L, bool R, typename F> stack<map<string, string>> aliasenv_maker<T,L,R,F>::my_aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  template <typename T, typename L, bool R, typename F>
    string aliasenv_maker<T,L,R,F>::get_child_name()
    {
      return F::get_name();
    }

  template <typename T, typename L, bool R, typename F>
    string aliasenv_maker<T,L,R,F>::get_name()
    {
      return "maker";
    }

  template <typename T, typename L, bool R, typename F>
    map<string, string>& aliasenv_maker<T,L,R,F>::aliases()
    {
      if(my_aliases.empty())
        my_aliases.push(aliastab_t());
      return my_aliases.top();
    }

  template <typename T, typename L, bool R, typename F>
    void aliasenv_maker<T,L,R,F>::pop()
    {
      my_aliases.pop();
    }

  template <typename T, typename L, bool R, typename F>
    void aliasenv_maker<T,L,R,F>::push()
    {
      my_aliases.push(aliastab_t());
    }


  template <typename T, typename L, bool R, typename F>
    string aliasenv_maker<T,L,R,F>::alias(const string& a, bool* s)
    {
      auto itr = aliases().find(a);
      if(itr == aliases().end())
      {
        bool mysuc;
        string res = F::alias(a, &mysuc);
        if(mysuc)
        {
          if(s != NULL)
            *s = true;
          return res;
        }

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

  template <typename T, typename L, bool R, typename F>
    void aliasenv_maker<T,L,R,F>::init()
    {

    }

  template <typename T, typename L, bool R, typename F>
    void aliasenv_maker<T,L,R,F>::clear()
    {
      aliases().clear();
    }

  template <typename T, typename L, bool R, typename F>
    string& aliasenv_maker<T,L,R,F>::access(const string& str)
    {
      return aliases()[str];
    }


};

#endif

