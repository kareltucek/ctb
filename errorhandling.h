#ifndef ERROR_GUARD
#define ERROR_GUARD

#include "defines.h"
#include <iostream>
#include <string>

namespace ctb
{
  typedef pair<string,bool> error_struct;

  void error(const string& e, bool critical = true)
  {
    throw error_struct(e,critical);
  }

  void warning(string e)
  {
    cerr << endl << "warning: " << e << endl;
  }

  void warn(string e)
  {
    cerr << endl << "warning: " << e << endl;
  }
};

#endif
