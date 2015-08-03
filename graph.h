#ifndef GRAPH_GUARD
#define GRAPH_GUARD

#include "datatypes.h"
#include <functional>
#include <queue>
#include <map>
#include <string>
#include <limits>
#include <assert.h>

namespace ctb
{
  //contained Type, Id type, Owner types
  template <class T, class I, bool directed, class ... O>
    class graph_generic
    {
      private:

        class node
        {
          private:
            graph_generic* parent;
            typedef std::pair<node*, int> route;
            mutable std::vector<route> map;
            int index;
            int lastpass;
            int newid();
            friend class graph_generic;
            template <typename... L> node(I vid, int index, L&&... p);
            bool update_distances();
            bool init_map(int size);
            node * get_path(node * n);
          public:
            template <class A, class...B> using proxy = proxy_<A,node,graph_generic,B...>;
            proxy<T, O...> data;
            proxy<I> id;
            proxy<std::vector<node*> > out;
            proxy<std::vector<node*> > in;
            proxy<T,O...>& operator->();
            template <bool recurse = false, bool inverse = false> void crawl(std::function<bool(node*)> f, std::function<bool(node*)> g, std::queue<node*>* q = NULL);
            void crawl_topological(std::function<void(node*)> f);
        };

        typedef std::map<I, node*> vertex_container_t;
        typedef std::vector<node*> vertex_list_t;
        int index;
      public:
        typedef node node_t;
        template <class A> using proxy = proxy_<A,graph_generic>;
        proxy<vertex_list_t> in;
        proxy<vertex_list_t> out;
        proxy<vertex_container_t> verts;
        graph_generic();
        ~graph_generic();
        template <typename...L> void addvert(I v, bool in , bool out , L&&... p) ;
        void addedge(I aid, I bid, int b_argpos = -1) ;
        void calculate_distances();
        int get_dist(I a, I b, I* c = NULL) const;
        static void self_test();
        void clear();
    };


  typedef graph_generic<dummy,int,true> graph_default;

  template <class T, class I, bool directed, class ... O>
    graph_generic<T,I,directed,O...>::~graph_generic()
    {
      clear();
    }
  
  template <class T, class I, bool directed, class ... O>
    void graph_generic<T,I,directed,O...>::clear()
    {
      in.rw().clear();
      out.rw().clear();
      for(auto v : verts.rw())
        delete v.second;
    }

  template <class T, class I, bool directed, class ... O>
    void graph_generic<T,I,directed,O...>::self_test()
    {
      graph_generic<dummy, int, true> g;
      g.addvert(1, true, false);
      g.addvert(2, false, false);
      g.addvert(3, false, true);
      g.addvert(4, false, true);
      g.addedge(1,2);
      g.addedge(2,3);
      g.addedge(2,4);
      int i = 0;
      int results[] = {1,2,3,4};
      g.out.r().front()->crawl_topological([&](node* n){ assert(n->id.r() == results[i++]);});
      g.calculate_distances();
      assert(g.get_dist(1,4) == 2);
      //  
      graph_generic<dummy, int, false> h;
      h.addvert(1, false, false);
      h.addvert(2, false, false);
      h.addvert(3, false, false);
      h.addvert(4, false, false);
      h.addedge(1,2);
      h.addedge(2,3);
      h.addedge(3,4);
      h.addedge(1,4);
      h.calculate_distances(); //implicit test of crawling functions
      assert(h.get_dist(1,3) == 2);
      assert(h.get_dist(1,4) == 1);
    }

  template <class T, class I, bool directed, class ... O>
    graph_generic<T,I,directed,O...>::node::proxy<T,O...>& graph_generic<T,I,directed,O...>::node::operator->()  
    {
      return data;
    }

  template <class T, class I, bool directed, class ... O>
    int graph_generic<T,I,directed,O...>::get_dist(I a, I b, I* c) const
    {
      auto va = verts.r().find(a);
      auto vb = verts.r().find(b);
      if(va == verts->end() || vb == verts->end())
        throw "unknown vertex id";
      //  
      if(c != NULL)
        *c = va->second->map[vb->second->index].first->id.r();
      return va->second->map[vb->second->index].second;
    }

  template <class T, class I, bool directed, class ... O>
    typename graph_generic<T,I,directed,O...>::node* graph_generic<T,I,directed,O...>::node::get_path(node * n)
    {
      return map[n->index].first;
    }

  template <class T, class I, bool directed, class ... O>
    void graph_generic<T,I,directed,O...>::calculate_distances()  
    {
      if(verts.r().empty())
        return;
      int size = this->index;
      (*verts.r().begin()).second->template crawl<true, true>([=](node* n)->bool{ n->init_map(size); return true;}, [=](node*n)->bool{return n->map.size() != size;} );
      (*verts.r().begin()).second->template crawl<directed, true>([=](node* n)->bool{ return n->update_distances();}, [=](node*n)->bool{return true;} );
    }


