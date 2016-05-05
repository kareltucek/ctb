#ifndef SPLIT_GUARD
#define SPLIT_GUARD

#include "defines.h"
#include <string>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>

#include "errorhandling.h"


namespace ctb
{
  typedef vector<string> stringlist;

  string trim(const string& s)
  {
    int end = s.find_last_not_of(" \n\r");
    int begin = s.find_first_not_of(" \n\r");
    if(end == -1)
      return s;
    else
      return s.substr(begin, end-begin+1);
  }


  bool fileexists(const char *name)
  {
    struct stat   buffer;
    return (stat (name, &buffer) == 0);
  }

  vector<string> split(string str, char d, bool squash = false)
  {
    string item;
    vector<string> l;
    istringstream ss(str);
    while(getline(ss, item, d))
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
    if(str[str.length()-1] == d && !squash)
      l.push_back("");
    return l;
  }

  vector<string> splitlist(string str, bool just_nonempty = true)
  {
    stringlist result;
    stringlist list = split(str, ',', false);
    for(int i = 0; i < list.size(); ++i)
    {
      string res = trim(list[i]);
      if(res.empty() && just_nonempty && list.size() != 0)
        warning(string("Empty record found in nonempty list: ")+str);
      if(!res.empty() || !just_nonempty)
        result.push_back(res);
    }
    return list;
  }


  vector<string> split_first(string str, char d)
  {
    int pos = str.find(d);
    if(pos == string::npos)
      error(string("expected ")+d+" in string "+str);
    vector<string> res;
    res.push_back( str.substr(0, pos) );
    res.push_back( str.substr(pos+1, str.length()-pos-1));
    return res;
  }

  map<string,string> split_params(string str, char d = ',', char d2 = '=', bool trim_ = true)
  {
    map<string, string> res;
    vector<string> pairs = split(str, d, true);
    for(const auto& pair : pairs)
    {
      if(pair.empty())
        continue;
      vector<string> p = split_first(pair, d2);
      if(p.size() != 2)
        error(string("expected ") + d2 + " in " + str + " while splitting parameters");
      if(trim_ && trim(p[0]).empty())
        error(string("key empty in parameter splitting with string: " ) + str);
      if(trim_)
        res[trim(p[0])] = trim(p[1]);
      else
        res[p[0]] = p[1];
    }
    return res;
  }


  template <typename S>
    void openstream(S& stream, const string& name, bool check_existence = true)
    {
      stream.open(name);
      if(!stream.is_open())
      {
        if(!fileexists(name.c_str()) && check_existence)
        {
          error(string("file does not exist: ").append(name));
        }
        else
        {
          error(string("file could not be opened: ").append(name));
        }
      }
    }

}

#endif 

