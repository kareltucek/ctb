#ifndef GRAPH_FACTOR_GUARD
#define GRAPH_FACTOR_GUARD

namespace ctb
{
  /**
   * General description
   * -------------------
   *  This is an extension to the basic graph, which maintains structure of a factor graph.
   * */

  template <class T, class I, bool directed, class ... O>
    class graph_factor : public graph_basic<T,I,directed,O...>
  {
    protected:
      class factor_class;
      typedef graph_basic<T,I,directed,O...> ancestor_t;
      typedef graph_basic<factor_class,int,true,graph_factor> factorgraph_myt;

        class factor_class 
        {
          protected:
            typename factorgraph_myt::node_t* me;
          public:
            template <class A, class...B> using proxy = proxy_<A,factorgraph_myt,ancestor_t,graph_factor,factor_class,B...>;
            proxy<std::set<typename ancestor_t::node*>> vertices;
            proxy<std::set<typename ancestor_t::node*>> in;
            proxy<std::set<typename ancestor_t::node*>> out;
            factor_class(typename factorgraph_myt::node_t* m) : me(m){};
        };
        void add_factor_edge(typename ancestor_t::node*, typename ancestor_t::node*);
    public:
      typedef factorgraph_myt factorgraph_t;
      typedef typename ancestor_t::node node_t;
      using graph_basic<T,I,directed,O...>::graph_basic;
      template <class A> using proxy = proxy_<A,graph_factor>;
      proxy<factorgraph_t> factor;
      void factorize();
      static void self_test();

  };
  typedef graph_factor<dummy,int,true> graph_factor_default;
  template <class T, class I, bool directed, class ... O> using graph_general = graph_factor<T,I,directed,O...>;

  template <class T, class I, bool directed, class ... O>
  void graph_factor<T,I,directed,O...>::add_factor_edge(node_t* from, node_t* to)
  {
    factor.rw().addedge(from->classid, to->classid); 
    factor.r().verts.r().find(from->classid.r())->second->data.rw().out.rw().insert(from);
    factor.r().verts.r().find(to->classid.r())->second->data.rw().in.rw().insert(to);
  }

  
  template <class T, class I, bool directed, class ... O>
  void graph_factor<T,I,directed,O...>::self_test()
  {
      typedef graph_factor_default g_t;
      g_t g;
      g.addvert(1, true, false);
      g.addvert(2, false, false);
      g.addvert(3, false, true);
      g.addvert(4, false, true);
      g.addedge(1,2);
      g.addedge(3,4);
      g.factorize();
      assert(g.verts->find(1)->second->classid.r() == g.verts->find(2)->second->classid.r());
      assert(g.verts->find(3)->second->classid == g.verts->find(4)->second->classid);
      assert(g.verts->find(1)->second->classid != g.verts->find(4)->second->classid);
  } 

    template <class T, class I, bool directed, class ... O>
  void graph_factor<T,I,directed,O...>::factorize()
  {
    factor.rw().clear();
    graph_basic<T,I,directed,O...>::factorize();
    for(int i = 0; i < this->classcount.r(); ++i)
      factor.rw().addvert(i, false, false);
    this->crawl_topological([=](node_t* n){ this->factor->verts.r().find(n->classid.r())->second->data.rw().vertices.rw().insert(n);  });
    this->crawl_topological([=](node_t* n){ 
        for(auto i : n->out.r().getlevel(1)) 
        {
        this->add_factor_edge(n,i); 
        }
        });
  }
};

#endif

