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
    public:
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
          factor_class(typename factorgraph_myt::node_t* m) : me(m), node(m), vertices(), in(), out(){};
      };
      void add_factor_edge(typename ancestor_t::node*, typename ancestor_t::node*, int order);

      int classids_factor;
    public:
      typedef factorgraph_myt factorgraph_t;
      typedef typename factorgraph_myt::node_t factornode_t;
      typedef typename factorgraph_myt::node_t fnode_t;
      typedef typename ancestor_t::node node_t;
      typedef typename ancestor_t::id_t id_t;
      using graph_basic<T,I,directed>::graph_basic;

      factorgraph_t factor;
      void update_factor();
      void factorize();
      static void self_test();

      void dump(ostream& o, function<string(node_t*)> f = [](node_t* n){return "black";}, function<string(node_t*)> g = [](node_t* n){return "";});
      template<bool detach = false> void dump_visual(function<string(node_t*)> f = [](node_t* n){return "black";});
      template<bool detach = false> void dump_visual_label(function<string(node_t*)> g, function<string(node_t*)> f = [](node_t* n){return "black";});

  };
  typedef graph_factor<dummy,int,true> graph_factor_default;
  template <class T, class I, bool directed> using graph_general = graph_factor<T,I,directed>;

  template <class T, class I, bool directed>
    void graph_factor<T,I,directed>::update_factor()
    {
      if(this->classids_factor != this->classids_current)
        factorize();
    }

  template <class T, class I, bool directed>
    void graph_factor<T,I,directed>::add_factor_edge(node_t* from, node_t* to, int order)
    {
      factor.addedge(from->classid, to->classid, 0, 0, 0, order); 
      factor.verts.find(from->classid)->second->data.out.insert(from);
      factor.verts.find(to->classid)->second->data.in.insert(to);
    }

  template <class T, class I, bool directed>
      template<bool detach>
    void graph_factor<T,I,directed>::dump_visual(function<string(node_t*)> f)
    {
      dump_visual_label([](node_t* n){return "";}, f);
    }

  template <class T, class I, bool directed>
      template<bool detach>
    void graph_factor<T,I,directed>::dump_visual_label(function<string(node_t*)> g, function<string(node_t*)> f)
    {
      ofstream ofs;
      ofs.open("/tmp/graphjfjfjf");
      dump(ofs, f, g);
      ofs.close();
      cout << "showing graph" << endl;
      string command = "bash -c \"cat /tmp/graphjfjfjf | dot -Tpng > /tmp/graphjfjfjf.png && nohup gpicview /tmp/graphjfjfjf.png\"";
      if(detach)
        command.append(" & ");
      system(command.c_str());
    }

  template <class T, class I, bool directed>
    void graph_factor<T,I,directed>::dump (ostream& o, function<string(node_t*)> colour_callback, function<string(node_t*)> label)
    {
      bool printlabels = false;
      auto& graph = *this;

      string prefix = "#";
      int level = 1;
      bool showempty = true;

      //output edges of factor graph
      auto f = [&](fnode_t* n)
      { 
        if(n->in.empty() && showempty)
          o << prefix << n->id << ";" << endl;
        for( auto e : n->in ) 
          o << prefix << e->from->id << " -> " << e->to->id << endl;// << "[label=\"" << e->order << "\"];" << endl;
      };

      //output edges of standard graph
      auto h = [&](node_t* n)
      { 
        if(n->in.empty() && showempty)
          o << prefix <<  n->id << endl;
        for( auto e : n->in.getlevel(level) ) 
        {
          o << prefix << e->from->id << " -> " << e->to->id;
          if(printlabels)
            o << " [label=\"" << e->order << "\",taillabel=\""+ ctb::to_string( e->frompos ) +"\",headlabel=\""+ ctb::to_string( e->topos ) +"\"];" << endl;
          else
            o << ";" << endl;
        }
      };

      //initialize crawling of partitions of a factor graph
      auto g =  [&](fnode_t* n)
      { 
        if(n->data.vertices.empty())
          o << prefix << "partition " << n->classid << " is empty" << endl;
        else
        {
          o << prefix << "partition " << n->id << ":" << endl;
          (*n->data.vertices.begin())->crawl_topological(h);
        }
      };

      //output color of node
      auto i =  [&](node_t* n)
      { 
        o << n->id << " [color=\"" << colour_callback(n) << "\",label=\"" + (ctb::to_string(n->id) + " " +label(n)) + "\"];" << endl;
      };

      level = 0;
      showempty = true;

      //print comments
      o << prefix << "factor structure is:" << endl;
      graph.factor.crawl_topological(f);

      o << prefix << "partitions are:" << endl;
      graph.factor.crawl_topological(g); 

      level = 1;
      o << prefix << "factor invisible edges are" << endl;
      graph.crawl_topological(h);

      //print actual graph
      prefix = "";
      o << "digraph G {" << endl;
      level = 1;
      o << "edge [color = gray];" << endl;
      graph.crawl_topological(h);
      o << "edge [color = black];" << endl;
      level = 0;
      graph.crawl_topological(h);
      o << "edge [color = red];" << endl;
      graph.factor.crawl_topological(f);

      graph.crawl_topological(i);//colorize by callback
      o << "}" << endl;
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
      assert(g.factor.verts.find(0)->second->data.vertices.size() == 2);
      cout << "size is " <<g.factor.verts.find(1)->second->data.vertices.size()  << endl;
      g.dump_visual();
      assert(g.factor.verts.find(1)->second->data.vertices.size() == 2);
      assert(g.factor.verts.find(0)->second->data.out.size() == 0);
      assert(g.factor.verts.find(1)->second->data.out.size() == 0);
      assert(g.factor.verts.find(0)->second->data.in.size() == 0);
      assert(g.factor.verts.find(1)->second->data.in.size() == 0);
      g.clear();
      assert(g.verts.size() == 0);
      assert(g.in.size() == 0);
      assert(g.out.size() == 0);
    } 

  template <class T, class I, bool directed>
    void graph_factor<T,I,directed>::factorize()
    {
      factor.clear();
      graph_basic<T,I,directed>::update_factor(); //will assign class ids in the simple graph
      for(int i = 0; i < this->classcount; ++i)
        factor.addvert(i, false, false);
      this->crawl_topological([&](node_t* n){ this->factor.verts.find(n->classid)->second->data.vertices.insert(n);  });
      this->crawl_topological([&](node_t* n){ for(auto i : n->in.getlevel(1)) { this->add_factor_edge(i->from,n,i->order); } });
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

      classids_factor = this->classids_current;
    }
};

#endif

