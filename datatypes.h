#ifndef DATATYPES_GUARD
#define DATATYPES_GUARD

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

  struct traits
  {
    typedef std::string opid_t;
    typedef std::string tid_t;
    typedef std::string vid_t;
    typedef uint32_t flag_t;
    typedef int param_t;
    typedef tagmaster<uint32_t> tag_handler_t;
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


  bool fileexists(const char *name)
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
    if(str[str.length()-1] == d)
      l.push_back("");
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
          error( std::string("unknown flag found: ", false).append(w));
        }
      }
      return f;
    }

  template <typename S>
    void openstream(S& stream, const std::string& name, bool check_existence = true)
    {
      stream.open(name);
      if(!stream.is_open())
      {
        if(!fileexists(name.c_str()) && check_existence)
        {
            error(std::string("file does not exist: ").append(name));
        }
        else
        {
          error(std::string("file could not be opened: ").append(name));
        }
      }
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

  std::string exec_path;


  std::string to_string(std::string str)
  {
    return str;
  }

  std::string to_string(int i)
  {
    return std::to_string(i);
  }

  int stoi(std::string str)
  {
    //will want some evaluation here later...
    int result = 0;
    try
    {
      result = std::stoi(str);
    }
    catch(...)
    {
      error(str.append(" does not look like an integer"), false);
    }
    return result;
  }




};

#endif 

