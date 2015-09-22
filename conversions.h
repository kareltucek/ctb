#ifndef CONVERT_GUARD
#define CONVERT_GUARD

#include <string>
#include <functional>

namespace ctb
{

template <typename A, typename B>
struct cvt
{
  static B convert(const B&);
};

template <> struct cvt<int,std::string> { static std::string convert(const int& a){return std::to_string(a);}; };

template <> struct cvt<std::string,int>
{
  static int convert(const std::string& a)
  {
    try
    {
      return std::stoi(a);
    }
    catch(...)
    {
      return std::hash<std::string>()(a);
    }
  };
};

}

#endif
