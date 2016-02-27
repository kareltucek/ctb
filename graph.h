#ifndef GRAPH_GUARD
#define GRAPH_GUARD

#include "defines.h"
#include "datatypes.h"
#include <functional>
#include <queue>
#include <map>
#include <string>
#include <limits>
#include <assert.h>
#include "multicont.h"
#include <set>
#include "ptrglue.h"

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
  template <class T, class I, bool directed>
    class graph_basic
    {
      protected:
        class node;
        struct edge
        {
          node* ptr;
          int frompos;
          int topos;
          GLUE_PTR(node, ptr);
        };
        MAKE(edge);

        class node
        {
          private:
            graph_basic* parent;
            typedef pair<node*, int> route;
            mutable vector<route> map;
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
            T data; /**vertex user data*/
            I id;
            int classid;
            multicont<vector<edge>> out;
            multicont<vector<edge>> in;
            T& operator->(); /**provides diect access to the data member*/
            template <bool recurse = false, bool inverse = false> void crawl(function<bool(node*)> f, function<bool(node*)> g, queue<node*>* q = NULL); /** see the documentation written in the actual code, for example see implementation of the calculate_distances() function */
            void crawl_topological(function<void(node*)> f); /** this is an overload of crawl for topological search*/
        };


        typedef map<I, node*> vertex_container_t;
        typedef vector<node*> vertex_list_t;
        int index;
        node* tovert(I);
        node* tovert(node*);
        node* tovert(node&);
        void rm_at(vector<node*>& list, node* to);
        void rm_atv(node* a,node* to);
      public:
        typedef edge edge_t;
        typedef node node_t;
        typedef I id_t;
        int classcount;
        vertex_list_t in;
        vertex_list_t out;
        vertex_container_t verts;
        graph_basic();
        ~graph_basic();
        template <typename...L> node* addvert(I v, bool in , bool out , L&&... p) ; /** v is identifier of a vetes, in and out specify whether vertex should be registered as output/input, p... are parameters to be passed to the 'data' member upon construction*/
        template <typename J> void addedge(J aid, J bid, int b_argpos = -1, int a_argpos = -1, int level = 0) ; /** self describing I believe*/
        template <typename J> void rmedge(J aid, J bid, int level = 0);
        template <typename J> void rmvert(J v);
        template <typename J> void connect_as(J v, J as, bool inputs = true, bool outputs = true);
        template <typename J> void get_edge_b_pos(J a, J b, int i);
        void calculate_distances(); /**performs bellman-ford algorithm */
        int get_dist(I a, I b, I* c = NULL) const; /** returns distance from a to b and the next vertex on path from a to b into c (if not null)*/
        static void self_test();
        void clear();
        void crawl_topological(function<void(node*)> f); /** this is an overload of crawl for topological search, may be also abbreviated as 'do f for each vertex'*/
        void factorize();
    };

  typedef graph_basic<dummy,int,true> graph_default;
  typedef graph_basic<dummy,int,true> graph_basic_default;

      template <class T, class I, bool directed>
  typename graph_basic<T,I,directed>::node* graph_basic<T,I,directed>::tovert(node& v)
  {
    return &v;
  }
  
      template <class T, class I, bool directed>
  typename graph_basic<T,I,directed>::node* graph_basic<T,I,directed>::tovert(node* v)
  {
    return v;
  }

      template <class T, class I, bool directed>
  typename graph_basic<T,I,directed>::node* graph_basic<T,I,directed>::tovert(I v)
  {
    typename vertex_container_t::iterator a = verts.find(v);
    if(a == verts.end())
      return NULL;
    return a->second;
  }

    template <class T, class I, bool directed>
  void graph_basic<T,I,directed>::factorize()
  {
    int classid = 0;
    int passid = node::newid();
    for(auto v : this->verts) //this will ensure that all components will be crawled
    {
      node* n = v.second;
      if(n->lastpass != passid)
      {
        n->template crawl<true, false>( [=](node* n)-> bool { n->classid = classid; n->lastpass = passid; return true; }, [=](node* n)->bool { return n->lastpass != passid; } );
        ++classid;
      }
    }
    classcount = classid;
  }

  template <class T, class I, bool directed>
    graph_basic<T,I,directed>::~graph_basic()
    {
      clear();
    }

  template <class T, class I, bool directed>
    void graph_basic<T,I,directed>::clear()
    {
      in.clear();
      out.clear();
      for(auto v : verts)
        delete v.second;
      verts.clear();
    }

  template <class T, class I, bool directed>
    void graph_basic<T,I,directed>::self_test()
    {
      cout << "testing graph" << endl;
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
      g.out.front()->crawl_topological([&](node* n){ assert(n->id == results[i++]);});
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

  template <class T, class I, bool directed>
    T& graph_basic<T,I,directed>::node::operator->()  
    {
      return data;
    }

  template <class T, class I, bool directed>
    int graph_basic<T,I,directed>::get_dist(I a, I b, I* c) const
    {
      auto va = verts.find(a);
      auto vb = verts.find(b);
      if(va == verts.end() || vb == verts.end())
        error( "unknown vertex id");
      //  
      if(c != NULL)
        *c = va->second->map[vb->second->index].first->id;
      return va->second->map[vb->second->index].second;
    }

  template <class T, class I, bool directed>
    typename graph_basic<T,I,directed>::node* graph_basic<T,I,directed>::node::get_path(node * n)
    {
      return map[n->index].first;
    }

  template <class T, class I, bool directed>
    void graph_basic<T,I,directed>::calculate_distances()  
    {
      if(verts.empty())
        return;
      int size = this->index;

      int passid = node::newid();

      for(auto v : verts) //this will ensure that all components will be crawled
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


  template <class T, class I, bool directed>
    bool graph_basic<T,I,directed>::node::update_distances()
    {
      bool updated = false;
      for( auto n : out)
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

  template <class T, class I, bool directed>
    bool graph_basic<T,I,directed>::node::init_map(int size)
    {
      map.clear();
      for(int i = 0; i < size; i++)
      {
        map.push_back(route(NULL, numeric_limits<int>::max()-1));
      }
      map[index].first = this;
      map[index].second = 0;
      return true;
    }

      template <class T, class I, bool directed>
  void graph_basic<T,I,directed>::rm_atv(node* a,node* to)
  {
    for( int i = 0; i < 2; i++)
    {
      auto& l = i == 0 ? a->in : a->out;
      for(int j = 0; j < l.getlevelcount(); ++j)
      {
        rm_at(l.getlevel(j), to);
      }
    }
  }

      template <class T, class I, bool directed>
  void graph_basic<T,I,directed>::rm_at(vector<node*>& list, node* to)
  {
    for (auto itr = list.begin(); itr != list.end(); ++itr) 
      if(*itr == to) //(we want to preserve argument indices)
        *itr = NULL;
  }


      template <class T, class I, bool directed>
    template <typename J>
  void graph_basic<T,I,directed>::connect_as(J _u, J _as, bool inputs, bool outputs)
  {
    node* u = tovert(_u);
    node* as = tovert(_as);

    throw "not implemented";

    if(inputs)
    {
      for( int i = 0; i < as->in.getlevelcount(); ++i)
      {
        for(int j = 0; j < as->in.getlevel(i).size(); ++j) 
        {
          auto e = as->in.getlevel(i)[j];
          addedge(e.ptr, u, e.frompos, e.topos, i);
        }
      }
    }

    if(outputs)
    {
      for( int i = 0; i < as->out.getlevelcount(); ++i)
      {
        for(int j = 0; j < as->out.getlevel(i).size(); ++j) 
        {
          auto e = as->in.getlevel(i)[j];
          addedge(u, e.ptr, e.frompos, e.topos, i);
        }
      }
    }
  }


      template <class T, class I, bool directed>
    template <typename J>
  void graph_basic<T,I,directed>::rmvert(J v)
  {
    auto a = tovert(v);
    if(a == NULL)
      error( "unknown vertex id in rmvert");
    for( int i = 0; i < 2; i++)
    {
      auto& l = i == 0 ? a->in : a->out;
      for(int j = 0; j < l.getlevelcount(); ++j)
      {
        for(auto b : l.getlevel(j))
        {
          rm_atv(b, a);
        }
      }
    }
    rm_at(in, a); 
    rm_at(out, a); 
    verts.erase(a);
    delete a; //not the safest thing to do. Now I admit that smart pointers should be used probably always.
  }

      template <class T, class I, bool directed>
    template <typename J>
  void graph_basic<T,I,directed>::rmedge(J aid, J bid, int l)
  {
    auto a = tovert(aid);
    auto b = tovert(bid);
    if(a == NULL || b == NULL)
      error( "unknown vertex id in rmedge");
    if(directed)
    {
      rm_at(a->out.getlevel(l), b);
      rm_at(b->in.getlevel(l), a);
    }
    else
    {
      rm_at(a->out.getlevel(l), b);
      rm_at(a->in.getlevel(l), b);
      rm_at(b->in.getlevel(l), a);
      rm_at(b->out.getlevel(l), a);
    }
  }

  template <class T, class I, bool directed>
    template <typename J>
    void graph_basic<T,I,directed>::addedge(J aid, J bid, int b_argpos, int a_argpos, int l)
    {
      auto a = tovert(aid);
      auto b = tovert(bid);
      if(a == NULL || b == NULL)
        error( "unknown vertex id");
      if(directed)
      {
        if(a_argpos != -1)
        {
          while(a->out.getlevel(l).size() < a_argpos+1)
            a->out.getlevel(l).push_back((edge){NULL, 0, 0});
          a->out.getlevel(l)[a_argpos] = make_edge({b, a_argpos, b_argpos});
        }
        else
          a->out.push_back(make_edge({b, a_argpos, b_argpos}));

        if(b_argpos != -1)
        {
          while(b->in.getlevel(l).size() < b_argpos+1)
            b->in.getlevel(l).push_back(make_edge({NULL, 0, 0}));
          b->in.getlevel(l)[b_argpos] = make_edge({a, a_argpos, b_argpos});
        }
        else
          b->in.push_back(make_edge({a, a_argpos, b_argpos}));
      }
      else
      {
        a->out.getlevel(l).push_back(make_edge({b, a_argpos, b_argpos}));
        a->in.getlevel(l).push_back(make_edge({b, a_argpos, b_argpos}));
        b->in.getlevel(l).push_back(make_edge({a, a_argpos, b_argpos}));
        b->out.getlevel(l).push_back(make_edge({a, a_argpos, b_argpos}));
      }
    }

  template <class T, class I, bool directed>
    template <typename...L> 
    typename graph_basic<T,I,directed>::node* graph_basic<T,I,directed>::addvert(I v, bool bin, bool bout, L&&... p)  
    {
      node* ptr = new node(v, index++, (forward<L>(p))...);
      verts.insert(typename vertex_container_t::value_type(v, ptr));
      if(bin)
        in.push_back(ptr);
      if(bout)
        out.push_back(ptr);
      return ptr;
    }


  template <class T, class I, bool directed>
    template <typename... L> 
    graph_basic<T,I,directed>::node::node(I vid, int idx, L&&... p) : id(vid), out(), in(), data(this, (forward<L>(p))...), index(idx)
    {
    }


  template <class T, class I, bool directed>
    graph_basic<T,I,directed>::graph_basic() : in(), out(), verts(), index(0)
  {
  }

  template <class T, class I, bool directed>
    int graph_basic<T,I,directed>::node::newid()
    {
      static int id = 0;
      return ++id;
    }

  template <class T, class I, bool directed>
    void graph_basic<T,I,directed>::crawl_topological(function<void(node*)> f)
    {
      int passid = node::newid();

      for(auto v : verts) //this will ensure that all components will be crawled
      {
        node* n = v.second;
        if(n->lastpass != passid)
          n->template crawl<true, false>([=](node* n)-> bool { f(n); n->lastpass = passid; return true;}, [=](node* n)->bool{return n->lastpass != passid;} );
    }
    }

  template <class T, class I, bool directed>
    void graph_basic<T,I,directed>::node::crawl_topological(function<void(node*)> f)
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
  template <class T, class I, bool directed>
    template <bool recurse, bool inverse>
    void graph_basic<T,I,directed>::node::crawl(function<bool(node*)> f, function<bool(node*)> g, queue<node*>* q)
    {
      bool root = false;
      if(q == NULL)
      {
        root = true;
        q = new queue<node*>();
      }
      if(!g(this))
        return;
      if(recurse && directed)
        for(auto inptr  : (!inverse ? in:out))
          if(inptr.ptr != NULL)
            inptr->template crawl<recurse,inverse>(f, g, q);
      if(f(this))
        for(auto outptr : (!inverse ? out:in))
          q->push(outptr.ptr);
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

