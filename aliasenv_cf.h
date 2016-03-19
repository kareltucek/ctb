
#ifndef aliasenv_cf_GUARD
#define aliasenv_cf_GUARD

#include "defines.h"
#include "writer.h"
#include "aliasenv_bind.h"
#include <map>

namespace ctb
{
  /** 
   * */

  class aliasenv_cf : public aliasenv_bind<aliasenv_cfmacros, aliasenv_generator, language_cpp>
  {
    protected:
    public:
      typedef language_cpp language;
      static string get_name();
      template <class G> static writer<aliasenv_cf> generate(int m,  G& generator, string name);
  } ;

  string aliasenv_cf::get_name()
  {
    return "cf";
  }

  template <class G>
    writer<aliasenv_cf> aliasenv_cf::generate(int granularity, G& generator, string name)
    {
      typedef multicontB<writer<aliasenv_bobox>> wrt;
      auto opts = generator.option_struct();

      wrt result;

      for(int i = 0; i < generator.partition_count(); ++i)
      {
        wrt part;
        generator.generate_partition(i, granularity, part, opts);
      }
      result.write_std();

      return result;
    }
};

#endif

