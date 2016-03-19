#ifndef DATATYPES_GUARD
#define DATATYPES_GUARD

#include "defines.h"
#include "split.h"
#include <string>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include "proxy.h"
#include "errorhandling.h"

namespace ctb
{
  /** Datatypes contains various general helper functions and datatypes
   *
   * Some (most) of these should later be (theoretically) moved into separate units.
   * */
  //various helper classes

  template<typename F>
    class tagmaster;

  struct traits_conv_test
  {
    typedef int opid_t;
    typedef int tid_t;
    typedef int vid_t;
    typedef uint32_t flag_t;
    typedef string param_t;
    static const int maxarity = 3;
  } ;

  struct traits
  {
    typedef string opid_t;
    typedef string tid_t;
    typedef string vid_t;
    typedef uint32_t flag_t;
    typedef int param_t;
    //typedef tagmaster<uint32_t> tag_handler_t;
    static const int maxarity = 3;
  } ;

  const int gmCOUNT = 3;
  enum maskmodes { gSELECT = 0, gPRINT = 1, gONCE = 2};
  enum maskmodes_masks { mSELECT = 1 , mPRINT = 2, mONCE = 4 };

  enum flags 
  {
    fINPUT = 1, fOUTPUT = 2, fDEBUG = 4
  } ;

  template <typename ... T> void pass(T...)
  {
  }


  template<typename F>
    F string_to_flags(string str)
    {
      static map<string, F> hash = {{"output",fOUTPUT},{"input",fINPUT},{"debug",fDEBUG}};
      F f = 0;
      stringlist words = splitlist(str);
      for(auto w : words)
      {
        if(hash.find(w) != hash.end() )
        {
          f |= hash[w];
        }
        else
        {
          error( string("unknown flag found: ").append(w).append("\n"), false);
        }
      }
      return f;
    }

  template<typename F>
    string flags_to_string(F f)
    {
      string b;
      static map<string, F> hash = {{"output",fOUTPUT},{"input",fINPUT},{"debug",fDEBUG}};

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

  string exec_path;


  string to_string(string str)
  {
    return str;
  }

  string to_string(int i)
  {
    return std::to_string(i);
  }

  string to_string(char* c)
  {
    return std::string(c);
  }

  int stoi(string str)
  {
    //may want some evaluation here later...
    int result = 0;
    try
    {
      result = std::stoi(str);
    }
    catch(...)
    {
      error(string("'").append(str).append("' does not look like an integer"), false);
    }
    return result;
  }

  template <typename A, typename B>
  map<string,string> toparam(A a, B b)
  {
    return {{ctb::to_string(a), ctb::to_string(b)}};
  }


  template <typename T, T V>
    struct value_container
    {
      static const T value = V;
    };


};

#endif 

