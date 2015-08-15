
#ifndef aliasenv_BOBOX_GUARD
#define aliasenv_BOBOX_GUARD

#include "writer.h"
#include "aliasenv_bobox.h"
#include <map>

namespace ctb
{
  /** see the aliasenv_generator */
  class aliasenv_bobox : public aliasenv_generator
  {
    protected:
      typedef std::map<std::string, std::string> aliastab_t;
      static aliastab_t aliases;
      static void init();
    public:
      typedef language_cpp language;
      static std::string alias(const std::string& a, bool* s = NULL);
      static std::string get_name();
      template <class G> static writer<aliasenv_bobox> generate(int m,  G& generator, std::string name);
  } ;

  std::map<std::string, std::string> aliasenv_bobox::aliases;

#define ADD(a,b) aliases.insert(aliastab_t::value_type(a,b))

  std::string aliasenv_bobox::get_name()
  {
    return "bobox";
  }

  std::string aliasenv_bobox::alias(const std::string& a, bool* s)
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

    ADD("fdeclin",  writer<aliasenv_generator>::from_file("templates/bobox_decl_in.h"));
    ADD("fdeclout", writer<aliasenv_generator>::from_file("templates/bobox_decl_out.h"));
    ADD("fenvin",   writer<aliasenv_generator>::from_file("templates/bobox_env_in.h"));
    ADD("fenvout",  writer<aliasenv_generator>::from_file("templates/bobox_env_out.h"));
    ADD("fbox",     writer<aliasenv_generator>::from_file("templates/bobox_box.h"));
    ADD("fsend",    writer<aliasenv_generator>::from_file("templates/bobox_send.h"));

    initialized = true;
  }


  template <class G>
    writer<aliasenv_bobox> aliasenv_bobox::generate(int max_granularity, G& generator, std::string name)
    {
      typedef writer<aliasenv_bobox> wrt;
      /*
         init();
         wrt ilist;
         for(auto n : generator.inlist())
         ilist.push("input_list_$1, $1", n->get_inout_pos());
         ilist.list_concat(",");

         wrt olist;
         for(auto n : generator.outlist())
         olist.push("output_list_$1, $1", n->get_inout_pos());
         olist.list_concat(",");

         wrt decl;
         for(auto n : generator.inlist())
         decl.print("$fdeclin", n->get_inout_pos(), n->getop()->get_type_string());
         for(auto n : generator.outlist())
         decl.print("$fdeclout", n->get_inout_pos(), n->getop()->get_type_string());

         wrt envelopes;
         for(auto n : generator.inlist())
         envelopes.print("$fenvin", n->get_inout_pos(), n->getop()->get_type_string());
         for(auto n : generator.outlist())
         envelopes.print("$fenvout", n->get_inout_pos(),  n->getop()->get_type_string());

         wrt minlist;
         minlist.print("std::numeric_limits<unsigned>::max()");
         for(auto n : generator.inlist())
         minlist = wrt().print("std::min($2, size_in_$1 - pos_in_$1)", n->get_inout_pos(), minlist);
         for(auto n : generator.outlist())
         minlist = wrt().print("std::min($2, size_out_$1 - pos_out_$1)", n->get_inout_pos(), minlist);

         wrt code;
         generator.generate(max_granularity, code);

         wrt inc;
         for(auto n : generator.inlist())
         inc.print("pos_in_$1 += batch_size;", n->get_inout_pos());
         for(auto n : generator.outlist())
         inc.print("pos_out_$1 += batch_size;", n->get_inout_pos());

         wrt send;
         for(auto n : generator.outlist())
         send.print("$fsend", n->get_inout_pos(), n->getop()->get_type_string());

         wrt box;
         box.print("$fbox", name, ilist, olist,decl, envelopes, minlist, code, inc, send);
         */

      return wrt();

    }
}

#endif
