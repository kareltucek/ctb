
#ifndef GENERATOR_GUARD
#define GENERATOR_GUARD

#include "defines.h"
#include "graph.h"
#include "instructions.h"
#include "aliasenv_maker.h"
#include "aliasenv_generator.h"
#include <functional>

namespace ctb
{
  /** General description
   *  -------------------
   *  Generator is an extension to the graph structure which serves for generating actual instruction code from a graph. Since the motivation of all the written functions is totaly clear to me, the question of 'what should be documented here' is quite unclear on the other hand. Please ask for comments at adress kareltucek centrum cz.
   *
   *  flags are commented in instab documentation (instructions.h)
   *
   *  Template arguments
   *  ------------------
   *  - T - traits
   *  - IT - instruction table type
   *  */

  template <class T, class IT>
    class generator 
    {
      private:
        class data_t;
        class datae_t;
      public:
        typedef graph_general<data_t, datae_t, typename T::vid_t, true> graph_t;
        typedef typename graph_t::node_t node_t;
        typedef typename graph_t::edge_t edge_t;
        typedef typename T::opid_t opid_t;
        typedef typename T::opid_t id_t;
        typedef typename T::vid_t vid_t;
        typedef typename T::tid_t tid_t;
        struct output_options{
          bool once;
          bool global_once;
          output_options() {once = false; global_once = false;};
        };
        MAKE(output_options);
      private:
        typedef typename IT::operation_t op_t;

        class data_t
        {
          private:
            typedef map<int, writer<aliasenv_generator> > acces_map_t;
            friend class generator;

            node_t* me;
            acces_map_t acces_map;
            map<string,string> params;

            static int newid(bool reset);
            string newname(string tag) ;
            template <class W> writer<aliasenv_generator> get_acces(int width, int gran, W& w, bool c);
            void check_structure(const vector<tid_t>& intypes, const vector<tid_t>& outtypes, bool checkin, bool checkout);
            void set_types(const vector<tid_t>& intypes, const vector<tid_t>& outtypes);
            void infer_type(const IT& it)  ;
            void clear_type();
          public:
            const op_t* op;
            id_t opid;

            data_t( node_t* me, const typename IT::operation_t* o, id_t opi, const map<string,string>& params = {});
            template <class W> void generate(graph_t& parent, int granularity, imp_contB<W>& w,  imp_contB<output_options>& , bool c);
            string get_param(const string&) const;
            void set_param(const string&, const string&);
            vector<tid_t> get_typespec() const; /** returns type specification infered from graph structure (i.e. otuput types of nodes connected to the inputs)*/
        };

        class datae_t
        {
          protected:
          public:
            edge_t* me;
            tid_t type;
            string dbglab;

            datae_t(edge_t* m): me(m), type() {};
        };

        static node_t* lastnode; //for error throws
        bool compiletest; /*abbreviated as plain 'c'*/
        static void dump(graph_t& g);
      public:

        IT& instab;
        graph_t graph;

        generator( IT& i);
        void set_instab( IT& i);

        //graph API
        template <typename...L> typename graph_t::node_t* add_vert(vid_t v, id_t op, L... p) ;
        template <typename J> void rm_vert(J v);
        template <typename J, typename K> void add_edge(J aid, K bid, int b_argpos, int a_argpos, int layer = 0, string dbglab = "") ;
        template <typename J, typename K> void connect_as(J v, K as, bool inputs = true, bool outputs = true);
        template <typename J> node_t* get_vert(J v)  ;
        void rm_edge(edge_t* e);
        template <bool Inverted = false> void foreach(function<void(node_t*)> f); /** this is an overload of crawl for topological search, may be also abbreviated as 'do f for each vertex'*/


