
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
      template <class G> static void get_signatures(G& generator, string& in_indices, string& out_indices, string& in_types, string& out_types, string& in_types_il, string& out_types_il);
      template <class G, class A, typename...T> static writer_nonexpansive_cpp_formatter generate_function(int granularity, int buffersize,  G& generator, string name, stringlist args, bool printlog, bool vectorized, T... tags);
      static writer<aliasenv_cf>* shiftoutput;
      static writer<aliasenv_cf>* shiftacum;
    public:
      typedef language_cpp language;
      static string get_name();
      static string alias(const string& a, bool* s);
      template <class G> static writer_nonexpansive_cpp_formatter generate(int m,  G& generator, string name, stringlist args);
      template <class G, class A> static writer_nonexpansive_cpp_formatter generate_vectorized(int granularity,  G& generator, string name, stringlist args, bool printlog);
      template <class G, class A> static writer_nonexpansive_cpp_formatter generate_simple_dbg(int granularity,  G& generator, string name, stringlist args, bool printlog);
  } ;

  map<string, string> aliasenv_cf::aliases;
  writer<aliasenv_cf>* aliasenv_cf::shiftoutput = NULL;
  writer<aliasenv_cf>* aliasenv_cf::shiftacum = NULL;

#define TOINT(a) ctb::stoi(writer<aliasenv_cf>().print(a ).write_str())
  string aliasenv_cf::alias(const string& a, bool* s)
  {
    if(a == "EXPANDSHIFT" && shiftoutput != NULL && shiftacum != NULL)
    {
      int cnt = TOINT("$widthout");
      shiftoutput->print("switch($offset){");
      for(int i = 0; i < cnt; i++)
      {
        //note that we *need* to expand one level of dollar expansion so we *cannot* call the print directly with writer
        shiftoutput->print("case $constoffset: $2 break;", i, shiftacum->write_str());
      }
      shiftoutput->print("}");
      shiftacum->clear();
      if(s != NULL)
        *s = true;
      return "";
    }

    auto itr = aliases.find(a);
    if(itr == aliases.end())
      return aliasenv_bind<aliasenv_cfmacros, aliasenv_generator, language_cpp>::alias(a, s);
    if(s != NULL)
      *s = true;
    return itr->second;
  }

