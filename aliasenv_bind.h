
#ifndef aliasenv_bind_GUARD
#define aliasenv_bind_GUARD

#include "defines.h"
#include "languages.h"

namespace ctb
{

  template < class A, class B, class L, bool R = true>
    class aliasenv_bind
    {
      public:
        typedef L language;
        static string alias(const string& a, bool* success = NULL)
        {
          //cout << "bind alias called" << endl;
          bool my_succ = false;
          string res;
          res = A::alias(a, &my_succ);
          if(!my_succ)
            res = B::alias(a, &my_succ);
          if( success != NULL)
            *success = my_succ;
          if(!my_succ && R)
            warning(string("bind aliasenv did not found alias: ") + a);
          return res;
        };
    };
};

#endif

