#ifndef ERROR_GUARD
#define ERROR_GUARD

#include "defines.h"
#include <iostream>
#include <string>

namespace ctb
{
  typedef pair<string,bool> error_struct;
  static bool warn_as_error = false;

  void error(const string& e, bool critical = true)
  {
#ifdef TESTOVANI
    const char* err = e.c_str();
    int a = 0; 
    int b = 5/a;
    cerr << b << err << endl;
#endif
    throw error_struct(e,critical);
  }

  void warning(string e)
  {
    if(warn_as_error)
      error(e);
    else
      cerr << endl << "warning: " << e << endl;
  }

  void warn(string e)
  {
    warning(e);
  }
};

#ifndef TESTOVANI
#define RETHROW(msg)\
catch(std::exception& e)\
{\
  error(string(e.what())+"\n    "+msg);\
}\
catch (error_struct& err)\
{\
  error(string(msg) + "\n    "+err.first, err.second);\
}
#else
#define RETHROW(msg)\
  catch (error_struct& err)\
{\
  error(string(msg) + "\n    "+err.first, err.second);\
}
#endif

#endif


