#ifndef ERROR_GUARD
#define ERROR_GUARD

#include <iostream>
#include <string>

namespace ctb
{
  typedef std::pair<std::string,bool> error_struct;

  void error(const std::string& e, bool critical = true)
  {
    throw error_struct(e,critical);
  }

  void warning(std::string e)
  {
    std::cerr << std::endl << "warning: " << e << std::endl;
  }

  void warn(std::string e)
  {
    std::cerr << std::endl << "warning: " << e << std::endl;
  }
};

#endif
