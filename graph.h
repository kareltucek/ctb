#ifndef GRAPH_GUARD
#define GRAPH_GUARD

#include "datatypes.h"
#include <functional>
#include <queue>
#include <map>
#include <string>
#include <limits>
#include <assert.h>
#include "multicont.h"
#include <set>

namespace ctb
{
  /**
   * General description
   * -------------------
   *  Graph is a general implementation of a graph structure, which has some support for handling DAG networks or for solving 'shortest path' problem in a general graph. 
   *
   *  Template arguments
   *  ------------------
   *  - T - vertex data type to be stored as the 'data' member of the node structure
   *  - I - vertex id type
   *  - directed - whether graph is to be directed. If directed = false I believe all edges will be inserted bidirectionally (and probably there will be different algorithms for distance or searching)
   *  - O... - classes to have a read-write access to the 'data' member
   *
   *  Services provided
   *  -----------------
   *  graph:
   *    - standard access
   *    - distance/path search
   *  node:
   *    - generic crawler, which starts at the node from which it was initiated. E.g. to apply function to all nodes, pick an arbitrary vertex from 'verts' or 'in' or 'out' and call crawl_topological...
   *
   *  Note that node::crawl<whichever> crawls only the reachable part of the graph while graph::crawl<whichever> guarantees entire graph to be crawled.
   * */

  //contained Type, Id type, Owner types
  template <class T, class I, bool directed, class ... O>
    class graph_basic
    {
      protected:

        class node
        {
          private:
            graph_basic* parent;
            typedef std::pair<node*, int> route;
            mutable std::vector<route> map;
            int index; /** I believe this is used as an identifier in maps*/
            int lastpass; /** is used mostly for keeping track of whether or not we've visited this node in current crawl*/
            friend class graph_basic;
            template <typename... L> node(I vid, int index, L&&... p);
            bool update_distances();
            bool init_map(int size);
            node * get_path(node * n);
          protected:
            static int newid();
          public:
            template <class A, class...B> using proxy = proxy_<A,node,graph_basic,B...>;
            proxy<T, O...> data; /**vertex user data*/
            proxy<I> id;
            proxy<int> classid;
            proxy<multicont<std::vector<node*>> > out;
            proxy<multicont<std::vector<node*>> > in;
            proxy<T,O...>& operator->(); /**provides diect access to the data member*/
            template <bool recurse = false, bool inverse = false> void crawl(std::function<bool(node*)> f, std::function<bool(node*)> g, std::queue<node*>* q = NULL); /** see the documentation written in the actual code, for example see implementation of the calculate_distances() function */
            void crawl_topological(std::function<void(node*)> f); /** this is an overload of crawl for topological search*/
        };

        typedef std::map<I, node*> vertex_container_t;
        typedef std::vector<node*> vertex_list_t;
        int index;
        node* tovert(I);
        node* tovert(node*);
        node* tovert(node&);
        void rmat(std::vector<node*>& list, node* to);
        void rmatv(node* a,node* to);
      public:
        typedef node node_t;
        typedef I id_t;
        template <class A> using proxy = proxy_<A,graph_basic>;
        proxy<int> classcount;
        proxy<vertex_list_t> in;
        proxy<vertex_list_t> out;
        proxy<vertex_container_t> verts;
        graph_basic();
        ~graph_basic();
        template <typename...L> node* addvert(I v, bool in , bool out , L&&... p) ; /** v is identifier of a vetes, in and out specify whether vertex should be registered as output/input, p... are parameters to be passed to the 'data' member upon construction*/
        template <typename J> void addedge(J aid, J bid, int b_argpos = -1, int a_argpos = -1, int level = 0) ; /** self describing I believe*/
        template <typename J> void rmedge(J aid, J bid, int level = 0);
        template <typename J> void rmvert(J v);
        void calculate_distances(); /**performs bellman-ford algorithm */
        int get_dist(I a, I b, I* c = NULL) const; /** returns distance from a to b and the next vertex on path from a to b into c (if not null)*/
        static void self_test();
        void clear();
        void crawl_topological(std::function<void(node*)> f); /** this is an overload of crawl for topological search, may be also abbreviated as 'do f for each vertex'*/
        void factorize();
    };

  typedef graph_basic<dummy,int,true> graph_default;
  typedef graph_basic<dummy,int,true> graph_basic_default;

      template <class T, class I, bool directed, class ... O>
  typename graph_basic<T,I,directed,O...>::node* graph_basic<T,I,directed,O...>::tovert(node& v)
  {
    return &v;
  }
  
      template <class T, class I, bool directed, class ... O>
  typename graph_basic<T,I,directed,O...>::node* graph_basic<T,I,directed,O...>::tovert(node* v)
  {
    return v;
  }

