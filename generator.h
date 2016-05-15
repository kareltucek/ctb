
#ifndef GENERATOR_GUARD
#define GENERATOR_GUARD

#include "defines.h"
#include "graph.h"
#include "errorhandling.h"
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
   *  Parameters
   *  ----------
   *  generation is parametrized by the following parameters:
   *  - granularity (packsize) 
   *  - layered writer - instruction patterns are printed into various layers of this writer
   *  - layered options - these options determine properties of layers of the layered writer
   *  - select tag handler - restricts the instruction set
   *  - print tag handler - determines whether an instruction should be printed at all
   *  - once tag handler - allows instructions be printed according to tags - in most cases, the output_options structure may be used to achieve the same results; however, the semantics are different
   *
   *  The 'print' and 'once' tag handlers are sort of deprecated - they were used to provide functionality, which was layer enabled by the multilayered generation and options.  However, their semantics is unique and may not be always replacable. 
   *
   *  Type inference
   *  --------------
   *  We keep infered types as annotations of layer zero edges. The type inference is done between all transformations. The type inference strictly checks explicit instructions. Transformations are determined weakly and do not require the type signature to be complete. 
   *
   *  Annotations are also checked and fixed during any edge edits done through api of the generator - this is to keep inference consistent during transformations. For both convenience and performance reasons, we scan only neighbouring nodes. The type inference of expansions is not guaranteed to remain consistent - it is transformer's duty to ensure that it performs its transformations in sensible order - everything should work as long as single vertex is being expanded at one moment.
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
        typedef typename IT::operation_t::ccode_pair_t ccode_pair_t;
      public:
        typedef graph_general<data_t, datae_t, typename T::vid_t, true> graph_t;
        typedef typename graph_t::factorgraph_t factorgraph_t;
        typedef typename graph_t::node_t node_t;
        typedef typename graph_t::edge_t edge_t;
        typedef typename T::opid_t opid_t;
        typedef typename T::opid_t id_t;
        typedef typename T::vid_t vid_t;
        typedef typename T::tid_t tid_t;
        /**/
        struct output_options{
          bool once; /** makes this layer output an instruction pattern only once per node*/
          bool global_once; /** if the writer layer is empty, then one pattern is written; no-op otherwise*/
          string layer; /** if nonempty, forward output of this layer into layer (if we want outputs of different layers (e.g. a one with 'once=true' and another with 'once=false' in one target layer)*/
          int order; /**if multiple layers get outputted into the same writer (using the 'layer' option), then this determines order of output per node. This means that patterns of the two layers get interleaved with respect to the order parameter and also the topological ordering of nodes*/
          output_options() {once = false; global_once = false; layer = "";  order = -1;};
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
            acces_map_t acces_map; /** holds memoized names of results*/
            map<string,string> params; /** holds aliases for this node*/

            static int newid(bool reset);
            string newname(string tag) ;
            template <class W> writer<aliasenv_generator> get_acces(int width, int gran, W& w, bool c, bool toplevel = true); /** this generates a path of conversions to width; the resulting sets of access names are stored into the access_map*/
            void check_structure(const vector<tid_t>& intypes, const vector<tid_t>& outtypes, bool checkin, bool checkout); /**check that infered types are consistent with operation's definition and with neighbouring edges*/
            vector<const ccode_pair_t*> get_pattern_order(const typename IT::operation_t::ccode_cont_t& op_cc, imp_contB<output_options>& opts); /** this sorts custom code patterns in op_cc according to opts.order values and returns pointers to the original structure*/
            void set_types(const vector<tid_t>& intypes, const vector<tid_t>& outtypes); /** set infered types to neighbouring edges*/
            void infer_type(const IT& it, bool check_structure = true)  ; /**infer types of neighbouring edges and set them using the set_types method*/
            void infer_type_nocheck(const IT& it)  ; /** overload which does not throw any warnings*/
            void clear_type(); /** clear type inference records*/
          public:
            const op_t* op;
            id_t opid;

            data_t( node_t* me, const typename IT::operation_t* o, id_t opi, const map<string,string>& params = {});
            template <class W> void generate(graph_t& parent, int granularity, imp_contB<W>& w,  imp_contB<output_options>& , bool c); /*generates code of this node as well as of any width conversion which is required to get its parameters*/
            string get_param(const string&) const; /*returns parameter's value*/
            void set_param(const string&, const string&); /*sets a parameter*/
            void set_context(bool on = true); /** sets generator aliasenv context to his exact node. This way, you may use parameters of this node via our alias environment directly*/
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
        // J,K are either vertex identifiers or references to vertices or pointers to vertices
        // note that this API also checks and fixes type inference and performs other generator-related edits, so the graph should not be accessed directly unless you know what you are doing
        template <typename...L> typename graph_t::node_t* add_vert(vid_t v, id_t op, L... p) ;
        template <typename J> void rm_vert(J v);
        template <typename J, typename K> void add_edge(J aid, K bid, int b_argpos, int a_argpos, int layer = 0, string dbglab = "") ;
        template <typename J, typename K> void connect_as(J v, K as, bool inputs = true, bool outputs = true); /**adds new edges to 'v' to immitate the connection of 'as' */
        template <typename J> node_t* get_vert(J v)  ; /** resolves identifier, reference or pointer to a vertex pointer*/
        void rm_edge(edge_t* e); /**removes an edge and disconnects the neighbouring vertices from this edge*/
        template <bool Inverted = false> void foreach(function<void(node_t*)> f); /** this is an overload of crawl for topological search, may be also abbreviated as 'do f for each vertex'*/


        //generator related API
        template <class W> void generate(int granularity, imp_contB<W>& w,  imp_contB<output_options>& wd, shared_ptr<taghandler_base> p = NULL, shared_ptr<taghandler_base> q = NULL, shared_ptr<taghandler_base> s = NULL) ; /**for documentation see the introductory section of the generator class/*/
        template <class W> void generate_partition(int partition, int granularity, imp_contB<W>& w,  imp_contB<output_options>& wd, shared_ptr<taghandler_base> p = NULL, shared_ptr<taghandler_base> q = NULL, shared_ptr<taghandler_base> s = NULL) ; /*for documentation see the introductory section of the generator class*/
        int get_broadest(int upperbound = 10000000) ; 
        imp_contB<output_options> option_struct();
        int partition_count();
        bool partition_is_topo_max(int p);
        bool partition_is_topo_min(int p);

        //ctb related API
        void set_compiletest(bool);
        template<template <typename ...> class L, typename...P> void transform(P...params) ;


        //others
        void dump_visual(); /**this exports the graph including various debug information into a temporary dot representation and initiates gpicview; informations which are outputted by this method differ from the ones outputted by the standard exporter*/
        void clear();
        void reset();
        void update_types(); /** This updates infered data types and checks the graph structure */
        void update_factor(); /** This updates the factor graph*/
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
    void generator<T,IT>::update_factor()  
    {
      graph.update_factor();
    }


  template <class T, class IT>
    void generator<T,IT>::update_types()  
    {
      try
      {
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
    void generator<T,IT>::data_t::infer_type_nocheck(const IT& it)  
    {
      try
      {
        infer_type(it, false);
      }
      NOTHROW
    }


  template <class T, class IT>
    void generator<T,IT>::data_t::infer_type(const IT& it, bool check)  
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
        if(check)
          check_structure(op->in_types, outtypes, !op->is(fEFINPUT), !op->is(fEFOUTPUT));
      }
      else
      {
        auto typesig = this->get_typespec();
        bool ambig = false;
        const auto& exp = it.find_expansion(op->expansions, typesig, &ambig);
        if(ambig && check)
          warn(string("Type inference provided ambiguous results at node ") + me->id);
        set_types(exp.in_types, exp.out_types);
        if(check)
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
      auto* vv = graph.to_vert(v);
      for(auto* e : vv->in)
        e->from->data.infer_type_nocheck(instab);
      vv->data.infer_type_nocheck(instab); 
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
      e->from->data.infer_type_nocheck(instab);
      e->to->data.infer_type_nocheck(instab);
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

      aliasenv_generator::set_granularity(packsize);
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
    vector<const typename generator<T,IT>::ccode_pair_t*> generator<T,IT>::data_t::get_pattern_order(const typename IT::operation_t::ccode_cont_t& op_cc, imp_contB<output_options>& opts)
    {
      typedef ccode_pair_t p_t;
      vector<const p_t*> result;
      map<int, vector<const p_t*>> acum;
      for(const p_t& p : op_cc)
        acum[opts[p.name].order].push_back(&p);
      for(const auto& listiter: acum)
        for(const auto& p: listiter.second)
          result.push_back(p);
      return result;
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
        typename IT::operation_t::ccode_cont_t  op_cc;
        const vector<int>* myins = NULL;
        int mywidth = 0;
        int myout = 0;
        int classid = me->classid;
        string type_string;
        string op_c;
        size_t printability; 
        bool found = false;
        if(op->is(fDEBUG))
        {
          mywidth = me->in_at(0)->from->data.op->get_max_width(granularity, &myins, &myout, true);
          found = op->get_op_string(myout, op_c, op_cc, printability, &myins, myout, mywidth);
        }
        else
          found = op->get_op_string(granularity, op_c, op_cc, printability, &myins, myout, mywidth);
        aliasenv_generator::set_widths(mywidth, myout);
        if(myins == NULL || myout == 0)
          error( string("suitable width not found!"));
        if((*myins).size() < op->in_types.size())
          error( string("widths_in does not match in_types"));
        if(granularity % myout != 0)
          error( string("granularities are relatively prime!"));
        if(me->in.size() != op->in_types.size())
        {
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
#define ARG(a) (me->in_at(a-1, false) != NULL ? W().print(me->in_at(a-1, false)->from->data.get_acces((*myins)[a-1], granularity, w, c), i*mywidth) : empty)
        W acces({newname(print("w$1",myout))});
        acces_map.insert(acces_map_t::value_type(myout, acces));
        if(op->is(fOUTPUT | fEFOUTPUT))
        {
          try { op->get_type_string(myout, type_string); }
          catch(...) {
          };
        }
        else
          op->get_type_string(myout, type_string);
        if(op->is(fEXPANSION))
          error( string("Found expansion during generation. All expansions have to be removed prior to generation by means of some (probably custom) graph transformation."));
        if(found && op_c.empty() && op_cc.empty())
          warn(string("instruction code and custom code are both empty for ").append(opid));
        /*even for unprintable code we want to consume ids -> cannot skip most of this function*/
        W basename = W().print(acces,0);
        vector<const ccode_pair_t*> op_cc_ordered = get_pattern_order(op_cc, opts);
        map<string, string> sl;
        for(int i = 0; i < granularity/mywidth && i < printability; i++)  
        {
          if(true) //handle nonregular widths
          {
            for(int j = 0; j < mywidth/myout; ++j) //split names
              sl[W().print("name$1", j+1).write_str()] = W().print(acces, i*mywidth+j*myout).write_str();
            for(int i = 0; i < 3; i++)
            {
              auto* e = me->in_at(i, false);
              if(e != NULL)
              {
                int myin = (*myins)[i];
                for(int j = 0; j < myout/myin; ++j) //join names
                {
                  sl[W().print("arg${1}n${2}", i+1, j+1).write_str()] = W().print(e->from->data.get_acces(myin, granularity, w, c), i*mywidth+j*myin).write_str();
                }
              }
            }
            for(auto* e : me->in)
            {
              if((*myins).size() <= e->to_pos)
                error("width not found for an input edge; structural type inference and check should not have allowed this! ");
              else
                sl[W().print("arg${1}", e->to_pos + 1).write_str()] = W().print(e->from->data.get_acces((*myins)[e->to_pos], granularity, w, c), i*mywidth).write_str(); 
            }
            aliasenv_generator::setnamemap(&sl);
          }


          if(i!=0 && c) //print empty declaration in case we want compile test
          {
            for(int j = 0; j < mywidth/myout; ++j) //split names
            {
              W name = W().print(acces,i*mywidth+j*myout);
              w.print("$declcode",     type_string, name, basename, ""  ,  myout*i, myout*i, 0, classid);
            }
          }
          else
          {
            for(int j = 0; j < mywidth/myout; ++j) //split names
            {
              W name = W().print(acces,i*mywidth+j*myout);
              if(!op_c.empty()) //neccessary!
              {
                if(op->is(fINPUT | fEFINPUT)) 
                  w["default"].print("$inputcode" , type_string, name, basename, op_c, myout*i, myout*i, 0, classid);
                else if(op->is(fOUTPUT | fEFOUTPUT))
                  w["default"].print("$outputcode", type_string, name, basename, op_c, myout*i, myout*i, 0, classid);
                else if(op->is(fDEBUG)) //same as output
                  w["default"].print("$outputcode", type_string, name, basename, op_c, myout*i, myout*i, 0, classid);
                else
                  w["default"].print("$innercode" , type_string, name, basename, op_c, myout*i, myout*i, 0, classid);
              }
            }

            for(int wi = 0; wi < op_cc_ordered.size(); wi++)
            {
              W name = W().print(acces,i*mywidth);
              string wname = op_cc_ordered[wi]->name;
              string oname = opts[wname].layer == "" ? op_cc_ordered[wi]->name : opts[wname].layer;
              if(opts[wname].once && i != 0)
              {
                continue;
              }
              else if(opts[wname].global_once && !w[oname].empty())
              {
                continue;
              }
              else 
              {
                w[oname].print(op_cc_ordered[wi]->code,           type_string, name, basename, ""  , myout*i, myout*i, 0, classid);
              }
            }
          }
        }
        aliasenv_generator::setnamemap(NULL);
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
    writer<aliasenv_generator> generator<T,IT>::data_t::get_acces(int width, int granularity, W& w, bool c, bool toplevel)
    {
      static int depth = 0;
      if(depth > 20)
      {
        warn("width conversion has 20 nested calls - something wrong is happening");
        if(allow_graphs)
          op->get_conversion_graph().dump_visual();
      }
      depth++;
      auto itr = acces_map.find(width);
      int classid = me->classid;
      if(itr != acces_map.end())
      {
        depth--;
        return itr->second;
      }
      else
      {
        //if a trivial path exists
        for(auto itr : acces_map)
        {
          string c1, c2, cc, cg, t;
          size_t printability;
          if(op->get_conv_string(itr.first, width, c1, c2, cc, cg, t, printability))
          {
            aliasenv_generator::set_widths(itr.first, width);
            if(c2.empty() && c1.empty() && cc.empty() && cg.empty())
              warn(string("all code strings are empty at ").append(opid));
            W acces({newname(print("conv_w$1",width))});
            acces_map.insert(acces_map_t::value_type(width, acces));
            W basename = W().print(acces, 0);
            map<string,string> sl;
            aliasenv_generator::setconvmap(&sl);
            if(itr.first > width)
            {
              //SPLITS
              for(int i = 0; i < granularity/itr.first && i < printability; i++)
              {
                sl["arg1"] = W().print(itr.second, i*itr.first).write_str();
                if(i!=0 && c)
                {
                  for(int j = 0; j < itr.first/width; ++j)
                    w.print("$declcode", t, W().print(acces, i*itr.first+j*width), basename,    "recursive argument here", i*itr.first, i*itr.first+j*width, j*width, classid, "", "", "");
                  continue;
                }
                if(!c1.empty())
                {
                  if(width != itr.first/2)
                    error("basic halving code used for split where width_in != 2*width_out");
                  w.print("$conversioncode", t, W().print(acces, i*itr.first      ), basename,    c1, i*itr.first, i*itr.first      , 0    , classid, "", "", "");
                  w.print("$conversioncode", t, W().print(acces, i*itr.first+width), basename,    c2, i*itr.first, i*itr.first+width, width, classid, "", "", "");
                }
                if(!cc.empty())
                {
                  for(int j = 0; j < itr.first/width; ++j)
                  {
                    sl[W().print("name$1", j+1).write_str()] = W().print(itr.second, i*width+j*itr.first).write_str();
                  }
                  w.print(cc, t, W().print(acces, i*itr.first ), basename, "recursive argument here", i*itr.first, i*itr.first, 0, classid, "");
                }
                if(!cg.empty())
                {
                  for(int j = 0; j < itr.first/width; ++j)
                  {
                    w.print(cg, t, W().print(acces, i*itr.first+j*width), basename,    "recursive argument here", i*itr.first, i*itr.first+j*width, j*width, classid, "", "");
                  }
                }
                if(c1.empty() && cc.empty() && cg.empty())
                  error("split conversion with all codes empty encountered!", false);
              }
              aliasenv_generator::setconvmap(NULL);
              depth--;
              return acces;
            }
            else
            {
              //JOINS
              for(int i = 0; i < granularity/width && i < printability; i++)
              {
                sl["arg1"] = W().print(itr.second, i*width).write_str();
                sl["arg2"] = W().print(itr.second, i*width+itr.first).write_str();
                if(i!=0 && c)
                {
                  w.print("$declcode", t, W().print(acces, i*width ), basename, "recursive argument here", i*width, i*width, 0, classid);
                  continue;
                }
                if(!c1.empty())
                {  
                  w.print("$conversioncode", t, W().print(acces, i*width         ), basename, c1, i*width, i*width, 0, classid, W().print(itr.second, i*width),  W().print(itr.second, i*width+itr.first), "", "");
                }
                if(!c2.empty())
                {
                  warning("nonempty code2 encountered at a join width conversion - ignoring");
                }
                if(!cc.empty())
                {
                  for(int j = 0; j < width/itr.first; ++j)
                  {
                    sl[W().print("arg$1", j+1).write_str()] = W().print(itr.second, i*width+j*itr.first).write_str();
                    sl[W().print("arg1n$1", j+1).write_str()] = W().print(itr.second, i*width+j*itr.first).write_str();
                  }

                  w.print(cc, t, W().print(acces, i*width ), basename, "recursive argument here", i*width, i*width, 0, classid);
                }
                if(!cg.empty())
                {
                  for(int j = 0; j < itr.first/width; ++j)
                  {
                    sl["arg1"] = W().print(itr.second, i*width+j*itr.first).write_str();
                    w.print(cg, t, W().print(acces, i*width ), basename, "recursive argument here", i*width, i*width+j*itr.first, j*itr.first, classid, "", "");
                  }
                }
                if(cg.empty() && cc.empty() && c1.empty())
                  error("merge conversion with all codes empty encountered!", false);
              }
              aliasenv_generator::setconvmap(NULL);
              depth--;
              return acces;
            }
          }
        }
        if(!toplevel)
        {
          string efrom;
          string eto = ctb::to_string(width);
          for(auto itr : acces_map)
            efrom= efrom + " " + ctb::to_string(itr.first);

          error(string() + "conversion path not found when expected - from " + efrom + " to " + eto);
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
        depth--;
        return writer<aliasenv_generator>();
      }
      else
      {
        while(minsrc != width)
        {
          int next;
          op->get_conversion_graph().get_dist(minsrc, width, &next);
          get_acces(next, granularity, w, c, false);
          minsrc = next;
        }
        depth--;
        return get_acces(width, granularity, w, c, false);
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