        //generator related API
        template <class W> void generate(int granularity, imp_contB<W>& w,  imp_contB<output_options>& wd, shared_ptr<taghandler_base> p = NULL, shared_ptr<taghandler_base> q = NULL, shared_ptr<taghandler_base> s = NULL) ; /**TODO document this!!!*/
        template <class W> void generate_partition(int partition, int granularity, imp_contB<W>& w,  imp_contB<output_options>& wd, shared_ptr<taghandler_base> p = NULL, shared_ptr<taghandler_base> q = NULL, shared_ptr<taghandler_base> s = NULL) ; /**TODO document this!!!*/
        int get_broadest(int upperbound = 10000000) ;
        imp_contB<output_options> option_struct();
        int partition_count();
        bool partition_is_topo_max(int p);
        bool partition_is_topo_min(int p);

        //ctb related API
        void set_compiletest(bool);
        template<template <typename ...> class L, typename...P> void transform(P...params) ;


        //others
        void dump_visual();
        void clear();
        void reset();
        void update_types(); /** This updates infered data types and checks the graph structure */
        void update(); /** In case instruction table is reloaded the operation pointers are no longer valid. This function updates them.*/

    };

  template <class T, class IT>
    typename generator<T,IT>::node_t* generator<T,IT>::lastnode = NULL;

  template <class T, class IT>
    bool generator<T,IT>::partition_is_topo_min(int p)  
    {
      return !graph.factor.get_vert(p)->in.empty();
    }

  template <class T, class IT>
    bool generator<T,IT>::partition_is_topo_max(int p)  
    {
      return !graph.factor.get_vert(p)->out.empty();
    }

  template <class T, class IT>
    void generator<T,IT>::update_types()  
    {
      try
      {
        //cout << "UPDATING TYPES" << endl;
        foreach([&](node_t* n){n->data.clear_type();});
        foreach([&](node_t* n){n->data.infer_type(instab);});
        generator::lastnode = NULL;
      }
      catch (error_struct& err)
      {
        string nodename;
        if(generator::lastnode != NULL)
        {
          nodename = generator::lastnode->id + ":" + generator::lastnode->data.opid;
        }
        string my = string("while processing node ") + nodename + "\n    " + err.first;

        if(allow_graphs)
        {
          cerr << my << endl;
          cerr << "showing corresponding node:" << endl;
          dump(graph);
        }

        generator::lastnode = NULL;

        if(err.second || allow_graphs)
          error(my, true);
        else
          warn(my);
      }
    }

  template <class T, class IT>
    void generator<T,IT>::data_t::clear_type()  
    {
      for(auto* e : me->out)
        e->data.type = "";
      for(auto* e : me->in)
        e->data.type = "";
    }

  template <class T, class IT>
    void generator<T,IT>::data_t::set_types(const vector<tid_t>& intypes, const vector<tid_t>& outtypes)  
    {
      for(int l = 0; l < me->in.get_layercount(); l++)
        for(auto* e : me->in.get_layer(l))
          if(e->to_pos < intypes.size())
            if(e->data.type == "")
              e->data.type = intypes[e->to_pos];
      for(int l = 0; l < me->out.get_layercount(); l++)
        for(auto* e : me->out.get_layer(l))
          if(e->from_pos < outtypes.size())
            if(e->data.type == "")
              e->data.type = outtypes[e->from_pos];
    }