      template <class T, class I, bool directed, class ... O>
  typename graph_basic<T,I,directed,O...>::node* graph_basic<T,I,directed,O...>::tovert(I v)
  {
    typename vertex_container_t::iterator a = verts.rw().find(v);
    if(a == verts.rw().end())
      return NULL;
    return a->second;
  }

    template <class T, class I, bool directed, class ... O>
  void graph_basic<T,I,directed,O...>::factorize()
  {
    int classid = 0;
    int passid = node::newid();
    for(auto v : this->verts.r()) //this will ensure that all components will be crawled
    {
      node* n = v.second;
      if(n->lastpass != passid)
      {
        n->template crawl<true, false>( [=](node* n)-> bool { n->classid.rw() = classid; n->lastpass = passid; return true; }, [=](node* n)->bool { return n->lastpass != passid; } );
        ++classid;
      }
    }
    classcount.rw() = classid;
  }

  template <class T, class I, bool directed, class ... O>
    graph_basic<T,I,directed,O...>::~graph_basic()
    {
      clear();
    }

  template <class T, class I, bool directed, class ... O>
    void graph_basic<T,I,directed,O...>::clear()
    {
      in.rw().clear();
      out.rw().clear();
      for(auto v : verts.rw())
        delete v.second;
      verts.rw().clear();
    }

