#ifndef PROXY_GUARD
#define PROXY_GUARD

#include "datatypes.h"

namespace ctb
{
  /** 
   * Proxy class provides a more or less conceptually correct implementation of public-read-only class fields. Originaly its syntax was supposed to be absolutely transparent. Unfortunately it ended up not being transparent at all.
   * */

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

  template <class T, class A = dummy_friend, class B = dummy_friend, class C = dummy_friend,  class D = dummy_friend>
    class proxy_ {
      friend typename friend_maker<A>::type;
      friend typename friend_maker<B>::type;
      friend typename friend_maker<C>::type;
      friend typename friend_maker<D>::type;
      private:
      //T operator=(const T& arg) { data = arg; return data; }
      T data;
      typedef typename std::remove_reference<typename std::remove_pointer<T>::type>::type result_dereferenced_t;
      typedef typename std::remove_reference<T>::type result_t;
      template <typename U> struct br_ct  { typedef decltype(std::declval<T const>()[std::declval<U>()]) type; };
      template <typename U> struct br_t { typedef decltype(std::declval<T      >()[std::declval<U>()]) type; };
      template <typename U> U& dereference( U* t) const {return *t;};
      template <typename U> const U& dereference(const U& t) const {return t;};
      result_t& rw() { return data; }
      //operator T&() { return data; }
      //T& operator*() { return data; }
      //result_t* operator->() { return &data; }
      //template <typename U> typename br_t<U>::type operator [](U args) { return data[args]; }
      public:
      const result_t& r() const { return data; }
      operator const T&() const { return data; }
      const T& operator*() const { return data; }
      const result_dereferenced_t* operator->() const { return &dereference(data); }
      template <typename U> typename br_ct<U>::type operator [](U args) const { return data[args]; }
      template <typename...L> proxy_(L&&... args) : data((std::forward<L>(args))...){}
    };
};
#endif 
