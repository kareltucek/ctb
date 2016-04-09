
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
      typedef map<string, string> aliastab_t;
      static aliastab_t aliases;
    public:
      typedef language_cpp language;
      static string get_name();
      static string alias(const string& a, bool* s);
      template <class G> static writer<aliasenv_cf> generate(int m,  G& generator, string name, stringlist args);
      template <class G> static writer<aliasenv_cf> generate_vector_dbg(int m,  G& generator, string name, stringlist args, bool printlog);
      template <class G> static writer<aliasenv_cf> generate_simple_dbg(int m,  G& generator, string name, stringlist args, bool printlog);
  } ;

  map<string, string> aliasenv_cf::aliases;

  string aliasenv_cf::alias(const string& a, bool* s)
  {
    auto itr = aliases.find(a);
    if(itr == aliases.end())
      return aliasenv_bind<aliasenv_cfmacros, aliasenv_generator, language_cpp>::alias(a, s);
    if(s != NULL)
      *s = true;
    return itr->second;
  }

  string aliasenv_cf::get_name()
  {
    return "cf";
  }

  template <class G>
    writer<aliasenv_cf> aliasenv_cf::generate(int granularity, G& generator, string name, stringlist args)
    {
      if(args.size() < 4)
        error("aliasenv_cf expected 3 arguments - one of the following optins as the third argument: 'simpledbg', 'vectordbg', 'vector'");

      aliasenv_cf::aliases.clear();

      if(args[3] == "simpledbg")
        return generate_simple_dbg(granularity, generator, name, args, true);
      else if(args[3] == "vectordbg")
        return generate_vector_dbg(2, generator, name, args, true);
      else if(args[3] == "vector")
        error("not implemented (yet)");
      else  
        error(string("third argument not reckognized: ") + args[3] + " expected one of 'simpledbg', 'vectordbg', 'vector'");

      return writer_plain();
    }

#define CODE_OF_i  " $1 " 
#define CODE_OF_gran  " $2 " 
#define CODE_OF_labels  " $3 " 
#define CODE_OF_labelsr  " $4 " 
#define CODE_OF_gonext  " $5 " 
#define CODE_OF_gonextone  " $6 " 
#define CODE_OF_goprev  " $7 " 
#define CODE_OF_goprevone  " $8 " 
#define CODE_OF_bodyone  " $9 " 
#define CODE_OF_bodyvec  " $10 " 
#define CODE_OF_bodyonesingle  " $11 " 
#define CODE_OF_bodyvecsingle  " $12 " 
#define CODE_OF_emptyind  " $13 " 
#define CODE_OF_logs  " $14 " 
#define CODE_OF_emptygran  " $15 " 
#define CODE_OF_fullgran  " $16 " 

