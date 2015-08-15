
#ifndef aliasenv_SIMPLE_GUARD
#define aliasenv_SIMPLE_GUARD

#include "writer.h"
#include "aliasenv_simple.h"
#include <map>

namespace ctb
{
  /** 
   * for general aliasenv documentation see the aliasenv_generator.h 
   *
   * This specific class relizes generation environment for a simple testing output. That is this class drives generation of a simple method test which somehow tests whether internal instruction generators work as expected without providing full input/output syntax of bobox. When writting this, bobox is still unavailable to the tester and thus it makes sense to have 'something' which can be compiled.
   *
   * Specifically we use internal aliases defined by aliasenv_generator and on top we define expansions for 'input' and 'output' aliases, which are output-environment dependent and thus are defined neither as part of instruction table nor part of the internal generator. Then we load into memory templates from file and direct their composition. During this process we use also an actual generator to generate actual 'isntruction code'.
   * */
  class aliasenv_simple : public aliasenv_generator
  {
    protected:
      typedef std::map<std::string, std::string> aliastab_t;
      static aliastab_t aliases;
      static void init();
    public:
      typedef language_cpp language;
      static std::string get_name();
      static std::string alias(const std::string& a, bool* s = NULL);
      template <class G> static writer<aliasenv_simple> generate(int m,  G& generator, std::string name);
  } ;

  std::map<std::string, std::string> aliasenv_simple::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  std::string aliasenv_simple::get_name()
  {
    return "simple";
  }

  std::string aliasenv_simple::alias(const std::string& a, bool* s)
  {
    auto itr = aliases.find(a);
    if(itr == aliases.end())
      return aliasenv_generator::alias(a, s);
    if(s != NULL)
      *s = true;
    return itr->second;
  }


  void aliasenv_simple::init()
  {
    static bool initialized = false;
    if(initialized)
      return;

    aliasenv_generator::init();

    ADD("input", "data_in_$cindex[pos_in_$cindex+j]");
    ADD("output", "data_out_$cindex[pos_out_$cindex+j]");

    ADD("fdeclin",  writer<aliasenv_generator>::from_file(std::string().append(exec_path).append("templates/simple_decl_in.h")));
    ADD("fdeclout", writer<aliasenv_generator>::from_file(std::string().append(exec_path).append("templates/simple_decl_out.h")));
    ADD("fenvin",   writer<aliasenv_generator>::from_file(std::string().append(exec_path).append("templates/simple_env_in.h")));
    ADD("fenvout",  writer<aliasenv_generator>::from_file(std::string().append(exec_path).append("templates/simple_env_out.h")));
    ADD("fbox",     writer<aliasenv_generator>::from_file(std::string().append(exec_path).append("templates/simple_box.h")));

    initialized = true;
  }


  template <class G>
    writer<aliasenv_simple> aliasenv_simple::generate(int granularity, G& generator, std::string name)
    {
      init();
      typedef writer<aliasenv_simple> wrt;

      wrt ilist;
      wrt olist;

      wrt decl;
      std::string type_string;
      for( auto n : generator.graph->in.r())
      {
        n->data->op->get_type_string(1, type_string);
        decl.print("$fdeclin", n->data->get_inout_pos(), type_string);
      }
      for(auto n : generator.graph->out.r())
      {
        n->data->op->get_type_string(1, type_string);
        decl.print("$fdeclout", n->data->get_inout_pos(), type_string);
      }

      wrt envelopes;
      for(auto n : generator.graph->in.r())
      {
        n->data->op->get_type_string(1, type_string);
        envelopes.print("$fenvin", n->data->get_inout_pos(), type_string);
      }
      for(auto n : generator.graph->out.r())
      {
        n->data->op->get_type_string(1, type_string);
        envelopes.print("$fenvout", n->data->get_inout_pos(),  type_string);
      }

      wrt minlist;
      minlist.print("std::numeric_limits<unsigned>::max()");
      for(auto n : generator.graph->in.r())
        minlist = wrt().print("std::min($2, size_in_$1 - pos_in_$1)", n->data->get_inout_pos(), minlist);
      for(auto n : generator.graph->out.r())
        minlist = wrt().print("std::min($2, size_out_$1 - pos_out_$1)", n->data->get_inout_pos(), minlist);

      wrt code;
      generator.generate(granularity, code);

      wrt code2;
      generator.generate(1, code2);

      wrt inc;
      for(auto n : generator.graph->in.r())
        inc.print("pos_in_$1 += batch_size;", n->data->get_inout_pos());
      for(auto n : generator.graph->out.r())
        inc.print("pos_out_$1 += batch_size;", n->data->get_inout_pos());

      wrt box;
      box.print("$fbox", name, ilist, olist,decl, envelopes, minlist, code, code2, inc, granularity);

      return box;
    }
};

#endif