  template <class T, class I, bool directed, class ... O>
    bool graph_generic<T,I,directed,O...>::node::update_distances()
    {
      bool updated = false;
      for( auto n : out.rw())
      {
        for( int i = 0; i < n->map.size(); ++i)
        {
          if( n->map[i].second + 1 < map[i].second)
          {
            map[i] = n->map[i];
            map[i].second = n->map[i].second + 1;
            map[i].first = n;
            updated = true;
          }
        }
      }
      return updated;
    }

  template <class T, class I, bool directed, class ... O>
    bool graph_generic<T,I,directed,O...>::node::init_map(int size)
    {
      map.clear();
      for(int i = 0; i < size; i++)
      {
        map.push_back(route(NULL, std::numeric_limits<int>::max()-1));
      }
      map[index].first = this;
      map[index].second = 0;
      return true;
    }

  template <class T, class I, bool directed, class ... O>
    void graph_generic<T,I,directed,O...>::addedge(I aid, I bid, int b_argpos)  
    {
      typename vertex_container_t::iterator a = verts.rw().find(aid);
      typename vertex_container_t::iterator b = verts.rw().find(bid);
      if(a == verts->end() || b == verts->end())
        throw "unknown vertex id";
      if(directed)
      {
        a->second->out.rw().push_back(b->second);
        if(b_argpos != -1)
        {
          while(b->second->in.rw().size() < b_argpos+1)
            b->second->in.rw().push_back(NULL);
          b->second->in.rw()[b_argpos] = a->second;
        }
        else
          b->second->in.rw().push_back(a->second);
      }
      else
      {
        a->second->out.rw().push_back(b->second);
        a->second->in.rw().push_back(b->second);
        b->second->in.rw().push_back(a->second);
        b->second->out.rw().push_back(a->second);
      }
    }

  template <class T, class I, bool directed, class ... O>
    template <typename...L> 
    void graph_generic<T,I,directed,O...>::addvert(I v, bool bin, bool bout, L&&... p)  
    {
      node* ptr = new node(v, index++, (std::forward<L>(p))...);
      verts.rw().insert(typename vertex_container_t::value_type(v, ptr));
      if(bin)
        in.rw().push_back(ptr);
      if(bout)
        out.rw().push_back(ptr);
    }


  template <class T, class I, bool directed, class ... O>
    template <typename... L> 
    graph_generic<T,I,directed,O...>::node::node(I vid, int idx, L&&... p) : id(vid), out(), in(), data(this, (std::forward<L>(p))...), index(idx)
    {
    }


  template <class T, class I, bool directed, class ... O>
    graph_generic<T,I,directed,O...>::graph_generic() : in(), out(), verts(), index(0)
  {
  }

  template <class T, class I, bool directed, class ... O>
    int graph_generic<T,I,directed,O...>::node::newid()
    {
      static int id = 0;
      return ++id;
    }

  template <class T, class I, bool directed, class ... O>
    void graph_generic<T,I,directed,O...>::node::crawl_topological(std::function<void(node*)> f)
    {
      int passid = newid();

      crawl<true, false>([=](node* n)-> bool { f(n); n->lastpass = passid; return true;}, [=](node* n)->bool{return n->lastpass != passid;} );
    }

  /**
   * g return defines whether a node should be processed at all (prevents recurse if false)(e.g. is supposed to return true once per every pass)
   * f return defines whether we should enque childs 
   * recurse  specifies whether we should search parent nodes (ensures topological pass in a dag)
   * reverse  reverses the direction of everything (usefull only in directed graphs)
   *
   * the actual work is supposed to be done by the function f
   *
   * the flow is:
   *   g ? return : ...
   *   recurse ? search parents : ...
   *   f ? enqueue childs : ...
   *
   * e.g. (with proper f and g)
   * undirected crawl<false,false>(f{return updated || first pass},g{ return true }); performs belman ford algorithm
   * directed   crawl<true, false>(f{return true}                 ,g{  true once  }); sorts a dag topologically (or respectively passes it topologically)
   * */
  template <class T, class I, bool directed, class ... O>
    template <bool recurse, bool inverse>
    void graph_generic<T,I,directed,O...>::node::crawl(std::function<bool(node*)> f, std::function<bool(node*)> g, std::queue<node*>* q)
    {
      bool root = false;
      if(q == NULL)
      {
        root = true;
        q = new std::queue<node*>();
      }
      if(!g(this))
        return;
      if(recurse && directed)
        for(auto inptr  : (!inverse ? in:out).r())
          if(inptr != NULL)
            inptr->crawl<recurse,inverse>(f, g, q);
      if(f(this))
        for(auto outptr : (!inverse ? out:in).r())
          q->push(outptr);
      if(root)
      {
        while(!q->empty())
        {
          node* n = q->front();
          q->pop();
          n->crawl<recurse,inverse>(f,g,q);
        }
        delete q;
      }
    }

};

#endif

