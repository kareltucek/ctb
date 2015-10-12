
#ifndef GENERATOR_GUARD
#define GENERATOR_GUARD

#include "graph.h"
#include "instructions.h"
#include "aliasenv_maker.h"
#include "aliasenv_generator.h"

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
      public:
        typedef graph_general<data_t, typename T::vid_t, true, generator> graph_t;
        typedef typename graph_t::node_t node_t;
      private:
        typedef typename T::opid_t id_t;
        typedef typename T::vid_t vid_t;
        typedef typename T::param_t param_t;
        typedef typename IT::operation_t op_t;
        template <typename A> using proxy = proxy_<A,generator>;

        class data_t
        {
          private:
            typedef std::map<int, writer<aliasenv_generator> > acces_map_t;
            friend class generator;

            node_t* me;
            acces_map_t acces_map;
            std::vector<param_t> parameters;

            template<typename P, typename...Ps> void push_params(P&&, Ps&&... params);
            void push_params();
            static int newid(bool reset);
            std::string newname(std::string tag) ;
            template <class W> writer<aliasenv_generator> get_acces(int width, int gran, W& w, bool c);
          public:
            template <typename A> using proxy = proxy_<A,data_t,generator>;
            proxy<const op_t*> op;
            proxy<id_t> opid;

            template <typename... L> data_t( node_t* me, const typename IT::operation_t* o, id_t opi, L&&... p);
            template <class W> void generate(int granularity, W& w, bool c);
            int get_inout_pos() const;
        };

        bool compiletest; /*abbreviated as plain 'c'*/
      public:

        proxy< IT&> instab;
        proxy<graph_t> graph;

        generator( IT& i);
        void set_instab( IT& i);

        template <typename...L> void addvert(vid_t v, id_t op, L... p) ;
        void addedge(vid_t aid, vid_t bid, int b_argpos) ;

        template <class W> void generate(int granularity, W& w, std::shared_ptr<taghandler_base> p = NULL, std::shared_ptr<taghandler_base> q = NULL, std::shared_ptr<taghandler_base> s = NULL) ;
        int get_broadest(int upperbound = 10000000) ;

        void set_compiletest(bool);

        template<template <typename ...> class L, typename...P> void transform(P...params) ;

        void clear();
        void reset();
        void update(); /** In case instruction table is reloaded the operation pointers are no longer valid. This function updates them.*/
    };

  typedef generator<traits, instruction_table_default> generator_default;

    template <class T, class IT>
        template<template <typename ...> class L, typename...P> 
  void generator<T,IT>::transform(P...params)
  {
    L<graph_t> l;
    l.transform(graph.rw(),params...);
  }

  template <class T, class IT>
    void generator<T,IT>::update()
    {
      for(auto itr = graph->verts->cbegin(); itr != graph->verts->cend(); ++itr)
      {
        data_t& n = itr->second->data.rw();
        n.op.rw() = &instab->dec(n.opid.r());
        /*
           std::string a = n.opid.r();
           instab->dec(a);
           instab->dec(n.opid.r());
           */
      }
      //graph.rw().crawl_topological([&](typename graph_t::node_t* n){n->data.op.rw() = instab->dec(n->data->opid.r());});
      /*
         for( auto op : graph->verts.r() )
         {
         std::string id = op.second->data->opid.r();
         op.second->data.rw().op.rw() =
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
      graph.rw().clear();
    }

  template <class T, class IT>
    template <typename...L> void generator<T,IT>::addvert(vid_t v, id_t op, L... p)  
    {
      auto ptr = &instab->dec(op);
      graph.rw().addvert(v, ptr->is(fINPUT), ptr->is(fOUTPUT), ptr, op, p...);
    }

  template <class T, class IT>
    template <typename... L>
    generator<T,IT>::data_t::data_t( node_t* m, const typename IT::operation_t* o, id_t opi, L&&... p) : me(m), opid(opi), acces_map(), op(o)
    {
      push_params((std::forward<L>(p))...);
    }

  template <class T, class IT>
    void generator<T,IT>::set_instab( IT& i)
    {
      instab.rw() = i; 
    }

  template <class T, class IT>
    template <typename P, typename...Ps>
    void generator<T,IT>::data_t::push_params(P&& p, Ps&&...params)
    {
      parameters.push_back(std::forward<P>(p));
      push_params((std::forward<Ps>(params))...);
    }

  template <class T, class IT>
    void generator<T,IT>::addedge(vid_t aid, vid_t bid, int b_argpos)  
    {
      graph.rw().addedge(aid, bid, b_argpos);
    }


  template <class T, class IT>
    void generator<T,IT>::data_t::push_params()
    {
    }

  template <class T, class IT>
    generator<T,IT>::generator( IT & i) : instab(i), graph(), compiletest(false)
  {
  }

  template <class T, class IT>
    template <class W>
    void generator<T,IT>::generate(int packsize, W& w, std::shared_ptr<taghandler_base> ts, std::shared_ptr<taghandler_base> tp, std::shared_ptr<taghandler_base> to)
    {
      if(graph->out->empty())
        error( "graph is empty");
      if(ts!=NULL)
        instab.rw().add_tags(ts,gSELECT);
      if(tp!=NULL)
        instab.rw().add_tags(tp,gPRINT);
      if(to!=NULL)
        instab.rw().add_tags(to,gONCE);
      instab->update_tags();
      data_t::newid(true); //resets ids
      graph.rw().crawl_topological([&](node_t* n) {n->data.rw().generate(packsize, w, compiletest); });
      if(to!=NULL)
        instab.rw().rm_tags(to,gONCE);
      if(tp!=NULL)
        instab.rw().rm_tags(tp,gPRINT);
      if(ts!=NULL)
        instab.rw().rm_tags(ts,gSELECT);
    }

  template <class T, class IT>
    template <class W>
    void generator<T,IT>::data_t::generate(int granularity, W& w, bool c)
    {
      try
      {
        W empty;
        int myin, myout;
        int mygran = op->is(fDEBUG) ? me->in[0]->data->op->get_max_width(granularity, &myin, &myout) : op->get_max_width(granularity,&myin, &myout);
        if(mygran == 0)
          error( std::string("suitable width not found!"));
        if(granularity % mygran != 0)
          error( std::string("granularities are relatively prime!"));
        if(myin != myout)
          error( std::string("asymetric instructions not supported"));
        if(me->in->size() != op->in_types->size())
          error( std::string("count of input nodes does not match operation specification"));
        for(int i = 0; i < me->in->size(); ++i)
          if(me->in[i]->data->op->out_type.r() != op->in_types[i])
            error( std::string("argument ").append(std::to_string(i)).append(" does not match defined input type: got ").append(me->in[i]->data->op->out_type.r()).append(" wanted ").append(op->in_types[i]));
        //op->imbue_width(mygran);
        acces_map.clear();
#define ARG(a) (a-1 < me->in->size() ? W().print(me->in[a-1]->data.rw().get_acces(myin, granularity, w, c), i*myout) : empty)
        W acces({newname(print("w$1",myout))});
        acces_map.insert(acces_map_t::value_type(myout, acces));
        std::string type_string, op_c, op_cc;
        std::size_t printability; 
        op->get_type_string(mygran, type_string);
        bool found = op->get_op_string(mygran, op_c, op_cc, printability);
        if(found && op_c.empty() && op_cc.empty())
          warn(std::string("instruction code and custom code are both empty for ").append(opid));
        /*even for unprintable code we want to consume ids -> cannot skip most of this function*/
	W basename = W().print(acces,0);
        for(int i = 0; i < granularity/myin && i < printability; i++) 
        {
	  W name = W().print(acces,i*myout);
          if(i!=0 && c)
          {
            w.print("$declcode",     type_string, name, basename, ""  , get_inout_pos(), myin*i, myin*i, 0, ARG(1), ARG(2), ARG(3));
          }
          else if(op_c.empty())
          {
            w.print(op_cc,           type_string, name, basename, ""  , get_inout_pos(), myin*i, myin*i, 0, ARG(1), ARG(2), ARG(3));
          }
          else
          {
            if(op->is(fINPUT))
              w.print("$inputcode" , type_string, name, basename, op_c, get_inout_pos(), myin*i, myin*i, 0,ARG(1), ARG(2), ARG(3));
            else if(op->is(fOUTPUT))
              w.print("$outputcode", type_string, name, basename, op_c, get_inout_pos(), myin*i, myin*i, 0,ARG(1), ARG(2), ARG(3));
            else if(op->is(fDEBUG)) //same as output
              w.print("$outputcode", type_string, name, basename, op_c, get_inout_pos(), myin*i, myin*i, 0,ARG(1), ARG(2), ARG(3));
            else
              w.print("$innercode" , type_string, name, basename, op_c, 0              , myin*i, myin*i, 0,ARG(1), ARG(2), ARG(3));
          }
        }
      }
      catch (error_struct& err)
      {
        std::stringstream s;
        s << "while processing node " << me->id.r() << " with opcode " << opid.r() << "\n    " << err.first;
        if(err.second)
          error(s.str(), true);
        else
          std::cerr << s.str() << std::endl;
      }
    }

  template <class T, class IT>
    template <class W>
    writer<aliasenv_generator> generator<T,IT>::data_t::get_acces(int width, int granularity, W& w, bool c)
    {
      auto itr = acces_map.find(width);
      if(itr != acces_map.end())
      {
        return itr->second;
      }
      else
      {
        //if a trivial path exists
        for(auto itr : acces_map)
        {
          std::string c1, c2, cc, cg, t;
          std::size_t printability;
          if(op->get_conv_string(itr.first, width, c1, c2, cc, t, printability))
          {
            if(c2.empty() && c1.empty() && cc.empty())
              warn(std::string("all code strings are empty at ").append(opid));
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
                      w.print("$declcode", t, W().print(acces, i*itr.first+j*width), basename,    "recursive argument here", get_inout_pos(), i*itr.first, i*itr.first+j*width, j*width, W().print(itr.second, i*itr.first), "", "");
                }
                else if(!c1.empty())
                {
                  if(width != itr.first/2)
                    error("basic halving code used for split where width_in != 2*width_out");
                  w.print("$conversioncode", t, W().print(acces, i*itr.first      ), basename,    c1, get_inout_pos(), i*itr.first, i*itr.first      , 0    , W().print(itr.second, i*itr.first), "", "");
                  w.print("$conversioncode", t, W().print(acces, i*itr.first+width), basename,    c2, get_inout_pos(), i*itr.first, i*itr.first+width, width, W().print(itr.second, i*itr.first), "", "");
                }
                else if(!cc.empty())
                {
                  stringlist sl;
                  for(int j = 0; j < itr.first/width; ++j)
                    sl.push_back(W().print(itr.second, i*width+j*itr.first).write_str());
                  w.print(cc, t, W().print(acces, i*itr.first ), basename, "recursive argument here", get_inout_pos(), i*itr.first, i*itr.first, 0, W().print(itr.second, i*itr.first), sl);
                }
                else if(!cg.empty())
                {
                  for(int j = 0; j < itr.first/width; ++j)
                      w.print(cg, t, W().print(acces, i*itr.first+j*width), basename,    "recursive argument here", get_inout_pos(), i*itr.first, i*itr.first+j*width, j*width, W().print(itr.second, i*itr.first), "", "");
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
                  w.print("$declcode", t, W().print(acces, i*width ), basename, "recursive argument here", get_inout_pos(), i*width, i*width, 0);
                }
                else if(!c1.empty())
                {  
                  w.print("$conversioncode", t, W().print(acces, i*width         ), basename, c1, get_inout_pos(), i*width, i*width, 0, W().print(itr.second, i*width),  W().print(itr.second, i*width+itr.first), "", "");
                }
                else if(!cc.empty())
                {
                  stringlist sl;
                  for(int j = 0; j < itr.first/width; ++j)
                    sl.push_back(W().print(itr.second, i*width+j*itr.first).write_str());
                  w.print(cc, t, W().print(acces, i*width ), basename, "recursive argument here", get_inout_pos(), i*width, i*width, 0, sl);
                //  w.print(cc, t, W().print(acces, i*width ), "recursive argument here", get_inout_pos(), W().print(itr.second, i*width),  W().print(itr.second, i*width+itr.first), "", "");
                }
                else if(!cg.empty())
                {
                  for(int j = 0; j < itr.first/width; ++j)
                    w.print(cg, t, W().print(acces, i*width ), basename, "recursive argument here", get_inout_pos(), i*width, i*width+j*itr.first, j*itr.first, W().print(itr.second, i*width+j*itr.first), "", "");
                }
                else
                  error("conversion with all codes empty encountered!", false);
              }
              return acces;
            }
          }
        }
      }
      //throw std::string("acces code of width ") + std::to_string(width) + " not found.";
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
        error( std::string("conversion path to ") + std::to_string(width) + " at " + (me->id.r()) +  " not found.");
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
    int generator<T,IT>::data_t::get_inout_pos() const
    {
      if(parameters.empty())
        return 0;
      //throw std::string("envelope id not defined for vertex: ").append(id);
      return (int)parameters[0];
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
    std::string generator<T,IT>::data_t::newname(std::string tag)
    {
      return print("var_$1_id$2_t$3_$$1_at$4", opid, newid(false), tag, me->id.r());
    }

  template <class T, class IT>
    int generator<T,IT>::get_broadest(int upperbound)
    {
      int w = 0;
      if(graph->out->empty())
        error( "graph is empty");
      else
        graph.rw().crawl_topological([&](node_t* n) { w = std::max(n->data->op->get_max_width(upperbound),w);  });
      return w;
    }
};

#endif