  template <class T, class IT>
    void generator<T,IT>::data_t::check_structure(const vector<tid_t>& intypes, const vector<tid_t>& outtypes, bool checkin, bool checkout)  
    {
      string ctx = string("at node ") + me->id + ":" + opid + " of expected type signature '" + ctb::l_to_string(intypes) + "'(in), '" + ctb::l_to_string(outtypes) + "'(out)\n    ";

      if(checkin)
      {
        for(int l = 0; l < me->in.get_layercount(); l++)
        {
          for(auto* e : me->in.get_layer(l))
          {
            //cout << "====processing edge >> " << e->from->id << " - " << e->layer << " -> " << e->to->id << endl;
            if(e->to_pos >= intypes.size())
            {
              warn(ctx + "Incoming edge at " + ctb::to_string(e->to_pos) + " from " + e->from->id + " not expected by type signature encountered at node " + me->id);
              continue;
            }
            if(e->data.type != intypes[e->to_pos])
              warn(ctx + "Types do not match infered types at input edge '" + e->from->id + " -> " + e->to->id + "'; got '" + e->data.type + "'");
          }
        }
        for(int i = 0; i < intypes.size(); i++)
          if(me->in_at(i, false, true) == NULL)
            warn(ctx + "Incoming edge which was expected by type signature not found at position " + ctb::to_string(i) + " of node " + me->id);
      }
      if(checkout)
      {
        for(int l = 0; l < me->out.get_layercount(); l++)
        {
          for(auto* e : me->out.get_layer(l))
          {
            //cout << "====processing edge << " << e->from->id << " - " << e->layer << " -> " << e->to->id << endl;
            if(e->from_pos >= outtypes.size())
            {
              warn(ctx + "Outgoing edge at " + ctb::to_string(e->from_pos) + " to " + e->to->id + " not expected by type signature encountered at node " + me->id);
              continue;
            }
            if(e->data.type != outtypes[e->from_pos])
              warn(ctx + "Types do not match infered types at output edge '" + e->from->id + " -> " + e->to->id + "'; got " + e->data.type + "'");
          }
        }
        for(int i = 0; i < outtypes.size(); i++)
          if(me->out_at(i, false, true) == NULL)
            warn(ctx + "Outgoing edge which was expected by type signature not found at position " + ctb::to_string(i) + " at node " + me->id);
      }
    }

  template <class T, class IT>
    void generator<T,IT>::data_t::infer_type(const IT& it)  
    {
      generator::lastnode = this->me;
      if(op->is(fNOOP))
        return;
      if(!op->is(fEXPANSION))
      {
        vector<vid_t> outtypes;
        if(!op->is(fOUTPUT | fEFOUTPUT | fDEBUG))
          outtypes.push_back(op->out_type);
        set_types(op->in_types, outtypes);
        check_structure(op->in_types, outtypes, !op->is(fEFINPUT), !op->is(fEFOUTPUT));
      }
      else
      {
        auto typesig = this->get_typespec();
        bool ambig = false;
        const auto& exp = it.find_expansion(op->expansions, typesig, &ambig);
        if(ambig)
          warn(string("Type inference provided ambiguous results at node ") + me->id);
        set_types(exp.in_types, exp.out_types);
        check_structure(exp.in_types, exp.out_types, true, true);
      }
    }


  template <class T, class IT>
    template <bool Inverted>
    void generator<T,IT>::foreach(function<void(node_t*)> f)  
    {
      graph.template crawl_topological<Inverted>([&](node_t* n){f(n);});
    }

  template <class T, class IT>
    void generator<T,IT>::rm_edge(edge_t* e)  
    {
      graph.rm_edge(e);
    }

  template <class T, class IT>
    template <typename J>
    typename generator<T,IT>::node_t* generator<T,IT>::get_vert(J v)  
    {
      return graph.get_vert(v);
    }


  template <class T, class IT>
    template <typename J, typename K>
    void generator<T,IT>::connect_as(J v, K as, bool inputs, bool outputs)  
    {
      graph.connect_as(v, as, inputs, outputs);
    }

  template <class T, class IT>
    template <typename J>
    void generator<T,IT>::rm_vert(J vert)  
    {
      graph.rm_vert(vert);
    }


  template <class T, class IT>
    template <typename J, typename K>
    void generator<T,IT>::add_edge(J aid, K bid, int b_argpos, int a_argpos, int layer, string dbglab)  
    {
      auto* e = graph.add_edge(aid, bid, b_argpos, a_argpos, layer);
      e->data.dbglab = dbglab;
    }

  typedef generator<traits, instruction_table_default> generator_default;

  template <class T, class IT>
    imp_contB<typename generator<T,IT>::output_options> generator<T,IT>::option_struct()
    {
      return imp_contB<output_options>();
    }

  template <class T, class IT>
    template<template <typename ...> class L, typename...P> 
    void generator<T,IT>::transform(P...params)
    {
      L<generator<T,IT>> l;
      l.transform(*this ,params...);
      update_types();
    }