  template <class T, class I, bool directed, class ... O>
    void graph_basic<T,I,directed,O...>::self_test()
    {
      graph_basic<dummy, int, true> g;
      g.addvert(1, true, false);
      g.addvert(2, false, false);
      g.addvert(3, false, true);
      g.addvert(4, false, true);
      g.addedge(1,2);
      g.addedge(2,3);
      g.addedge(2,4);
      int i = 0;
      int results[] = {1,2,3,4};
      g.out->front()->crawl_topological([&](node* n){ assert(n->id.r() == results[i++]);});
      g.calculate_distances();
      assert(g.get_dist(1,4) == 2);
      //  
      graph_basic<dummy, int, false> h;
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
    graph_basic<T,I,directed,O...>::node::proxy<T,O...>& graph_basic<T,I,directed,O...>::node::operator->()  
    {
      return data;
    }

  template <class T, class I, bool directed, class ... O>
    int graph_basic<T,I,directed,O...>::get_dist(I a, I b, I* c) const
    {
      auto va = verts->find(a);
      auto vb = verts->find(b);
      if(va == verts->end() || vb == verts->end())
        error( "unknown vertex id");
      //  
      if(c != NULL)
        *c = va->second->map[vb->second->index].first->id.r();
      return va->second->map[vb->second->index].second;
    }

  template <class T, class I, bool directed, class ... O>
    typename graph_basic<T,I,directed,O...>::node* graph_basic<T,I,directed,O...>::node::get_path(node * n)
    {
      return map[n->index].first;
    }

  template <class T, class I, bool directed, class ... O>
    void graph_basic<T,I,directed,O...>::calculate_distances()  
    {
      if(verts->empty())
        return;
      int size = this->index;

      int passid = node::newid();

      for(auto v : verts.r()) //this will ensure that all components will be crawled
      {
        node* n = v.second;
        if(n->lastpass != passid)
        {
          //TODO: a more explicit explanation should probably come here...
      n->template crawl<true, true>([=](node* n)->bool{ n->init_map(size); n->lastpass = passid; return true;}, [=](node*n)->bool{return n->map.size() != size;} );
      n->template crawl<directed, true>([=](node* n)->bool{ return n->update_distances();}, [=](node*n)->bool{return true;} );
    }
    }
    }


  template <class T, class I, bool directed, class ... O>
    bool graph_basic<T,I,directed,O...>::node::update_distances()
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
    bool graph_basic<T,I,directed,O...>::node::init_map(int size)
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
  void graph_basic<T,I,directed,O...>::rmatv(node* a,node* to)
  {
    for( int i = 0; i < 2; i++)
    {
      auto& l = i == 0 ? a->in.rw() : a->out.rw();
      for(int j = 0; j < l.getlevelcount(); ++j)
      {
        rmat(l.getlevel(j), to);
      }
    }
  }

      template <class T, class I, bool directed, class ... O>
  void graph_basic<T,I,directed,O...>::rmat(std::vector<node*>& list, node* to)
  {
    for (auto itr = list.begin(); itr != list.end(); ++itr) 
      if(*itr == to) //(we want to preserve argument indices)
        *itr = NULL;
  }

      template <class T, class I, bool directed, class ... O>
    template <typename J>
  void graph_basic<T,I,directed,O...>::rmvert(J v)
  {
    auto a = tovert(v);
    if(a == NULL)
      error( "unknown vertex id in rmvert");
    for( int i = 0; i < 2; i++)
    {
      auto& l = i == 0 ? a->in.rw() : a->out.rw();
      for(int j = 0; j < l.getlevelcount(); ++j)
      {
        for(auto b : l.getlevel(j))
        {
          rmatv(b, a);
        }
      }
    }
    rmat(in.rw(), a); 
    rmat(out.rw(), a); 
    verts.rw().erase(a);
    delete a; //not the safest thing to do. Now I admit that smart pointers should be used probably always.
  }

      template <class T, class I, bool directed, class ... O>
    template <typename J>
  void graph_basic<T,I,directed,O...>::rmedge(J aid, J bid, int l)
  {
    auto a = tovert(aid);
    auto b = tovert(bid);
    if(a == NULL || b == NULL)
      error( "unknown vertex id in rmedge");
    if(directed)
    {
      rmat(a->out.rw().getlevel(l), b);
      rmat(b->in.rw().getlevel(l), a);
    }
    else
    {
      rmat(a->out.rw().getlevel(l), b);
      rmat(a->in.rw().getlevel(l), b);
      rmat(b->in.rw().getlevel(l), a);
      rmat(b->out.rw().getlevel(l), a);
    }
  }

  template <class T, class I, bool directed, class ... O>
    template <typename J>
    void graph_basic<T,I,directed,O...>::addedge(J aid, J bid, int b_argpos, int a_argpos, int l)
    {
      auto a = tovert(aid);
      auto b = tovert(bid);
      if(a == NULL || b == NULL)
        error( "unknown vertex id");
      if(directed)
      {
        if(a_argpos != -1)
        {
          while(a->out.rw().getlevel(l).size() < a_argpos+1)
            a->out.rw().getlevel(l).push_back(NULL);
          a->out.rw().getlevel(l)[a_argpos] = b;
        }
        else
          a->out.rw().push_back(b);

        if(b_argpos != -1)
        {
          while(b->in.rw().getlevel(l).size() < b_argpos+1)
            b->in.rw().getlevel(l).push_back(NULL);
          b->in.rw().getlevel(l)[b_argpos] = a;
        }
        else
          b->in.rw().push_back(a);
      }
      else
      {
        a->out.rw().getlevel(l).push_back(b);
        a->in.rw().getlevel(l).push_back(b);
        b->in.rw().getlevel(l).push_back(a);
        b->out.rw().getlevel(l).push_back(a);
      }
    }

  template <class T, class I, bool directed, class ... O>
    template <typename...L> 
    typename graph_basic<T,I,directed,O...>::node* graph_basic<T,I,directed,O...>::addvert(I v, bool bin, bool bout, L&&... p)  
    {
      node* ptr = new node(v, index++, (std::forward<L>(p))...);
      verts.rw().insert(typename vertex_container_t::value_type(v, ptr));
      if(bin)
        in.rw().push_back(ptr);
      if(bout)
        out.rw().push_back(ptr);
      return ptr;
    }


  template <class T, class I, bool directed, class ... O>
    template <typename... L> 
    graph_basic<T,I,directed,O...>::node::node(I vid, int idx, L&&... p) : id(vid), out(), in(), data(this, (std::forward<L>(p))...), index(idx)
    {
    }


  template <class T, class I, bool directed, class ... O>
    graph_basic<T,I,directed,O...>::graph_basic() : in(), out(), verts(), index(0)
  {
  }

  template <class T, class I, bool directed, class ... O>
    int graph_basic<T,I,directed,O...>::node::newid()
    {
      static int id = 0;
      return ++id;
    }

  template <class T, class I, bool directed, class ... O>
    void graph_basic<T,I,directed,O...>::crawl_topological(std::function<void(node*)> f)
    {
      int passid = node::newid();

      for(auto v : verts.r()) //this will ensure that all components will be crawled
      {
        node* n = v.second;
        if(n->lastpass != passid)
          n->template crawl<true, false>([=](node* n)-> bool { f(n); n->lastpass = passid; return true;}, [=](node* n)->bool{return n->lastpass != passid;} );
    }
    }

  template <class T, class I, bool directed, class ... O>
    void graph_basic<T,I,directed,O...>::node::crawl_topological(std::function<void(node*)> f)
    {
      int passid = node::newid();

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
   * directed   crawl<true, false>(f{return true}                 ,g{  true once  }); sorts a dag topologically (or respectively passes it in topological ordering)
   * */
  template <class T, class I, bool directed, class ... O>
    template <bool recurse, bool inverse>
    void graph_basic<T,I,directed,O...>::node::crawl(std::function<bool(node*)> f, std::function<bool(node*)> g, std::queue<node*>* q)
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
            inptr->template crawl<recurse,inverse>(f, g, q);
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

