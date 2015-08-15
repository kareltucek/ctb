#ifndef TAGMASTER_GUARD
#define TAGMASTER_GUARD

#include <map>
#include "datatypes.h"

namespace ctb
{
  /** Tagmaster serves for managin tags or more generally flags. Intuitive way of dealing with tags/flags requires them to be encoded as a comma separated list, but for machine processing a binary mask is much more suitable.
   *
   * The tagmaster is written (mainly) for the purpose of keeping track of tags in the meaning of different versions of instructions on different hardware. For this purpose it is not clear how should tag requirements be defined. Thus tagmaster is a general structure which is passed as a part of traits. Instance of this class is meant to be constructed on the operator's side and passed to instruction table. which will use its method to determine whether an instruction is satisfactory or not.
   *
   * This structure thus contains:
   *   Method which can respond to series of tags saying whether it is satisfactory or not.
   *   Data on which the satisfactionability depends (these may be arbitrary)
   *   A system which can generically translate tags to masks.
   *
   * Type F should be initializable by 0 to an empty mask, by 1 to a minimal mask and should support << & | ! == and != operators.
   *
   * Since the mask mappings are time-order dependent, we need a unique instance tag map. Since there is possibility of needing multiple (different) instances at the same time, we cant afford static table.
   * */

  template<typename F>
    class tagmaster
    {
      private:
        F required;
        F allowed;
        F excluded;
        F at;
        F full;
        std::map<std::string,F> tags_used;
      public:
        tagmaster(const std::string& r = "",const std::string& a = "",const std::string& e = "");
        template<typename T> tagmaster(T&&);
        template<typename T> tagmaster& operator=(T&&);
        bool is_satisfactory(const std::string&);
        F to_mask(const std::string&);
        void clear();

        static void self_test();
    };

  typedef tagmaster<uint32_t> tagmaster_default;

    template<typename F>
  void tagmaster<F>::clear()
  {
    required = F(0);
    allowed = F(0);
    excluded = F(0);
    at = F(1);
    full = F(0);
    tags_used.clear();
  }

  template<typename F>
    tagmaster<F>::tagmaster(const std::string& r,const std::string& a,const std::string& e) : at(1), full(0), tags_used()
  {
      required=to_mask(r);
      allowed=to_mask(a);
      excluded=to_mask(e);
  }

  template<typename F>
    template<typename T>
    tagmaster<F>::tagmaster(T&& t) : required(t.required), allowed(t.allowed), excluded(t.excluded), at(t.at), full(t.full), tags_used(std::forward(t.tags_used))
    {
    }

  template<typename F>
    template<typename T>
    tagmaster<F>& tagmaster<F>::operator=(T&& t)
    {
      required = std::forward(t).required;
      allowed =  std::forward(t).allowed;
      excluded = std::forward(t).excluded;
      at =       std::forward(t).at;
      full =     std::forward(t).full;
      tags_used = std::forward(t).tags_used;
    }

  template<typename F>
    F tagmaster<F>::to_mask(const std::string& str)
    {
      F mask = F(0); 
      stringlist tags = split(str,',');
      for(const std::string& t : tags) 
      {
        if(t == "")
          continue;
        auto itr = tags_used.find(t);
        if(itr == tags_used.end())
        {
          tags_used[t] = at;
          full |= at;
          mask |= at;
          at = at << 1;
          if(at == 0)
            error("mask overflow", true);
        }
        else
        {
          mask |= itr->second;
        }
      }
      return mask;
    }

  template<typename F>
    bool tagmaster<F>::is_satisfactory(const std::string& str)
    {
      F mask = to_mask(str);
      return ((mask & required) == required) && ((mask & excluded) == 0 ) && ((mask & allowed) != 0 || allowed == 0);
    }

    template<typename F>
  void tagmaster<F>::self_test()
  {
    tagmaster_default t;
    auto l =t.to_mask("a,b,c");
    auto k = t.to_mask("b,c,a");
    assert(k==l);
    assert(t.to_mask("a,a,c")==t.to_mask("a,c,a"));
    tagmaster_default b("a,b","b,c","d,e");
    assert(b.is_satisfactory("b,a,c") == true);
    assert(b.is_satisfactory("a") == false);
    assert(b.is_satisfactory("b,a,c,d") == false);
    tagmaster_default c;
    assert(c.is_satisfactory("a,b,c"));
  }


}
#endif
