#ifndef DATATYPES_GUARD
#define DATATYPES_GUARD

#include <string>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <sstream>

namespace ctb
{
  /** Datatypes contains various general helper functions and datatypes*/

  //various helper classes
  struct traits
  {
    typedef std::string opid_t;
    typedef std::string tid_t;
    typedef std::string vid_t;
    typedef int flag_t;
    typedef int param_t;
    static const int maxarity = 3;
  } ;

  enum flags 
  {
    fINPUT = 1, fOUTPUT = 2
  } ;

  template <typename ... T> void pass(T...)
  {
  }

  typedef std::vector<std::string> stringlist;

  template <typename T>
    struct friend_maker
    {
      typedef T type;
    };

  struct dummy_friend
  {
  };


  struct dummy
  {
    template<typename...D>
      dummy(D...t){};
  };

  bool fileexists(char *name)
  {
    struct stat   buffer;
    return (stat (name, &buffer) == 0);
  }

  std::vector<std::string> split(std::string str, char d, bool squash = false)
  {
    std::string item;
    std::vector<std::string> l;
    std::istringstream ss(str);
    while(std::getline(ss, item, d))
    {
      int end = item.find_last_not_of(" \n\r");
      int begin = item.find_first_not_of(" \n\r");
      if(begin == -1)
      {
        if(!squash)
          l.push_back("");
      }
      else
        l.push_back(item.substr(begin, end-begin+1));
    }
    return l;
  }

  template<typename F>
    F string_to_flags(std::string str)
    {
      static std::map<std::string, F> hash = {{"output",fOUTPUT},{"input",fINPUT}};
      F f = 0;
      stringlist words = split(str, ',');
      for(auto w : words)
      {
        if(hash.find(w) != hash.end() )
        {
          f |= hash[w];
        }
        else
        {
          throw std::string("unknown flag found: ").append(w);
        }
      }
      return f;
    }

  template<typename F>
    std::string flags_to_string(F f)
    {
      std::string b;
      static std::map<std::string, F> hash = {{"output",fOUTPUT},{"input",fINPUT}};

      for(auto rec : hash)
      {
        if( (rec.second & f) > 0)
        {
          if(!b.empty())
            b.append(",");
          b.append(rec.first);
        }
      }
      return b;
    }


  int stoi(std::string str)
  {
    //will want some evaluation here later...
    return std::stoi(str);
  }


  /** 
   * Proxy class provides a more or less conceptually correct implementation of public-read-only class fields. Originaly its syntax was supposed to be absolutely transparent. Unfortunately it ended up not being transparent at all.
   * */
  template <class T, class A = dummy_friend, class B = dummy_friend, class C = dummy_friend,  class D = dummy_friend>
    class proxy_ {
      friend typename friend_maker<A>::type;
      friend typename friend_maker<B>::type;
      friend typename friend_maker<C>::type;
      friend typename friend_maker<D>::type;
      private:
      //T operator=(const T& arg) { data = arg; return data; }
      T data;
      typedef typename std::remove_reference<T>::type result_t;

      template <typename U> struct br_ct  { typedef decltype(std::declval<T const>()[std::declval<U>()]) type; };
      template <typename U> struct br_t { typedef decltype(std::declval<T      >()[std::declval<U>()]) type; };

      result_t& rw() { return data; }
      //operator T&() { return data; }
      //T& operator*() { return data; }
      //result_t* operator->() { return &data; }
      //template <typename U> typename br_t<U>::type operator [](U args) { return data[args]; }
      public:
      const result_t& r() const { return data; }
      operator const T&() const { return data; }
      const T& operator*() const { return data; }
      const result_t* operator->() const { return &data; }
      template <typename U> typename br_ct<U>::type operator [](U args) const { return data[args]; }
      template <typename...L> proxy_(L&&... args) : data((std::forward<L>(args))...){}
    };

  enum functor_id{fidli = 0, fidlg = 1, fidei = 2, fideg = 3, fidg=4};
  static std::map<std::string, functor_id> cmd_id_hash = {{"loadinstab", fidli}, {"loadgraph",fidlg}, {"exportinstab",fidei},{"exportgraph",fideg},{"generate",fidg}};

};

#endif 