  template <class T, class IT>
    void generator<T,IT>::update()
    {
      for(auto itr = graph.verts.cbegin(); itr != graph.verts.cend(); ++itr)
      {
        data_t& n = itr->second->data;
        n.op = &instab.dec(n.opid);
        /*
           string a = n.opid;
           instab->dec(a);
           instab->dec(n.opid);
           */
      }
      //graph.crawl_topological([&](typename graph_t::node_t* n){n->data.op = instab->dec(n->data->opid);});
      /*
         for( auto op : graph->verts )
         {
         string id = op.second->data->opid;
         op.second->data.op =
         instab->dec(id);
         }
         */
    }

  template <class T, class IT>
    void generator<T,IT>::set_compiletest(bool c)
    {
      compiletest = c;
    }

  template <class T, class IT>
    void generator<T,IT>::reset()
    {
      data_t::newid(true);
    }

  template <class T, class IT>
    void generator<T,IT>::clear()  
    {
      graph.clear();
      generator::lastnode = NULL;
    }

  template <class T, class IT>
    template <typename...L> typename generator<T,IT>::graph_t::node_t* generator<T,IT>::add_vert(vid_t v, id_t op, L... p)  
    {
      const op_t* ptr;
      try
      {
        ptr = &instab.dec(op);
      }
      RETHROW(string("while adding vertex '") + ctb::to_string(v) +"' of type '" + ctb::to_string(op) + "'");
      return graph.add_vert(v, ptr->is(fINPUT), ptr->is(fOUTPUT), ptr, op, p...);
    }

  template <class T, class IT>
    generator<T,IT>::data_t::data_t( node_t* m, const typename IT::operation_t* o, id_t opi, const map<string, string>& p) : me(m), opid(opi), acces_map(), op(o), params(p)
    {
    }

  template <class T, class IT>
    void generator<T,IT>::set_instab( IT& i)
    {
      instab = i; 
    }

  template <class T, class IT>
    generator<T,IT>::generator( IT & i) : instab(i), graph(), compiletest(false)
  {
  }

  template <class T, class IT>
    int generator<T,IT>::partition_count()
    {
      graph.update_factor();
      return graph.factor.size();
    }

  template <class T, class IT>
    template <class W>
    void generator<T,IT>::generate_partition(int partition, int packsize, imp_contB<W>& w,  imp_contB<output_options>& opts, shared_ptr<taghandler_base> ts, shared_ptr<taghandler_base> tp, shared_ptr<taghandler_base> to)
    {
      if(graph.out.empty())
        error( "graph is empty");
      if(ts!=NULL)
        instab.add_tags(ts,gSELECT);
      if(tp!=NULL)
        instab.add_tags(tp,gPRINT);
      if(to!=NULL)
        instab.add_tags(to,gONCE);
      instab.update_tags();
      graph.update_factor();

      node_t* some_vert;
      try
      {
        some_vert = *graph.factor.verts[partition]->data.vertices.begin();
      }
      RETHROW(string("Could not access any vertex of required partition ") + ctb::to_string(partition) + " note that partition count is " + ctb::to_string(partition_count()) );

      verbose(string() + "generating partition " + ctb::to_string(partition) + " for packsize " + ctb::to_string(packsize));
      some_vert->crawl_topological([&](node_t* n) {n->data.generate(graph, packsize, w, opts, compiletest); });
      verbose(string() + "    partition generated");
      generator::lastnode = NULL;

      if(to!=NULL)
        instab.rm_tags(to,gONCE);
      if(tp!=NULL)
        instab.rm_tags(tp,gPRINT);
      if(ts!=NULL)
        instab.rm_tags(ts,gSELECT);
    }

  template <class T, class IT>
    template <class W>
    void generator<T,IT>::generate(int packsize, imp_contB<W>& w,  imp_contB<output_options>& opts, shared_ptr<taghandler_base> ts, shared_ptr<taghandler_base> tp, shared_ptr<taghandler_base> to)
    {
      data_t::newid(true); //resets ids
      for(int i = 0; i < partition_count(); ++i)
        generate_partition(i, packsize, w, opts, ts, tp, to);
    }

