#ifndef GRAPH_FACTOR_GUARD
#define GRAPH_FACTOR_GUARD

#include "defines.h"

namespace ctb
{
  /**
   * General description
   * -------------------
   *  This is an extension to the basic graph, which can generate structure of a factor graph. Note that the factor structure is not kept up-to-date through edits of the original graph.
   * */

  template <class T, class I, bool directed>
    class graph_factor : public graph_basic<T,I,directed>
  {
    protected:
      class factor_class;
      typedef graph_basic<T,I,directed> ancestor_t;
      typedef graph_basic<factor_class,int,true> factorgraph_myt;

        class factor_class 
        {
          protected:
            typename factorgraph_myt::node_t* me;
          public:
            typename factorgraph_myt::node_t* node;
            set<typename ancestor_t::node*> vertices;
            set<typename ancestor_t::node*> in;
            set<typename ancestor_t::node*> out;
            factor_class(typename factorgraph_myt::node_t* m) : me(m), node(m){ };
        };
        void add_factor_edge(typename ancestor_t::node*, typename ancestor_t::node*);
    public:
      typedef factorgraph_myt factorgraph_t;
      typedef typename ancestor_t::node node_t;
      typedef typename ancestor_t::id_t id_t;
      using graph_basic<T,I,directed>::graph_basic;

      factorgraph_t factor;
      void factorize();
      static void self_test();

  };
  typedef graph_factor<dummy,int,true> graph_factor_default;
  template <class T, class I, bool directed> using graph_general = graph_factor<T,I,directed>;

  template <class T, class I, bool directed>
  void graph_factor<T,I,directed>::add_factor_edge(node_t* from, node_t* to)
  {
    factor.addedge(from->classid, to->classid); 
    factor.verts.find(from->classid)->second->data.out.insert(from);
    factor.verts.find(to->classid)->second->data.in.insert(to);
  }

  
  template <class T, class I, bool directed>
  void graph_factor<T,I,directed>::self_test()
  {
    cout << "testing factor graph" << endl;
      typedef graph_factor_default g_t;
      g_t g;
      g.addvert(1, true, false);
      g.addvert(2, false, false);
      g.addvert(3, false, true);
      g.addvert(4, false, true);
      g.addedge(1,2);
      g.addedge(3,4);
      g.factorize();
      assert(g.verts.find(1)->second->classid == g.verts.find(2)->second->classid);
      assert(g.verts.find(3)->second->classid == g.verts.find(4)->second->classid);
      assert(g.verts.find(1)->second->classid != g.verts.find(4)->second->classid);
  } 

    template <class T, class I, bool directed>
  void graph_factor<T,I,directed>::factorize()
  {
    factor.clear();
    graph_basic<T,I,directed>::factorize(); //will assign class ids in the simple graph
    for(int i = 0; i < this->classcount; ++i)
      factor.addvert(i, false, false);
    this->crawl_topological([&](node_t* n){ this->factor.verts.find(n->classid)->second->data.vertices.insert(n);  });
    this->crawl_topological([&](node_t* n){ for(auto i : n->in.getlevel(1)) { this->add_factor_edge(i->from,n); } });
    set<int> ins;
    set<int> outs;

    for(auto n : this->in)
      ins.insert(n->classid);
    for(auto n : this->out)
      outs.insert(n->classid);
    for(auto n : ins)
      factor.in.push_back(factor.verts.find(n)->second);
    for(auto n : outs)
      factor.out.push_back(factor.verts.find(n)->second);
  }
};

#endif

