
#ifndef aliasenv_SIMU_GUARD
#define aliasenv_SIMU_GUARD

#include "defines.h"
#include "writer.h"
#include "aliasenv_simu.h"
#include <map>

namespace ctb
{
  /** 
   * for general aliasenv documentation see the aliasenv_generator.h 
   *
   * This specific class relizes generation environment for a simu testing output. That is this class drives generation of a simu method test which somehow tests whether internal instruction generators work as expected without providing full input/output syntax of bobox. When writting this, bobox is still unavailable to the tester and thus it makes sense to have 'something' which can be compiled.
   *
   * Specifically we use internal aliases defined by aliasenv_generator and on top we define expansions for 'input' and 'output' aliases, which are output-environment dependent and thus are defined neither as part of instruction table nor part of the internal generator. Then we load into memory templates from file and direct their composition. During this process we use also an actual generator to generate actual 'isntruction code'.
   * */
  class aliasenv_simu : public aliasenv_generator
  {
    protected:
      typedef map<string, string> aliastab_t;
      static aliastab_t aliases;
      static void init();
      template <class G> static writer<aliasenv_simu> generate_body(int m,  G& generator, string name);
    public:
      typedef language_cpp language;
      static string get_name();
      static string alias(const string& a, bool* s = NULL);
      template <class G> static writer<aliasenv_simu> generate(int m,  G& generator, string name);
  } ;

  map<string, string> aliasenv_simu::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  string aliasenv_simu::get_name()
  {
    return "simu";
  }

  string aliasenv_simu::alias(const string& a, bool* s)
  {
    auto itr = aliases.find(a);
    if(itr == aliases.end())
      return aliasenv_generator::alias(a, s);
    if(s != NULL)
      *s = true;
    return itr->second;
  }


  void aliasenv_simu::init()
  {
    static bool initialized = false;
    if(initialized)
      return;

    aliasenv_generator::init();

    ADD("input", "data_in_$cindex[pos_in_$cindex+j+$iindex]");
    ADD("inputg", "data_in_$cindex,pos_in_$cindex+j+$iindex");
    ADD("output", "data_out_$cindex[pos_out_$cindex+j+$iindex]");
    ADD("outputg", "data_out_$cindex,pos_out_$cindex+j+$iindex");

    ADD("fdeclin",  writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/simu_decl_in.h")));
    ADD("fdeclcont",  writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/simu_decl_cont.h")));
    ADD("fdeclout", writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/simu_decl_out.h")));
    ADD("fenvin",   writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/simu_env_in.h")));
    ADD("fenvout",  writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/simu_env_out.h")));
    ADD("fbox",     writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/simu_box.h")));
    ADD("fbody",    writer<aliasenv_generator>::from_file(string().append(exec_path).append("templates/simu_body.h")));

    initialized = true;
  }

  template <class G>
    writer<aliasenv_simu> aliasenv_simu::generate(int granularity, G& generator, string name)
    {
#ifdef TMPTEST
      if(granularity > 16)
        granularity = 16;
#endif

      init();
      typedef writer<aliasenv_simu> wrt;

      wrt decl;
      string type_string;
      for( auto n : generator.graph.out)
      {
        n->data.op->get_type_string(1, type_string);
        decl.print("$fdeclcont", n->data.get_inout_pos(), type_string);
      }

      writer<aliasenv_simu> bodies;
      writer<aliasenv_simu> calls;
      for(int i = 1; i <= granularity; i *= 2)
      {
        bodies += generate_body(i, generator, name);
        calls.print("test_body$1();", i);
      }

      writer<aliasenv_simu> box;
      box.print("$fbox", decl, bodies, calls) ;
      return box;
    }

  template <class G>
    writer<aliasenv_simu> aliasenv_simu::generate_body(int granularity, G& generator, string name)
    {
      typedef multicontA<writer<aliasenv_simu>> wrt;
      auto opts = generator.option_struct();

      wrt ilist;
      wrt olist;

      wrt dummy;
      wrt decl;
      string type_string;
      for( auto n : generator.graph.in)
      {
        n->data.op->get_type_string(1, type_string);
        decl.print("$fdeclin", n->data.get_inout_pos(), type_string, n->data.op->out_type, n->id);
      }
      for(auto n : generator.graph.out)
      {
        n->data.op->get_type_string(1, type_string);
        decl.print("$fdeclout", n->data.get_inout_pos(), type_string, n->data.op->out_type, n->id);
      }

      wrt envelopes;
      for(auto n : generator.graph.in)
      {
        n->data.op->get_type_string(1, type_string);
        envelopes.print("$fenvin", n->data.get_inout_pos(), type_string, n->data.op->out_type);
      }
      for(auto n : generator.graph.out)
      {
        n->data.op->get_type_string(1, type_string);
        envelopes.print("$fenvout", n->data.get_inout_pos(),  type_string, n->data.op->out_type, granularity, n->id);
      }

      wrt minlist;
      minlist.print("std::numeric_limits<unsigned>::max()");
      for(auto n : generator.graph.in)
        minlist = wrt().print("std::min($2, size_in_$1 - pos_in_$1)", n->data.get_inout_pos(), minlist);
      for(auto n : generator.graph.out)
        minlist = wrt().print("std::min($2, size_out_$1 - pos_out_$1)", n->data.get_inout_pos(), minlist);

      wrt code;
      generator.generate(granularity, code, opts);

      wrt code2;
      if(granularity != 1)
        generator.generate(1, code2, opts);

      wrt inc;
      for(auto n : generator.graph.in)
        inc.print("pos_in_$1 += batch_size;", n->data.get_inout_pos());
      for(auto n : generator.graph.out)
        inc.print("pos_out_$1 += batch_size;", n->data.get_inout_pos());

      wrt box;
      box.print("$fbody", name, ilist, olist,decl, envelopes, minlist, code, code2, inc, granularity);

      return box;
    }
};

#endif

