#ifndef DATATYPES_GUARD
#define DATATYPES_GUARD

#include <string>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>

#include "errorhandling.h"


namespace ctb
{
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

}

#endif 

