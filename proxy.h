#ifndef PROXY_GUARD
#define PROXY_GUARD

#include "datatypes.h"

namespace ctb
{
  /**
   * General description
   * -------------------
   * Proxy class provides a more or less conceptually correct implementation of public-read-only class fields. Originaly its syntax was supposed to be absolutely transparent. Unfortunately it ended up not being transparent at all.
   *
   * At the moment read only access should work or all types by simple use of '->' operator.
   * [] operator is automatically forwarded as constant reference.
   * Read-write access can be gained using the rw() method. This limitation is due to the fact that compiler chooses version of method prior to checking access privileges.
   *
   * Template parameters
   * -------------------
   * T is the datatype proxied.
   * A,B,C... are types which will have read-write access to the data stored. This is meant to have syntax of a variadic list, unfortunately there seems to be no way of befriending a variadic list.
   *
   * Note: Looking back I have to admit this was a foolish idea and a bad design decision.
   * */

  template <typename T>
    struct friend_maker
    {
      typedef T type;
    };

  template <template <class...> class T>
    struct super_friend_maker
    {
      template <class...S> using type = T<S...>;
    };

  template<class...T>
  struct dummy_friend_v
  {
  };

  struct dummy_friend
  {
  };

  struct dummy
  {
    template<typename...D>
      dummy(D...t){};
  };

  template <class T, class A = dummy_friend, class B = dummy_friend, class C = dummy_friend,  class D = dummy_friend, class E = dummy_friend>
    class proxy_ {
      friend typename friend_maker<A>::type;
      friend typename friend_maker<B>::type;
      friend typename friend_maker<C>::type;
      friend typename friend_maker<D>::type;
      friend typename friend_maker<E>::type;
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

  /*rw proxy is now neccessary for keeping interfaces consistent*/

  template <class T>
    class proxy_rw {
      private:
      //T operator=(const T& arg) { data = arg; return data; }
      T data;
      typedef typename std::remove_reference<typename std::remove_pointer<T>::type>::type result_dereferenced_t;
      typedef typename std::remove_reference<T>::type result_t;
      template <typename U> struct br_ct  { typedef decltype(std::declval<T const>()[std::declval<U>()]) type; };
      template <typename U> struct br_t { typedef decltype(std::declval<T      >()[std::declval<U>()]) type; };
      template <typename U> U& dereference( U* t) const {return *t;};
      template <typename U> const U& dereference(const U& t) const {return t;};
      public:
      result_t& rw() { return data; }
      result_t& r() { return data; }
      operator T&() { return data; }
      T& operator*() { return data; }
      result_t* operator->() { return &data; }
      template <typename U> typename br_t<U>::type operator [](U args) { return data[args]; }
      //const result_t& r() const { return data; }
      //operator const T&() const { return data; }
      //const T& operator*() const { return data; }
      //const result_dereferenced_t* operator->() const { return &dereference(data); }
      //template <typename U> typename br_ct<U>::type operator [](U args) const { return data[args]; }
      template <typename...L> proxy_rw(L&&... args) : data((std::forward<L>(args))...){}
    };
};
#endif 
