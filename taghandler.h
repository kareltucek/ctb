#ifndef TAGHANDLER_GUARD
#define TAGHANDLER_GUARD

#include "defines.h"
#include <map>
#include "datatypes.h"

namespace ctb
{
  /** 
   * taghandler_base defines an interface for a general tag handling structure
   * */

  class taghandler_base
  {
    public:
      virtual bool is_satisfactory(const string&) = 0;
  };

}
#endif