#define LOG(l) (log ? l : "")

  template <class G>
    writer<aliasenv_cf> aliasenv_cf::generate_vector_dbg(int granularity, G& generator, string name, stringlist args, bool log)
    {
      typedef multicontB<writer<aliasenv_cf>> wrt;
      auto opts = generator.option_struct();

      aliases["granularity"] = ctb::to_string(granularity);
      aliases["buffersize"] = ctb::to_string(8*granularity);

      opts["default"].once = false;
      opts["defaultsingle"].once = true;
      opts["labels"].once = true;
      opts["labelsr"].once = true;
      opts["gonext"].once = true;
      opts["goprev"].once = true;
      opts["goprevone"].once = true;
      opts["gonextone"].once = true;
      opts["emptygran"].once = true;
      opts["fullgran"].once = true;
      opts["emptyind"].global_once = true;
      opts["indicator"].global_once = true;

      wrt decl;
      wrt body;
      wrt logs;
      wrt body_logs;
      wrt result;

      decl.print("int terminating = -1;");

      for(int i = 0; i < generator.partition_count(); ++i)
      {
        wrt part;
        wrt partvec;
        generator.generate_partition(i, 1, part, opts);
        generator.generate_partition(i, granularity, partvec, opts);

        decl.print(" $2 ", i, part["global"]);

        //decl.print(" $2 int remains_$1 = 100; ", i, part["global"]);
        //if(part["indicator"].empty()) //not needed yet
          body.print( string()
              //VECTORIZED
              + "{                                                            " 
              + "  PARTITION_$1:                                              " 
              +    CODE_OF_labels
              +    LOG("printf(\"#JUMPED INTO COMPONENT %i\\n\", $1); ")
              + "  if( $1 == 0 && remaining < $2 )                            "
              + "  {                                                          "
              + "    terminating = 0;                                         "
              + "    if(remaining == 0)                                       " 
              + "    {                                                        "
              + "      terminating = 1;                                       "
              +        LOG("printf(\"#FIRST TERMINATED\\n\"); ")
              + "      goto PARTITION_$[$1+1];                                "
              + "    }                                                        "
              + "    goto SIN_PROC_PARTITION_$1;                               "
              + "  }                                                          "
              + "  if( $1 == terminating )                                    "
              + "  {                                                          "
              + "    goto REQ_PARTITION_$1;                                   "
              + "  }                                                          "
              +    CODE_OF_goprev
              +    CODE_OF_gonext
              + "  VEC_PROC_PARTITION_$1:                                     " 
              +    LOG("printf(\"#  AT VECTOR PROC OF COMPONENT %i\\n\", $1); ")
              +    LOG(" $$1 ")
              +    LOG("printf(\"sep\\n\"); ")
              +    CODE_OF_bodyvec
              +    CODE_OF_bodyvecsingle
              + "  if( 0 == $1 )                                              " 
              + "  {                                                          "
              + "    remaining -= $2;                                         " 
              + "  }                                                          " 
              +    LOG(" $$1 ")
              +    LOG("printf(\"sep\\n\"); ")
              + "  goto PARTITION_$1;                                         "
              + "}                                                            "
              + "                                                             "
              
              //SINGLE
              + "{                                                            " 
              + "  REQ_PARTITION_$1:                                          " 
              +    CODE_OF_labelsr
              +    LOG("printf(\"#PULLED INTO COMPONENT %i\\n\", $1); ")
              + "  if( 0 == $1 )                                              " 
              + "  {                                                          "
              + "    goto PARTITION_$1;                                       " 
              + "  }                                                          " 
              + "  if ( terminating == $1 )                                   "
              + "  {                                                          " 
              + "    if(" + CODE_OF_emptyind+ " false)                  " 
              + "    {                                                        " 
              +        LOG("printf(\"#TERMINATED %i\\n\", terminating); ")
              + "      terminating++;                                         " 
              + "      goto PARTITION_$[$1+1];                                " 
              + "    }                                                        " 
              + "    goto SIN_PROC_PARTITION_$1;                              " 
              + "  }                                                          " 
              +    CODE_OF_goprevone
              + "  if(" + CODE_OF_emptygran + " false)                       " 
              + "  {                                                          " 
              + "    goto SIN_PROC_PARTITION_$1;                              " 
              + "  }                                                          " 
              + "  if(" + CODE_OF_fullgran + " false)                        " 
              + "  {                                                          " 
              + "    goto SIN_PROC_PARTITION_$1;                              " 
              + "  }                                                          " 
              + "  goto VEC_PROC_PARTITION_$1;                                " 
              + "  SIN_PROC_PARTITION_$1:                                     " 
              +    LOG("printf(\"#  AT SINGLE PROC OF COMPONENT %i\\n\", $1); ")
              +    CODE_OF_gonextone
              +    LOG(" $$1 ")
              +    LOG("printf(\"sep\\n\"); ")
              +    CODE_OF_bodyone
              +    CODE_OF_bodyonesingle
              + "  if( 0 == $1 )                                              " 
              + "  {                                                          "
              + "    remaining--;                                             " 
              + "  }                                                          " 
              +    LOG(" $$1 ")
              +    LOG("printf(\"sep\\n\"); ")
              + "  goto PARTITION_$1;                                         " 
              + "}                                                            " 
              , i, granularity, part["labels"], part["labelsr"], part["gonext"], part["gonextone"], part["goprev"], part["goprevone"], part["default"], partvec["default"], part["defaultsingle"], partvec["defaultsingle"], part["emptyind"], part["logs"], part["emptygran"], part["fullgran"]);

          logs.print("$1", part["logs"]);
      }
      body_logs.print(LOG("printf(\"sep\\n\"); ")); //initialize table
      body_logs.print("$2 ", logs, body.write_str());
      body_logs.print("PARTITION_$1: ", generator.partition_count()); //flush last table
      body_logs.print(LOG("printf(\"JUMPED TO THE FINISH LOG PARTITION\\n\"); ")); //flush last table
      body_logs.print(LOG(" $1 ;"), logs);
      body_logs.print(LOG("printf(\"sep last\\n\");"));
      body_logs.print("return;"); //flush last table

      result.print("#include <stdio.h>\n");
      result.print("#include <mmintrin.h> \n");
      result.print("#include <xmmintrin.h> \n");
      result.print("#include <emmintrin.h> \n");
      result.print("#include <pmmintrin.h> \n");
      result.print("#include <tmmintrin.h> \n");
      result.print("#include <smmintrin.h> \n");
      result.print("#include <nmmintrin.h>\n");
      result.print("#include \"macros.h\"\n");
      result.print(" void test(int remaining)");
      result.print("{ /*declarations*/ $1 /*body*/ $2 }", decl, body_logs);
      result.print("int main(){test(100); return 0;}");
      result.print("");

      //result.write_std();

      return result;
    }

  template <class G>
    writer<aliasenv_cf> aliasenv_cf::generate_simple_dbg(int granularity, G& generator, string name, stringlist args, bool log)
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
          body.print( string() 
              + "{ PARTITION_$1:  $2" //labels
              + "{ "
              + LOG("printf(\"#JUMPED INTO COMPONENT %i\\n\", $1); ")
              + " $3 " //go down
              + " $4 " 
              + "if(remains_$1 == 0) goto PARTITION_$[$1+1];"
              + "if(remains_0 == 0) goto PARTITION_$1_PROC;"
              + " $5 " //go up
              + "PARTITION_$1_PROC: "
              + LOG(" $$1 ")
              + LOG("printf(\"sep\\n\"); ")
              + " $6 " //perform body
              + "remains_$1--;"
              + LOG(" $$1 ")
              + LOG("printf(\"sep\\n\"); ")
              + "goto PARTITION_$1;} }", i, part["labels"], part["gonext"], part["labels_req"], part["goprev"], part["default"], part["logs"]);
          logs.print("$1", part["logs"]);
      }
      body_logs.print(LOG("printf(\"sep\\n\"); ")); //initialize table
      body_logs.print("$2 ", logs, body.write_str());
      body_logs.print("PARTITION_$1: ", generator.partition_count()); //flush last table
      body_logs.print(LOG("printf(\"#JUMPED TO THE FINISH LOG PARTITION\\n\"); ")); //flush last table
      body_logs.print(LOG(" $1 ;"), logs);
      body_logs.print(LOG("printf(\"sep last\\n\");"));
      body_logs.print( "return;"); //flush last table

      result.print("#include <stdio.h>\n");
      result.print(" void test(){$1 $2 /*PARTITION_$3: return;*/}", decl, body_logs, generator.partition_count());
      result.print("int main(){test(); return 0;}");
      result.print("");

      //result.write_std();

      return result;
    }
};

#endif

