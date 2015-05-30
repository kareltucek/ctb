#ifndef DATATYPES_GUARD
#define DATATYPES_GUARD

#include <string>
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

template <typename ... T> void pass(T...)
{
}

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
    friend class friend_maker<A>::type;
    friend class friend_maker<B>::type;
    friend class friend_maker<C>::type;
    friend class friend_maker<D>::type;
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

enum flags 
{
  fINPUT = 1, fOUTPUT = 2
} ;

#endif 

