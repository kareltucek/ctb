
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

  class aliasenv_cf 
  {
    protected:
    public:
      typedef language_cpp language;
      static string get_name();
      static string alias(const string& a, bool* s);
      template <class G> static writer<aliasenv_cf> generate(int m,  G& generator, string name);
  } ;

  string aliasenv_cf::alias(const string& a, bool* s)
  {
    return aliasenv_bind<aliasenv_cfmacros, aliasenv_generator, language_cpp>::alias(a, s);
  }

  string aliasenv_cf::get_name()
  {
    return "cf";
  }

  template <class G>
    writer<aliasenv_cf> aliasenv_cf::generate(int granularity, G& generator, string name)
    {
      typedef multicontB<writer<aliasenv_cf>> wrt;
      auto opts = generator.option_struct();

      opts["default"].once = false;
      opts["labels"].once = true;
      opts["gonext"].once = true;
      opts["goprev"].once = true;
      opts["indicator"].global_once = true;

      wrt decl;
      wrt body;
      wrt logs;
      wrt body_logs;
      wrt result;

      for(int i = 0; i < generator.partition_count(); ++i)
      {
        wrt part;
        generator.generate_partition(i, granularity, part, opts);
        decl.print(" $2 int remains_$1 = 100; ", i, part["global"]);
        //if(part["indicator"].empty()) //not needed yet
          body.print(
              "{ PARTITION_$1:  $2" //labels
              "{ "
              "printf(\"JUMPED INTO COMPONENT %i\\n\", $1); "
              "$3 $4" //go down, requie labels
              "if(remains_$1 == 0) goto PARTITION_$[$1+1];"
              "if(remains_0 == 0) goto PARTITION_$1_PROC;"
              " $5 " //go up
              "PARTITION_$1_PROC: "
              " $$1 "
              "printf(\"sep\\n\"); "
              " $6 " //perform body
              "remains_$1--;"
              " $$1 "
              "printf(\"sep\\n\"); "
              "goto PARTITION_$1;} }", i, part["labels"], part["gonext"], part["labels_req"], part["goprev"], part["default"], part["logs"]);
        //else
        //{
        //  body.print("{ PARTITION_$1:  $2 {$3  $4 $5 goto PARTITION_$1;} }");
        //}
          logs.print("$1", part["logs"]);
      }
      body_logs.print("printf(\"sep\\n\"); "); //initialize table
      body_logs.print("$2 ", logs, body.write_str());
      body_logs.print("PARTITION_$1: ", generator.partition_count()); //flush last table
      body_logs.print("printf(\"JUMPED TO THE FINISH LOG PARTITION\\n\"); "); //flush last table
      body_logs.print(" $1 ;", logs);
      body_logs.print("printf(\"sep last\\n\"); return;"); //flush last table

      result.print("#include <stdio.h>\n");
      result.print(" void test(){$1 $2 /*PARTITION_$3: return;*/}", decl, body_logs, generator.partition_count());
      result.print("int main(){test(); return 0;}");
      result.print("");

      //result.write_std();

      return result;
    }
};

#endif

