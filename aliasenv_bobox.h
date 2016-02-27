
#ifndef aliasenv_BOBOX_GUARD
#define aliasenv_BOBOX_GUARD

#include "defines.h"
#include "writer.h"
#include "aliasenv_bobox.h"
#include <map>

namespace ctb
{
  /** see the aliasenv_generator */
  class aliasenv_bobox : public aliasenv_generator
  {
    protected:
      typedef map<string, string> aliastab_t;
      static aliastab_t aliases;
      static void init();
    public:
      typedef language_cpp language;
      static string alias(const string& a, bool* s = NULL);
      static string get_name();
      template <class G> static writer<aliasenv_bobox> generate(int m,  G& generator, string name);
  } ;

  map<string, string> aliasenv_bobox::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))
#define SET(a,b) aliases[a]=b

  string aliasenv_bobox::get_name()
  {
    return "bobox";
  }

  string aliasenv_bobox::alias(const string& a, bool* s)
  {
    auto itr = aliases.find(a);
    if(itr == aliases.end())
      return aliasenv_generator::alias(a, s);
    if(s != NULL)
      *s = true;
    return itr->second;
  }


  void aliasenv_bobox::init()
  {
    static bool initialized = false;
    if(initialized)
      return;

    aliasenv_generator::init();

    ADD("input", "data_in_$cindex[pos_in_$cindex+j]");
    ADD("output", "data_out_$cindex[pos_out_$cindex+j]");

    ADD("fdeclin",  writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/bobox_decl_in.h")));
    ADD("fdeclout", writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/bobox_decl_out.h")));
    ADD("fenvin",   writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/bobox_env_in.h")));
    ADD("fenvout",  writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/bobox_env_out.h")));
    ADD("fbox",     writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/bobox_box.h")));
    ADD("fsend",    writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/bobox_send.h")));
    ADD("fcode",    writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/bobox_code.h")));
    ADD("falign",   writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/bobox_align.h")));
    ADD("fcase",    writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/bobox_case.h")));

    initialized = true;
  }


  template <class G>
    writer<aliasenv_bobox> aliasenv_bobox::generate(int granularity, G& generator, string name)
    {
      typedef writer<aliasenv_bobox> wrt;

      init();
      wrt ilist;
      wrt olist;
      string type_string;

      //construct box list definitions
      for(auto n : generator.graph.in)
        ilist.push("input_list_$1, $1", n->data.get_inout_pos());
      ilist.list_concat(",");

      for(auto n : generator.graph.out)
        olist.push("output_list_$1, $1", n->data.get_inout_pos());
      olist.list_concat(",");

      //construct declarations
      wrt decl;
      for(auto n : generator.graph.in)
      {
        n->data.op->get_type_string(1, type_string);
        decl.print("$fdeclin", n->data.get_inout_pos(), type_string);
      }
      for(auto n : generator.graph.out)
      {
        n->data.op->get_type_string(1, type_string);
        decl.print("$fdeclout", n->data.get_inout_pos(), type_string);
      }

      //accept envelopes
      wrt envelopes;
      for(auto n : generator.graph.in)
      {
        n->data.op->get_type_string(1, type_string);
        envelopes.print("$fenvin", n->data.get_inout_pos(), type_string);
      }
      for(auto n : generator.graph.out)
      {
        n->data.op->get_type_string(1, type_string);
        envelopes.print("$fenvout", n->data.get_inout_pos(),  type_string);
      }

      //get batch size
      wrt minlist;
      minlist.print("std::numeric_limits<unsigned>::max()");
      for(auto n : generator.graph.in)
        minlist = wrt().print("std::min($2, size_in_$1 - pos_in_$1)", n->data.get_inout_pos(), minlist);
      for(auto n : generator.graph.out)
        minlist = wrt().print("std::min($2, size_out_$1 - pos_out_$1)", n->data.get_inout_pos(), minlist);

      //alignment code
      wrt alignment;
      if(generator.graph.in.empty() || generator.graph.out.empty())
      {
        error("Box without inputs or outputs passed. Cant handle alignment for such boxes. (There does not seem to be a reason to write nontrivial code for cases which should never happen)");
      }
      else
      {
        alignment.print("$falign", granularity, generator.graph.in.front()->data.get_inout_pos(), generator.graph.in.front()->data.get_inout_pos());
        alignment.print("/*check alignment*/;");
        for(auto n : generator.graph.in)
          alignment.print("aligned &= align_offset == pos_in_$2 % $1;", granularity, n->data.get_inout_pos());
        for(auto n : generator.graph.out)
          alignment.print("aligned &= output_offset == pos_out_$2 % $1;", granularity, n->data.get_inout_pos());
      }

      //generate actual code
      wrt code_simple;
      generator.generate(1, code_simple);
      
      wrt code_unaligned;
      auto pcu = make_shared<tagmaster_default>("","unalignedio,universal","","");
      generator.generate(granularity, code_unaligned, pcu);
      wrt code_aligned;
      generator.generate(granularity, code_aligned, make_shared<tagmaster_default>("","alignedio,universal","",""));

      wrt code_shifted;
      for(int i = 1; i < granularity; ++i)
      {
        SET("alignoffset", wrt().print("$1", i).write_str());
        wrt tmpcode;
        generator.generate(granularity, tmpcode, make_shared<tagmaster_default>("","shiftedio,universal","",""));
        code_shifted.print("$fcase", tmpcode, granularity);
      }

      wrt code_preload;
      /*TODO*/
      SET("alignoffset", "(align_offset + output_offset)");
      shared_ptr<tagmaster_default> tq = make_shared<tagmaster_default>("","preloadio,universal","","");
      shared_ptr<tagmaster_default> tp = make_shared<tagmaster_default>("preloadio","","","");
      generator.generate(granularity, code_preload, tq, tp, tp);

      wrt code;
      code.print("$fcode", code_aligned, code_shifted, code_unaligned, code_simple, code_preload, granularity);

      //increment counters
      wrt inc;
      for(auto n : generator.graph.in)
        inc.print("pos_in_$1 += batch_size;", n->data.get_inout_pos());
      for(auto n : generator.graph.out)
        inc.print("pos_out_$1 += batch_size;", n->data.get_inout_pos());

      //send envelopes
      wrt send;
      for(auto n : generator.graph.out)
      {
        n->data.op->get_type_string(1, type_string);
        send.print("$fsend", n->data.get_inout_pos(), type_string);
      }

      //put everything together
      wrt box;
      box.print("$fbox", name, ilist, olist,decl, envelopes, minlist, alignment, code, inc, send);

      return box;

    }
}

#endif