#undef TOINT
#define SET(a,b) aliases[a]=b

  string aliasenv_cf::get_name()
  {
    return "cf";
  }

  template <class G>
    writer_nonexpansive_cpp_formatter aliasenv_cf::generate(int granularity, G& generator, string name, stringlist args)
    {
      if(args.size() < 4)
        error("not enough arguments: syntax: <command> cf {simpledbg|vectordbg|vector} [packsize]");

      int g = args.size() > 4 ? ctb::stoi(args[4]) : granularity;

      aliasenv_cf::aliases.clear();

      if(args[3] == "simpledbg")
        return generate_simple_dbg<G, aliasenv_cf>(g, generator, name, args, true);
      else if(args[3] == "vectordbg")
        return generate_vectorized<G, aliasenv_cf>(g, generator, name, args, true);
      else if(args[3] == "testgran2")
        return generate_vectorized<G, aliasenv_cf>(2, generator, name, args, true);
      else if(args[3] == "vector")
        return generate_vectorized<G, aliasenv_cf>(g, generator, name, args, false);
      else  
        error(string("third argument not reckognized: ") + args[3] + " expected one of 'simpledbg', 'vectordbg', 'vector'");

      return writer_nonexpansive_cpp_formatter();
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
#define CODE_OF_bodyoneinc  " $17 " 
#define CODE_OF_bodyvecinc  " $18 " 

#define LOG(l) (log ? l : "")

  template <class G, class A>
    writer_nonexpansive_cpp_formatter aliasenv_cf::generate_vectorized(int granularity, G& generator, string name, stringlist args, bool log)
    {
      writer_nonexpansive_cpp_formatter result;
      writer_nonexpansive_cpp_formatter aligned, shifted, unalign, singled;

      granularity = granularity > 16 ? 16 : granularity;

      int buffersize = granularity*2;
      if(generator.partition_count() > 1)
      {
        if(buffersize%16 != 0)
          buffersize = (((buffersize/16)+1)*16);
      }

      auto tags_unaligned = make_shared<tagmaster_default>("","unalignedio,universal","","");
      auto tags_aligned = make_shared<tagmaster_default>("","alignedio,universal","","");
      auto tags_shifted = make_shared<tagmaster_default>("","shiftedio,universal","","");

      ADD("input", "ictx.data_$ioindex[ictx.index_$ioindex+$iindex]");
      ADD("output", "octx.data_$ioindex[octx.index_$ioindex+$iindex]");
      ADD("inputinc", "ictx.index_$ioindex+=$packsize;");
      ADD("outputinc", "octx.index_$ioindex+=$packsize;");
      ADD("offset", "ictx.offset_$ioindex");
      ADD("constoffset", "$1");

      aligned = generate_function<G, A>(granularity, buffersize, generator, "aligned", args, log, true, tags_aligned);
      shifted = generate_function<G, A>(granularity, buffersize, generator, "shifted", args, log, true, tags_shifted);
      unalign = generate_function<G, A>(granularity, buffersize, generator, "unaligned", args, log, true, tags_unaligned);
      singled = generate_function<G, A>(granularity, buffersize, generator, "single", args, log, false, tags_unaligned);
      
      result.print("#include <stdio.h>\n");
      result.print("#include <mmintrin.h> \n");
      result.print("#include <xmmintrin.h> \n");
      result.print("#include <emmintrin.h> \n");
      result.print("#include <pmmintrin.h> \n");
      result.print("#include <tmmintrin.h> \n");
      result.print("#include <smmintrin.h> \n");
      result.print("#include <nmmintrin.h>\n");
      result.print("#include \"macros.h\"\n");
      //result.print("int main(){test(100); return 0;}");
      result.print("template <typename CTXIN, typename CTXOUT>");
      result.print("class box_$1", name);
      result.print("{");
      result.print("public:");
      result.print("$1 $2 $3 $4", aligned, shifted, unalign, singled);
      result.print("};");

      string in_indices, in_types, in_types_il, out_types_il, out_types, out_indices;
      get_signatures(generator, in_indices, out_indices, in_types, out_types, in_types_il, out_types_il);

      result.print("#define IN_INDICES_$1 $2", name, in_indices);
      result.print("#define IN_TYPES_$1 $2", name, in_types);
      result.print("#define IN_TYPES_IL_$1 $2", name, in_types_il);
      result.print("#define OUT_INDICES_$1 $2", name, out_indices);
      result.print("#define OUT_TYPES_$1 $2", name, out_types);
      result.print("#define OUT_TYPES_IL_$1 $2", name, out_types_il);
      result.print("#ifdef RUN_BOX\n");
      result.print("RUN_BOX(box_$1, IN_INDICES_$1, IN_TYPES_$1, IN_TYPES_IL_$1, OUT_INDICES_$1, OUT_TYPES_$1, OUT_TYPES_IL_$1)\n", name);
      result.print("#endif");
      return result;
    }

  template <class G>
    void aliasenv_cf::get_signatures(G& generator, string& in_indices, string& out_indices, string& in_types, string& out_types, string& in_types_il, string& out_types_il)
    {
      map<int, string> in;
      map<int, string> out;

      int procin = 0;
      int procout = 1;

      for(int proc = 0; proc < 2; proc++)
      {
        const auto& cont = proc == procin ? generator.graph.in : generator.graph.out;
        const auto& cont2 = proc == procin ? in : out;

        for(auto n : cont)
        {
          string type_string, index;
          n->data.op->get_type_string(1, type_string);
          index = n->data.get_param("ioindex");
          int numberindex = ctb::stoi(index);
          if(proc == procin)
            in[numberindex] = type_string;
          else
            out[numberindex] = type_string;
        }
        writer_plain wt, wi, wti;
        int last = -1;
        for(auto& itr : cont2)
        {
          while(last < itr.first-1)
          {
            wti.push("int");
            last++;
          }
          wi.push(itr.first);
          wt.push(itr.second);
          wti.push(itr.second);
          last++;
        }
        wi.list_concat(",");
        wt.list_concat(",");
        wti.list_concat(",");
        if(proc == procin)
        {
          in_indices = wi.write_str();
          in_types = wt.write_str();
          in_types_il = wti.write_str();
        }
        else
        {
          out_indices = wi.write_str();
          out_types = wt.write_str();
          out_types_il = wti.write_str();
        }
      }
    }

  template <class G, class A, typename...T>
    writer_nonexpansive_cpp_formatter aliasenv_cf::generate_function(int granularity, int buffersize, G& generator, string name, stringlist args, bool log, bool vectorized, T... tags)
    {
      typedef imp_contB<writer<A>> wrt;
      auto opts = generator.option_struct();

      aliases["granularity"] = ctb::to_string(granularity);
      aliases["buffersize"] = ctb::to_string(buffersize);

      /**
       * user-space layers:
       * -default
       * -shiftacum
       * -shiftacumpreload
       * -preload
       * -global
       * */

      opts["default"].once = false;
      opts["default"].order = 4;
      opts["shiftacum"].once = false;
      opts["shiftacum"].order = 3;
      opts["preload"].layer = "default";
      opts["preload"].order = 1;
      opts["preload"].once = true;
      opts["shiftacumpreload"].layer = "shiftacum";
      opts["shiftacumpreload"].order = 2;
      opts["shiftacumpreload"].once = true;
      opts["defaultsingle"].once = true;
      opts["labels"].once = true;
      opts["labelsr"].once = true;
      opts["gonext"].once = true;
      opts["goprev"].once = true;
      opts["goprevone"].once = true;
      opts["gonextone"].once = true;
      opts["emptygran"].once = true;
      opts["fullgran"].once = true;
      opts["increments"].once = true;
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
        
        aliasenv_cf::shiftacum = &(partvec["shiftacum"]);
        aliasenv_cf::shiftoutput = &(partvec["default"]);

        generator.generate_partition(i, 1, part, opts, tags...);
        generator.generate_partition(i, granularity, partvec, opts, tags...);

        decl.print(" $2 ", i, part["global"]);

        string factorminmax = "";
        if((generator.partition_is_topo_min(i) || generator.partition_is_topo_max(i)) && vectorized)
          factorminmax = "goto PARTITION_$1;";

        string singlebodyswitch = "";
        if(!vectorized)
          singlebodyswitch = "goto SIN_PROC_PARTITION_$1;";

        //decl.print(" $2 int remains_$1 = 100; ", i, part["global"]);
        //if(part["indicator"].empty()) //not needed yet
          body.print( string()
              //VECTORIZED
              + "{                                                            " 
              + "  PARTITION_$1:                                              " 
              +    CODE_OF_labels
              +    LOG("printf(\"//JUMPED INTO COMPONENT %i\\n\", $1); ")
              + "  if( $1 == 0 && remaining < $2 )                            "
              + "  {                                                          "
              + "    terminating = 0;                                         "
              + "    if(remaining == 0)                                       " 
              + "    {                                                        "
              + "      terminating = 1;                                       "
              +        LOG("printf(\"//FIRST TERMINATED\\n\"); ")
              + "      goto PARTITION_$[$1+1];                                "
              + "    }                                                        "
              + "    goto SIN_PROC_PARTITION_$1;                               "
              + "  }                                                          "
              + "  if( $1 == terminating )                                    "
              + "  {                                                          "
              + "    goto REQ_PARTITION_$1;                                   "
              + "  }                                                          "
              +    CODE_OF_goprev
              +    singlebodyswitch
              +    CODE_OF_gonext
              + "  VEC_PROC_PARTITION_$1:                                     " 
              +    LOG("printf(\"//  AT VECTOR PROC OF COMPONENT %i\\n\", $1); ")
              +    CODE_OF_bodyvec
              +    CODE_OF_bodyvecsingle
              +    CODE_OF_bodyvecinc
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
              +    LOG("printf(\"//PULLED INTO COMPONENT %i\\n\", $1); ")
              + "  if( 0 == $1 )                                              " 
              + "  {                                                          "
              + "    goto PARTITION_$1;                                       " 
              + "  }                                                          " 
              + "  if ( terminating == $1 )                                   "
              + "  {                                                          " 
              + "    if(" + CODE_OF_emptyind+ " false)                  " 
              + "    {                                                        " 
              +        LOG("printf(\"//TERMINATED %i\\n\", terminating); ")
              + "      terminating++;                                         " 
              + "      goto PARTITION_$[$1+1];                                " 
              + "    }                                                        " 
              + "    goto SIN_PROC_PARTITION_$1;                              " 
              + "  }                                                          " 
              +    singlebodyswitch
              +    factorminmax
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
              +    LOG("printf(\"//  AT SINGLE PROC OF COMPONENT %i\\n\", $1); ")
              +    CODE_OF_gonextone
              +    CODE_OF_bodyone
              +    CODE_OF_bodyonesingle
              +    CODE_OF_bodyoneinc
              + "  if( 0 == $1 )                                              " 
              + "  {                                                          "
              + "    remaining--;                                             " 
              + "  }                                                          " 
              +    LOG(" $$1 ")
              +    LOG("printf(\"sep\\n\"); ")
              + "  goto PARTITION_$1;                                         " 
              + "}                                                            " 
              , i, granularity, part["labels"], part["labelsr"], part["gonext"], part["gonextone"], part["goprev"], part["goprevone"], part["default"], partvec["default"], part["defaultsingle"], partvec["defaultsingle"], part["emptyind"], part["logs"], part["emptygran"], part["fullgran"], part["increments"], partvec["increments"]);

          logs.print("$1", part["logs"]);
      }
      body_logs.print(LOG("printf(\"sep\\n\"); ")); //initialize table
      body_logs.print("$2 ", logs, body.write_str());
      body_logs.print("PARTITION_$1: ", generator.partition_count()); //flush last table
      body_logs.print(LOG("printf(\"JUMPED TO THE FINISH LOG PARTITION\\n\"); ")); //flush last table
      body_logs.print(LOG(" $1 ;"), logs);
      body_logs.print(LOG("printf(\"sep last\\n\");"));
      body_logs.print("return;"); //flush last table

      result.print("static void process_$1(int remaining, CTXIN& ictx, CTXOUT& octx)", name);
      result.print("{ /*declarations*/ $1 /*body*/ $2 }", decl, body_logs);

      //result.write_std();

      return result;
    }

  template <class G, class A>
    writer_nonexpansive_cpp_formatter aliasenv_cf::generate_simple_dbg(int granularity, G& generator, string name, stringlist args, bool log)
    {
      typedef imp_contB<writer<A>> wrt;
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
              + LOG("printf(\"//JUMPED INTO COMPONENT %i\\n\", $1); ")
              + " $3 " //go down
              + " $4 " 
              + "if(remains_$1 == 0) goto PARTITION_$[$1+1];"
              + "if(remains_0 == 0) goto PARTITION_$1_PROC;"
              + " $5 " //go up
              + "PARTITION_$1_PROC: "
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
      body_logs.print(LOG("printf(\"//JUMPED TO THE FINISH LOG PARTITION\\n\"); ")); //flush last table
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

