
#ifndef GENERATOR_GUARD
#define GENERATOR_GUARD

#include "graph.h"
#include "instructions.h"
#include "model_maker.h"
#include "model_generator.h"

namespace ctb
{
  template <class T, class IT>
    class generator
    {
      private:
        class data_t;
        typedef graph_generic<data_t, typename T::vid_t, true, generator> graph_t;
        typedef typename graph_t::node_t node_t;
        typedef typename T::opid_t id_t;
        typedef typename T::vid_t vid_t;
        typedef typename T::param_t param_t;
        typedef typename IT::operation_t op_t;
        template <typename A> using proxy = proxy_<A,generator>;

        class data_t
        {
          private:
            typedef std::map<int, writer<model_generator> > acces_map_t;

            node_t* me;
            acces_map_t acces_map;
            std::vector<param_t> parameters;

            template<typename P, typename...Ps> void push_params(P&&, Ps&&... params);
            void push_params();
            std::string newname(std::string tag) ;
            template <class W> writer<model_generator> get_acces(int width, int gran, W& w);
          public:
            template <typename A> using proxy = proxy_<A,data_t>;
            proxy<const op_t&> op;
            proxy<id_t> opid;

            template <typename... L> data_t( node_t* me, const typename IT::operation_t* o, id_t opi, L&&... p);
            template <class W> void generate(int granularity, W& w);
            int get_inout_pos() const;
        };

      public:
        proxy<const IT&> instab;
        proxy<graph_t> graph;

        generator(const IT& i);
        void set_instab(const IT& i);

        template <typename...L> void addvert(vid_t v, id_t op, L... p) ;
        void addedge(vid_t aid, vid_t bid, int b_argpos) ;

        template <class W> void generate(int granularity, W& w) ;
        int get_broadest(int upperbound = 10000000) ;
    };

  typedef generator<traits, instruction_table_default> generator_default; 

  template <class T, class IT>
    template <typename...L> void generator<T,IT>::addvert(vid_t v, id_t op, L... p)  
    {
      auto ptr = &instab->dec(op);
      graph.rw().addvert(v, ptr->is(fINPUT), ptr->is(fOUTPUT), ptr, op, p...);
    }

  template <class T, class IT>
    template <typename... L>
    generator<T,IT>::data_t::data_t( node_t* m, const typename IT::operation_t* o, id_t opi, L&&... p) : me(m), opid(opi), acces_map(), op(*o)
    {
      push_params((std::forward<L>(p))...);
    }

  template <class T, class IT>
    void generator<T,IT>::set_instab(const IT& i)
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
    generator<T,IT>::generator(const IT & i) : instab(i), graph()
  {
  }

  template <class T, class IT>
    template <class W>
    void generator<T,IT>::generate(int packsize, W& w)
    {
      if(graph->out->empty())
        throw "graph is empty";
      else
        graph->out[0]->crawl_topological([&](node_t* n) {n->data.rw().generate(packsize, w); });
    }

  template <class T, class IT>
    template <class W>
    void generator<T,IT>::data_t::generate(int granularity, W& w)
    {
      W empty;
      int myin, myout;
      int mygran = op->get_max_width(granularity,&myin, &myout);
      if(granularity % mygran != 0)
        throw std::string("granularities are relatively prime!");
      if(myin != myout)
        throw std::string("asymetric instructions not supported");
      op->imbue_width(mygran);
      acces_map.clear();
#define ARG(a) (a-1 < me->in->size() ? W().print(me->in[a-1]->data.rw().get_acces(myin, granularity, w), i*myout) : empty)
      W acces({newname(print("w$1",myout))});
      acces_map.insert(acces_map_t::value_type(myout, acces));
      for(int i = 0; i < granularity/myin; i++)
      {
        if(op->is(fINPUT))
          w.print("$inputcode;", op->get_type_string(), W().print(acces, i*myout), op->get_op_string(), get_inout_pos(), ARG(1), ARG(2), ARG(3));
        else if(op->is(fOUTPUT))
          w.print("$outputcode;", op->get_type_string(), W().print(acces, i*myout), op->get_op_string(), get_inout_pos(), ARG(1), ARG(2), ARG(3));
        else
          w.print("$innercode", op->get_type_string(), W().print(acces, i*myout), op->get_op_string(), 0              , ARG(1), ARG(2), ARG(3));
      }
    }

  template <class T, class IT>
    template <class W>
    writer<model_generator> generator<T,IT>::data_t::get_acces(int width, int granularity, W& w)
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
          std::string c1, c2, t;
          if(op->get_conv_string(itr.first, width, c1, c2, t))
          {
            W acces({newname(print("conv_w$1",width))});
            acces_map.insert(acces_map_t::value_type(width, acces));
            if(itr.first > width)
            {
              for(int i = 0; i < granularity/itr.first; i++)
              {
                w.print("$conversioncode", t, W().print(acces, i*itr.first      ), c1, get_inout_pos(), W().print(itr.second, i*itr.first), "", "");
                w.print("$conversioncode", t, W().print(acces, i*itr.first+width), c2, get_inout_pos(), W().print(itr.second, i*itr.first), "", "");
              }
              return acces;
            }
            else
            {
              for(int i = 0; i < granularity/width; i++)
              {
                w.print("$conversioncode", t, W().print(acces, i*width          ), c1, get_inout_pos(), W().print(itr.second, i*width),  W().print(itr.second, i*width+itr.first), "", "");
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
        int d = op.r().get_conversion_graph().get_dist(itr.first, width);
        if(d < mindist)
        {
          mindist = d;
          minsrc = itr.first;
        }
      }

      if(minsrc != -1)
      {
        throw std::string("conversion path to ") + std::to_string(width) + " at " + (me->id.r()) +  " not found.";
      }
      else
      {
        while(minsrc != width)
        {
          int next;
          op.r().get_conversion_graph().get_dist(minsrc, width, &next);
          get_acces(next, granularity, w);
          minsrc = next;
        }
        return get_acces(width, granularity, w);
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
    std::string generator<T,IT>::data_t::newname(std::string tag)
    {
      static int id = 0;
      return print("var_$1_$2_$3_$$vindex", opid, id++, tag);
    }

  template <class T, class IT>
    int generator<T,IT>::get_broadest(int upperbound)
    {
      int w = 0;
      if(graph->out->empty())
        throw "graph is empty";
      else
        graph->out[0]->crawl_topological([&](node_t* n) {w = std::max(n->data->op->get_max_width(upperbound),w); });
      return w;
    }
};

#endif