  template <class T, class IT>
    template <class W>
    void generator<T,IT>::data_t::generate(graph_t& parent, int granularity, imp_contB<W>&w, imp_contB<output_options>& opts, bool c)
    {
      generator<T,IT>::lastnode = me;
      aliasenv_generator::setparammap(&params);
      if(op->is(fNOOP))
        return;
      try
      {
        W empty;
        int myin, myout;
        int classid = me->classid;
        int mygran = op->is(fDEBUG) ? me->in_at(0)->from->data.op->get_max_width(granularity, &myin, &myout) : op->get_max_width(granularity,&myin, &myout);

        if(mygran == 0)
          error( string("suitable width not found!"));
        if(granularity % mygran != 0)
          error( string("granularities are relatively prime!"));
        if(myin != myout)
          error( string("asymetric instructions not supported"));
        if(me->in.size() != op->in_types.size())
        {
          //cout << op->in_types[0] << endl;
          error( string("count of input nodes does not match operation specification, insize is ") + ctb::to_string(me->in.size()) +", expected is " + ctb::to_string(op->in_types.size()));
        }
        for(int i = 0; i < me->in.size(); ++i)
        {
          auto e = me->in[i];
          if(e->from->data.op->out_type != op->in_types[e->to_pos])
          {
            error( string("argument ") + to_string(e->to_pos) + " does not match defined input type: got '" + e->from->data.op->out_type + "' wanted '" + op->in_types[e->to_pos] + "'");
          }
        }
        //op->imbue_width(mygran);
        acces_map.clear();
#define ARG(a) (me->in_at(a-1, false) != NULL ? W().print(me->in_at(a-1, false)->from->data.get_acces(myin, granularity, w, c), i*myout) : empty)
        W acces({newname(print("w$1",myout))});
        acces_map.insert(acces_map_t::value_type(myout, acces));
        string type_string;
        string op_c;
        typename IT::operation_t::ccode_cont_t  op_cc;
        size_t printability; 
        if(op->is(fOUTPUT | fEFOUTPUT))
        {
          try { op->get_type_string(mygran, type_string); }
          catch(...) {};
        }
        else
          op->get_type_string(mygran, type_string);
        bool found = op->get_op_string(mygran, op_c, op_cc, printability);
        if(op->is(fEXPANSION))
          error( string("Found expansion during generation. All expansions have to be removed prior to generation by means of some (probably custom) graph transformation."));
        if(found && op_c.empty() && op_cc.empty())
          warn(string("instruction code and custom code are both empty for ").append(opid));
        /*even for unprintable code we want to consume ids -> cannot skip most of this function*/
        W basename = W().print(acces,0);
        for(int i = 0; i < granularity/myin && i < printability; i++) 
        {
          W name = W().print(acces,i*myout);
          if(i!=0 && c) //print empty declaration in case we want compile test
          {
            w.print("$declcode",     type_string, name, basename, ""  ,  myin*i, myin*i, 0, classid, ARG(1), ARG(2), ARG(3));
          }
          else
          {
            if(!op_c.empty()) //neccessary!
            {
              if(op->is(fINPUT | fEFINPUT)) 
                w["default"].print("$inputcode" , type_string, name, basename, op_c, myin*i, myin*i, 0, classid,ARG(1), ARG(2), ARG(3));
              else if(op->is(fOUTPUT | fEFOUTPUT))
                w["default"].print("$outputcode", type_string, name, basename, op_c, myin*i, myin*i, 0, classid,ARG(1), ARG(2), ARG(3));
              else if(op->is(fDEBUG)) //same as output
                w["default"].print("$outputcode", type_string, name, basename, op_c, myin*i, myin*i, 0, classid,ARG(1), ARG(2), ARG(3));
              else
                w["default"].print("$innercode" , type_string, name, basename, op_c, myin*i, myin*i, 0, classid,ARG(1), ARG(2), ARG(3));
            }

            for(int wi = 0; wi < op_cc.size(); wi++)
            {
              string wname = op_cc[wi].name;
              if(opts[wname].once && i != 0)
              {
                continue;
              }
              else if(opts[wname].global_once && !w[wname].empty())
              {
                continue;
              }
              else 
              {
                w[wname].print(op_cc[wi].code,           type_string, name, basename, ""  , myin*i, myin*i, 0, classid, ARG(1), ARG(2), ARG(3));
              }
            }
          }
        }
      }
      catch (error_struct& err)
      {
        stringstream s;
        string my = string("while processing node ") + me->id + " with opcode " + opid + "\n    " + err.first;

        if(allow_graphs)
        {
          cerr << my << endl;
          cerr << "showing malfunctioning node:" << endl;
          dump(parent);
        }

        if(err.second || allow_graphs)
          error(my, true);
        else
          warn(my);
      }
      aliasenv_generator::setparammap(NULL);
    }

