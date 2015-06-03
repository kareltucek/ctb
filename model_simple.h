
#ifndef MODEL_SIMPLE_GUARD
#define MODEL_SIMPLE_GUARD

#include "writer.h"
#include "model_simple.h"
#include <map>

namespace ctb
{
  /** see the model_empty */
  class model_simple : public model_empty
  {
    protected:
      typedef std::map<std::string, std::string> aliastab_t;
      static aliastab_t aliases;
      static void init();
    public:
      static std::string alias(const std::string& a);
      template <class G> static writer<model_simple> generate(int m,  G& generator, std::string name);
  } ;

  std::map<std::string, std::string> model_simple::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  std::string model_simple::alias(const std::string& a)
  {
    auto itr = aliases.find(a);
    if(itr == aliases.end())
      return model_empty::alias(a);
    return itr->second;
  }


  void model_simple::init()
  {
    static bool initialized = false;
    if(initialized)
      return;

    model_empty::init();

    ADD("input", "data_in_$cindex[pos_in_$cindex+j]");
    ADD("output", "data_out_$cindex[pos_out_$cindex+j]");

    ADD("fdeclin",  writer<model_empty>::from_file("templates/simple_decl_in.h"));
    ADD("fdeclout", writer<model_empty>::from_file("templates/simple_decl_out.h"));
    ADD("fenvin",   writer<model_empty>::from_file("templates/simple_env_in.h"));
    ADD("fenvout",  writer<model_empty>::from_file("templates/simple_env_out.h"));
    ADD("fbox",     writer<model_empty>::from_file("templates/simple_box.h"));

    initialized = true;
  }


  template <class G>
    writer<model_simple> model_simple::generate(int granularity, G& generator, std::string name)
    {
      init();
      typedef writer<model_simple> wrt;

      wrt ilist;
      wrt olist;

      wrt decl;
      for( auto n : generator.graph->in.r())
      {
        n->data->op->imbue_width(1);
        decl.print("$fdeclin", n->data->get_inout_pos(), n->data->op->get_type_string());
      }
      for(auto n : generator.graph->out.r())
      {
        n->data->op->imbue_width(1);
        decl.print("$fdeclout", n->data->get_inout_pos(), n->data->op->get_type_string());
      }

      wrt envelopes;
      for(auto n : generator.graph->in.r())
        envelopes.print("$fenvin", n->data->get_inout_pos(), n->data->op->get_type_string());
      for(auto n : generator.graph->out.r())
        envelopes.print("$fenvout", n->data->get_inout_pos(),  n->data->op->get_type_string());

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

