#ifndef CONVERT_GUARD
#define CONVERT_GUARD

#include "defines.h"
#include <string>
#include <functional>

namespace ctb
{

template <typename A, typename B>
struct cvt
{
  static B convert(const B&);
};

template <typename A>
struct cvt<A,A>
{
  static A convert(const A& a){return a;};
};

template <> struct cvt<int,string> { static string convert(const int& a){return to_string(a);}; };

template <> struct cvt<string,int>
{
  static int convert(const string& a)
  {
    try
    {
      return std::stoi(a);
    }
    catch(...)
    {
      return hash<string>()(a);
    }
  };
};

}

#endif