  template <class T, class IT>
    void generator<T,IT>::dump_visual()
    {
      dump(this->graph);
    }


  template <class T, class IT>
    void generator<T,IT>::dump(graph_t& parent)
    {
        auto vertlab = [=](node_t* n)->string{ return n->data.opid; };
        auto vertcol = [=](node_t* n)->string{ return lastnode == n ? "red" : "black"; };
        auto edgelab = [=](edge_t* n)->string{ return ctb::to_string(n->data.type) + ":" + n->data.dbglab; };
        parent.template dump_visual_label<false>(vertlab,  vertcol, edgelab);
    }


  template <class T, class IT>
    template <class W>
    writer<aliasenv_generator> generator<T,IT>::data_t::get_acces(int width, int granularity, W& w, bool c)
    {
      auto itr = acces_map.find(width);
      int classid = me->classid;
      if(itr != acces_map.end())
      {
        return itr->second;
      }
      else
      {
        //if a trivial path exists
        for(auto itr : acces_map)
        {
          string c1, c2, cc, cg, t;
          size_t printability;
          if(op->get_conv_string(itr.first, width, c1, c2, cc, t, printability))
          {
            if(c2.empty() && c1.empty() && cc.empty())
              warn(string("all code strings are empty at ").append(opid));
            W acces({newname(print("conv_w$1",width))});
            acces_map.insert(acces_map_t::value_type(width, acces));
            W basename = W().print(acces, 0);
            if(itr.first > width)
            {
              //SPLITS
              for(int i = 0; i < granularity/itr.first && i < printability; i++)
              {
                if(i!=0 && c)
                {
                  for(int j = 0; j < itr.first/width; ++j)
                    w.print("$declcode", t, W().print(acces, i*itr.first+j*width), basename,    "recursive argument here", i*itr.first, i*itr.first+j*width, j*width, classid, W().print(itr.second, i*itr.first), "", "");
                }
                else if(!c1.empty())
                {
                  if(width != itr.first/2)
                    error("basic halving code used for split where width_in != 2*width_out");
                  w.print("$conversioncode", t, W().print(acces, i*itr.first      ), basename,    c1, i*itr.first, i*itr.first      , 0    , classid, W().print(itr.second, i*itr.first), "", "");
                  w.print("$conversioncode", t, W().print(acces, i*itr.first+width), basename,    c2, i*itr.first, i*itr.first+width, width, classid, W().print(itr.second, i*itr.first), "", "");
                }
                else if(!cc.empty())
                {
                  stringlist sl;
                  for(int j = 0; j < itr.first/width; ++j)
                    sl.push_back(W().print(itr.second, i*width+j*itr.first).write_str());
                  w.print(cc, t, W().print(acces, i*itr.first ), basename, "recursive argument here", i*itr.first, i*itr.first, 0, classid, W().print(itr.second, i*itr.first), sl);
                }
                else if(!cg.empty())
                {
                  for(int j = 0; j < itr.first/width; ++j)
                    w.print(cg, t, W().print(acces, i*itr.first+j*width), basename,    "recursive argument here", i*itr.first, i*itr.first+j*width, j*width, classid, W().print(itr.second, i*itr.first), "", "");
                }
                else
                  error("conversion with all codes empty encountered!", false);
              }
              return acces;
            }
            else
            {
              //JOINS
              for(int i = 0; i < granularity/width && i < printability; i++)
              {
                if(i!=0 && c)
                {
                  w.print("$declcode", t, W().print(acces, i*width ), basename, "recursive argument here", i*width, i*width, 0, classid);
                }
                else if(!c1.empty())
                {  
                  w.print("$conversioncode", t, W().print(acces, i*width         ), basename, c1, i*width, i*width, 0, classid, W().print(itr.second, i*width),  W().print(itr.second, i*width+itr.first), "", "");
                }
                else if(!cc.empty())
                {
                  stringlist sl;
                  for(int j = 0; j < itr.first/width; ++j)
                    sl.push_back(W().print(itr.second, i*width+j*itr.first).write_str());
                  w.print(cc, t, W().print(acces, i*width ), basename, "recursive argument here", i*width, i*width, 0, classid, sl);
                  //  w.print(cc, t, W().print(acces, i*width ), "recursive argument here", W().print(itr.second, i*width),  W().print(itr.second, i*width+itr.first), "", "");
                }
                else if(!cg.empty())
                {
                  for(int j = 0; j < itr.first/width; ++j)
                    w.print(cg, t, W().print(acces, i*width ), basename, "recursive argument here", i*width, i*width+j*itr.first, j*itr.first, classid, W().print(itr.second, i*width+j*itr.first), "", "");
                }
                else
                  error("conversion with all codes empty encountered!", false);
              }
              return acces;
            }
          }
        }
      }
      //throw string("acces code of width ") + to_string(width) + " not found.";
      //try to find a nontrivial path
      int mindist = 100;
      int minsrc = -1;
      for(auto itr : acces_map)
      {
        int d = op->get_conversion_graph().get_dist(itr.first, width);
        if(d < mindist)
        {
          mindist = d;
          minsrc = itr.first;
        }
      }

      if(minsrc == -1)
      {
        error( string("conversion path to ") + to_string(width) + " at " + (me->id) +  " not found.");
        return writer<aliasenv_generator>();
      }
      else
      {
        while(minsrc != width)
        {
          int next;
          op->get_conversion_graph().get_dist(minsrc, width, &next);
          get_acces(next, granularity, w, c);
          minsrc = next;
        }
        return get_acces(width, granularity, w, c);
      }
    };

