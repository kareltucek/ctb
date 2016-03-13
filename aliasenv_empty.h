
#ifndef aliasenv_EMPTY_GUARD
#define aliasenv_EMPTY_GUARD

#include "defines.h"
#include "languages.h"
#include "writer.h"
#include <map>
#include <stack>

namespace ctb
{

  class aliasenv_empty
  {
    public:
      typedef language_empty language;
      static string alias(const string& a, bool* success = NULL);
      static string get_name(){return "empty";};
  };

  string aliasenv_empty::alias(const string& a, bool* s)
  {
    if(s != NULL)
      *s = false;
    return "";
  }
};

#endif