  template <class T, class IT>
    void generator<T,IT>::data_t::set_param(const string& name, const string& value)
    {
      params[name] = value;
    }

  template <class T, class IT>
    string generator<T,IT>::data_t::get_param(const string& name) const
    {
      auto itr = params.find(name);
      if(itr == params.end())
      {
        error(string("vertex parameter not found: ")+name);
        return "";
      }
      return itr->second;
    }

  template <class T, class IT>
    vector<typename generator<T,IT>::tid_t> generator<T,IT>::data_t::get_typespec() const
    {
      vector<tid_t> typespec;
      for(int i = 0;; ++i)
      {
        AUTO(edge)* src = me->in_at(i, false);
        if(src == NULL)
          break;
        typespec.push_back(src->data.type);
      }
      return typespec;
    }

  template <class T, class IT>
    int generator<T,IT>::data_t::newid(bool reset)
    {
      static int id = 0;
      if(reset)
        id = 0;
      return ++id;
    }

  template <class T, class IT>
    string generator<T,IT>::data_t::newname(string tag)
    {
      return print("var_$1_id$2_t$3_$$1_at$4", opid, newid(false), tag, me->id);
    }

  template <class T, class IT>
    int generator<T,IT>::get_broadest(int upperbound)
    {
      int w = 0;
      if(graph.out.empty())
        error( "graph is empty");
      else
        graph.crawl_topological([&](node_t* n) { w = max(n->data.op->get_max_width(upperbound),w);  });
      return w;
    }
};

#endif

