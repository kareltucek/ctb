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
#include <utility>

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
  template <class T, class U, class I, bool directed>
    class graph_basic
    {
      protected:
        class node;
        class edge
        {
          protected:
            friend class graph_basic;
            template <typename... L> edge(node* from, node* to, int fp, int tp, int l, L&&... p);
          public:
            node* from;
            node* to;
            int from_pos;
            int to_pos;
            int layer;
            int order;

            U data;
            
          //GLUE_PTR(node, ptr);
        };
        //MAKE(edge);

        class node
        {
          protected:
            graph_basic* parent;
            typedef pair<node*, int> route;
            mutable vector<route> map;
            int index; /** I believe I am using this as an identifier in maps*/
            int lastpass; /** is used mostly for keeping track of whether or not we've visited this node in current crawl*/
            friend class graph_basic;
            template <typename... L> node(I vid, int index, L&&... p);
            bool update_distances();
            bool init_map(int size);
            node * get_path(node * n);
            edge* inout_at(int i, bool inswitch, bool check_uniqueness = true, bool alllayers = false);
          public:
            T data; /**vertex user data*/
            I id;
            int classid;
            imp_cont<vector<edge*>> out;
            imp_cont<vector<edge*>> in;
            edge* in_at(int i, bool check_uniqueness = true, bool alllayers = false);
            edge* out_at(int i, bool check_uniqueness = true, bool alllayers = false);
            template <bool recurse = false, bool inverse = false> void crawl(function<bool(node*)> f, function<bool(node*)> g, std::vector<int> layers = {0}, queue<node*>* q = NULL, bool root = true); /** see the documentation written in the actual code, for example see implementation of the calculate_distances() function */
            template <bool Inverted = false> void crawl_topological(function<void(node*)> f, std::vector<int> layers = {0}); /** this is an overload of crawl for topological search*/
            static int newid();
            int colourmark; /*public version of lastpass - may be used for construction of arbitrary crawls*/
        };


        typedef map<I, node*> vertex_container_t;
        typedef vector<node*> vertex_list_t;
        int index;
        node* tovert(I);
        node* tovert(node*);
        node* tovert(node&);
        string toname(I);
        string toname(node*);
        string toname(node&);
        void rm_at(vector<node*>& list, node* to);
        //void rm_at(vector<edge>& list, node* to);
        //void rm_atv(node* a,node* to);
      protected:
        int classids;
        int classids_current;
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
        template <typename...L> node* add_vert(I v, bool in , bool out , L&&... p) ; /** v is identifier of a vetes, in and out specify whether vertex should be registered as output/input, p... are parameters to be passed to the 'data' member upon construction*/
        template <typename J, typename K, typename...L> edge* add_edge_ex(J aid, K bid, int b_argpos, int a_argpos, int layer, L&&...) ;
        template <typename J, typename K, typename...L> edge* add_edge(J aid, K bid, int b_argpos = -1, int a_argpos = -1, int layer = 0) ; /** self describing I believe*/
        //template <typename J> void rm_edge(J aid, J bid, int layer = 0);
        template <typename J> void rm_vert(J v);
        template <typename J, typename K> void connect_as(J v, K as, bool inputs = true, bool outputs = true);
        void rm_edge(edge* e);
        void calculate_distances(); /**performs bellman-ford algorithm */
        int get_dist(I a, I b, I* c = NULL) const; /** returns distance from a to b and the next vertex on path from a to b into c (if not null)*/
        static void self_test();
        void clear();
        template <typename J> node* get_vert(J v);
        template <bool Inverted = false> void crawl_topological(function<void(node*)> f, std::vector<int> layers = {0}); /** this is an overload of crawl for topological search, may be also abbreviated as 'do f for each vertex'*/
        template <bool Inverted = false> void crawl_topological_b(function<bool(node*)> f, std::vector<int> layers = {0}); 
        //node* find_cycle_begin(vector<int> at_layers = {0}, vector<int> with_topology = {0,1});
        //bool cycle_exists(node* n, const vector<int>& at_layers = {0}, set<node*> vertices = {}, int remains = -1);
        bool cycle_exists(node** n = NULL);
        void factorize();
        void update_factor();
        int size();
    };

  typedef graph_basic<dummy,dummy,int,true> graph_default;
  typedef graph_basic<dummy,dummy,int,true> graph_basic_default;

  template <class T, class U, class I, bool directed>
    template <typename... L> 
    graph_basic<T,U,I,directed>::edge::edge(node* f, node* t, int tp, int fp, int l, L&&... p) : from(f), to(t), to_pos(tp), from_pos(fp), layer(l), order(-1), data(this, (forward<L>(p))...)
    {
    }


  template <class T, class U, class I, bool directed>
    template <typename J>
    typename graph_basic<T,U,I,directed>::node* graph_basic<T,U,I,directed>::get_vert(J v)  
    {
      return tovert(v);
    }

  template <class T, class U, class I, bool directed>
    bool graph_basic<T,U,I,directed>::cycle_exists(node** n)
    {
      update_factor();
      for(const auto& v : verts)
      {
        int maxin = 0;
        int minout = 100000000;

        for(int l : {0,1})
          for(auto e : v.second->in.get_layer(l))
            maxin = max(e->order, maxin);
        for(int l : {0,1})
          for(auto e : v.second->out.get_layer(l))
            minout = min(e->order, minout);

        if(maxin >= minout)
        {
          if(n != NULL)
            *n = v.second;
          return true;
        }
      }
      return false;
    }

  template <class T, class U, class I, bool directed>
    void graph_basic<T,U,I,directed>::update_factor()
    {
      if(classids != classids_current)
        factorize();
    }

  template <class T, class U, class I, bool directed>
    int graph_basic<T,U,I,directed>::size()
    {
      return verts.size();
    }

  template <class T, class U, class I, bool directed>
    typename graph_basic<T,U,I,directed>::node* graph_basic<T,U,I,directed>::tovert(node& v)
    {
      return &v;
    }

  template <class T, class U, class I, bool directed>
    typename graph_basic<T,U,I,directed>::node* graph_basic<T,U,I,directed>::tovert(node* v)
    {
      return v;
    }

  template <class T, class U, class I, bool directed>
    typename graph_basic<T,U,I,directed>::node* graph_basic<T,U,I,directed>::tovert(I v)
    {
      typename vertex_container_t::iterator a = verts.find(v);
      if(a == verts.end())
        return NULL;
      return a->second;
    }

  template <class T, class U, class I, bool directed>
    string graph_basic<T,U,I,directed>::toname(I v)
    {
      return ctb::to_string(v);
    }

  template <class T, class U, class I, bool directed>
    string graph_basic<T,U,I,directed>::toname(node& v)
    {
      return "<unknown: passed by reference>";
    }

  template <class T, class U, class I, bool directed>
    string graph_basic<T,U,I,directed>::toname(node* v)
    {
      return "<unknown: passed by pointer>";
    }

  template <class T, class U, class I, bool directed>
    void graph_basic<T,U,I,directed>::factorize()
    {
      int classid = 0;

      //note that we *do* want to have the partitions sorted by the topological ordering of vertices they contain
      vector<int> minouts;
      vector<int> maxins; //less than corresponding minouts
      auto make_class = [&](node* v)
      {
        if(v->classid == -1)
        {
          int mine = 100000+classid; //want these unique - edge order will be unique with respect to vertices
          v->template crawl<true, false>( 
              [&](node* n)-> bool { for(auto e : n->out.get_layer(1)) mine = min(e->order, mine); return true; }, 
              [&](node* n)->bool { bool res = n->classid == -1; n->classid = classid; return res; } 
              );
          minouts.push_back(mine);
          maxins.push_back(-classid);
          ++classid;
        }
      };

      //since the crawl is not reentrant, we will need to use positivness of classid as an indicator instead of lastpass
      int i = 0;
      this->crawl_topological([&](node* n){
          n->classid = -1;  
          i++;
          for(int l : {0,1})
             for(auto e : n->in.get_layer(l))
               e->order = i;
         }, {0,1}); 
      //then we go topologically through the graph and form partitions from every not-yet-assigned vertex, also we construct minimum factor-output values
      this->crawl_topological(make_class, {0,1}); 

      this->crawl_topological([&](node* n){ for(auto e: n->in.get_layer(1)) if(e->order < minouts[n->classid]) maxins[n->classid] = max(maxins[n->classid], e->order);}, {0,1}); 

      //now we try to translate the found ids into some nicer ordering (although this is not a solution)
      map<int,int> sortmap;
      int translation[classid];
      for (auto j = 0; j < maxins.size(); j++) 
        sortmap[maxins[j]] = j;
      i = 0;
      for(auto itr : sortmap)
        translation[itr.second] = i++;

      auto layers = vector<int>{0,1};
      this->crawl_topological([&translation](node* n){ n->classid = translation[n->classid];}, layers); 

      classcount = classid;
      classids = classids_current;
    }

  template <class T, class U, class I, bool directed>
    graph_basic<T,U,I,directed>::~graph_basic()
    {
      clear();
    }

  template <class T, class U, class I, bool directed>
    void graph_basic<T,U,I,directed>::clear()
    {
      in.clear();
      out.clear();
      set<node*> vv;
      for(auto v : verts)
        vv.insert(v.second);
      for(auto* v : vv)
        rm_vert(v);
      classcount = 0;
      index = 0;
      verts.clear();
      classids_current++; 
    }

  template <class T, class U, class I, bool directed>
    void graph_basic<T,U,I,directed>::self_test()
    {
      cout << "testing graph" << endl;
      graph_basic<dummy, dummy, int, true> g;
      g.add_vert(1, true, false);
      g.add_vert(2, false, false);
      g.add_vert(3, false, true);
      g.add_vert(4, false, true);
      g.add_edge(1,2);
      g.add_edge(2,3);
      g.add_edge(2,4);
      int i = 0;
      int results[] = {1,2,3,4};
      g.out.front()->crawl_topological([&](node* n){ assert(n->id == results[i++]);});
      g.calculate_distances();
      assert(g.get_dist(1,4) == 2);
      assert(g.cycle_exists() == false);
      //  
      graph_basic<dummy, dummy, int, false> h;
      h.add_vert(1, false, false);
      h.add_vert(2, false, false);
      h.add_vert(3, false, false);
      h.add_vert(4, false, false);
      h.add_edge(1,2);
      h.add_edge(2,3);
      h.add_edge(3,4);
      h.add_edge(1,4);
      h.calculate_distances(); //implicit test of crawling functions
      assert(h.get_dist(1,3) == 2);
      assert(h.get_dist(1,4) == 1);

      i = 0;
      graph_basic<dummy, dummy, int, true> k;
      k.add_vert(1, false, false);
      auto v = k.add_vert(2, false, false);
      k.add_vert(3, false, true);
      k.add_edge(1,2);
      k.add_edge(2,3);
      k.crawl_topological([&](node* n){ i++;});
      assert( i == 3);
      k.rm_vert(v);
      k.crawl_topological([&](node* n){ i++;});
      assert( i == 5);

    }

  template <class T, class U, class I, bool directed>
    int graph_basic<T,U,I,directed>::get_dist(I a, I b, I* c) const
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

  template <class T, class U, class I, bool directed>
    typename graph_basic<T,U,I,directed>::edge* graph_basic<T,U,I,directed>::node::out_at(int i,  bool check_uniqueness, bool layers)
    {
      return inout_at(i, false, check_uniqueness, layers);
    }

  template <class T, class U, class I, bool directed>
    typename graph_basic<T,U,I,directed>::edge* graph_basic<T,U,I,directed>::node::in_at(int i,  bool check_uniqueness, bool layers)
    {
      return inout_at(i, true, check_uniqueness, layers);
    }

  template <class T, class U, class I, bool directed>
    typename graph_basic<T,U,I,directed>::edge* graph_basic<T,U,I,directed>::node::inout_at(int i, bool inswitch, bool check_uniqueness, bool layers)
    {
      edge* res = NULL;
      auto& cnt = inswitch ? in : out;
      for (auto l = 0; l < cnt.get_layercount() && (l == 0 || layers); l++) 
      {
        for(auto v : cnt.get_layer(l))
        {
         int p = inswitch ? v->to_pos : v->from_pos;
          if(p == i)
          {
            if(res == NULL)
              res = v;
            else if(check_uniqueness)
              error("node has multiple incoming or outgoing edges at the same position");
          }
        }
      }
      if(check_uniqueness && res == NULL)
        error("incoming or ougoing edge not found when expected!");
      return res;
    }

  template <class T, class U, class I, bool directed>
    typename graph_basic<T,U,I,directed>::node* graph_basic<T,U,I,directed>::node::get_path(node * n)
    {
      return map[n->index].first;
    }

  template <class T, class U, class I, bool directed>
    void graph_basic<T,U,I,directed>::calculate_distances()  
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
          //first we initialize a routing table (the "map"). I.e. fill it by empty records with infinite distances.
          n->template crawl<true, true>(
              [=](node* n)->bool{ n->init_map(size); n->lastpass = passid; return true;}, 
              [=](node*n)->bool{return n->map.size() != size;} 
              );
          //then we update the distances. If we improved any route, the f function will add all neighbours to the queue. The g function lets the algorithm work undisturbed.
          n->template crawl<directed, true>(
              [=](node* n)->bool{ return n->update_distances();}, 
              [=](node*n)->bool{return true;} 
              );
        }
      }
    }


  template <class T, class U, class I, bool directed>
    bool graph_basic<T,U,I,directed>::node::update_distances()
    {
      bool updated = false;
      for( auto n : out)
      {
        for( int i = 0; i < n->to->map.size(); ++i)
        {
          if( n->to->map[i].second + 1 < map[i].second)
          {
            //map[i] = n->to->map[i];
            map[i].second = n->to->map[i].second + 1;
            map[i].first = n->to;
            updated = true;
          }
        }
      }
      return updated;
    }

  template <class T, class U, class I, bool directed>
    bool graph_basic<T,U,I,directed>::node::init_map(int size)
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

  //    template <class T, class U, class I, bool directed>
  //void graph_basic<T,U,I,directed>::rm_atv(node* a,node* to)
  //{
  //  for( int i = 0; i < 2; i++)
  //  {
  //    auto& l = i == 0 ? a->in : a->out;
  //    for(int j = 0; j < l.get_layercount(); ++j)
  //    {
  //      rm_at(l.get_layer(j), to);
  //    }
  //  }
  //}

  //    template <class T, class U, class I, bool directed>
  //void graph_basic<T,U,I,directed>::rm_at(vector<edge>& list, node* to)
  //{
  //  for (auto itr = list.begin(); itr != list.end(); ++itr) 
  //    if(itr->ptr == to) //(we want to preserve argument indices)
  //      itr->ptr = NULL;
  //}


  /*
  template <class T, class U, class I, bool directed>
    bool graph_basic<T,U,I,directed>::cycle_exists(node* n, const vector<int>& at_layers, set<node*> vertices, int remains)
    {
      if(vertices.find(n) != vertices.end())
        return true;
      if(remains == 0)
        return false;
      if(remains < 0)
      {
        remains = vertices.size()+2;
        vertices.insert(n);
      }
      set<node*> newverts;
      for(auto v : vertices)
      {
        for(auto l : at_layers)
        {
          for(auto e : v->out.get_layer(l))
          {
            newverts.insert(e->to);
          }
        }
      }
      return cycle_exists(n, at_layers, newverts, remains-1);
    }

  template <class T, class U, class I, bool directed>
    typename graph_basic<T,U,I,directed>::node* graph_basic<T,U,I,directed>::find_cycle_begin(vector<int> at_layers, vector<int> topology_layers)
    {
      node* found = NULL;
      auto start_bfs = [&](node* n)->bool
      {
        if(cycle_exists(n, at_layers))
        {
          found = n;
          return false;
        }
        return true;
      };

      crawl_topological(start_bfs, topology_layers);

      return found;
    }
    */

  template <class T, class U, class I, bool directed>
    void graph_basic<T,U,I,directed>::rm_at(vector<node*>& list, node* to)
    {
      for (auto itr = list.begin(); itr != list.end(); ++itr) 
        if(*itr == to) //(we want to preserve argument indices)
          *itr = NULL;
      classids_current++; 
    }

  template <class T, class U, class I, bool directed>
    template <typename J, typename K>
    void graph_basic<T,U,I,directed>::connect_as(J _u, K _as, bool inputs, bool outputs)
    {
      node* u = tovert(_u);
      node* as = tovert(_as);

      if(inputs)
      {
        for( int i = 0; i < as->in.get_layercount(); ++i)
        {
          for(int j = 0; j < as->in.get_layer(i).size(); ++j) 
          {
            auto e = as->in.get_layer(i)[j];
            add_edge(e->from, u, e->to_pos, e->from_pos, i);
          }
        }
      }

      if(outputs)
      {
        for( int i = 0; i < as->out.get_layercount(); ++i)
        {
          for(int j = 0; j < as->out.get_layer(i).size(); ++j) 
          {
            auto e = as->out.get_layer(i)[j];
            add_edge(u, e->to, e->to_pos, e->from_pos, i);
          }
        }
      }
      classids_current++; 
    }


  template <class T, class U, class I, bool directed>
    template <typename J>
    void graph_basic<T,U,I,directed>::rm_vert(J v)
    {
      auto a = tovert(v);
      if(a == NULL)
        error( "unknown vertex id in rm_vert");
      set<edge*> edges;
      for( int i = 0; i < 2; i++)
      {
        auto& l = i == 0 ? a->in : a->out;
        for(int j = 0; j < l.get_layercount(); ++j)
        {
          for(auto b : l.get_layer(j))
          {
              edges.insert(b);
          }
        }
      }
      for(auto b : edges)
        rm_edge(b);

      rm_at(in, a); 
      rm_at(out, a); 
      verts.erase(a->id);
      delete a; //not the safest thing to do. Now I admit that smart pointers should be used probably always.
      classids_current++; 
    }

  template <class T, class U, class I, bool directed>
    void graph_basic<T,U,I,directed>::rm_edge(edge* v)
    {
      int removed_at = 0;
      for( int i = 0; i < 2; i++)
      {
        auto& l = i == 0 ? v->to->in.get_layer(v->layer) : v->from->out.get_layer(v->layer);
        for(auto itr = l.begin(); itr != l.end(); ++itr)
        {
          if (*itr == v) 
          {
            removed_at++;
            l.erase(itr);
            break;    
          }
        }
      }
      assert(removed_at == 2);

      delete v;
      classids_current++; 
    }

  template <class T, class U, class I, bool directed>
    template <typename J, typename K, typename...L>
    typename graph_basic<T,U,I,directed>::edge* graph_basic<T,U,I,directed>::add_edge(J a, K b, int b_argpos, int a_argpos, int l)
    {
        return  add_edge_ex(a, b, b_argpos, a_argpos, l);
    }

  template <class T, class U, class I, bool directed>
    template <typename J, typename K, typename...L>
    typename graph_basic<T,U,I,directed>::edge* graph_basic<T,U,I,directed>::add_edge_ex(J aid, K bid, int b_argpos, int a_argpos, int l, L&&... p)
    {
      classids_current++; 
      auto a = tovert(aid);
      auto b = tovert(bid);

      if(a == NULL)
        error( string("unknown vertex in add_edge: ") + toname(aid) + " (-> " + toname(bid) + ")");
      if (b == NULL)
        error( string("unknown vertex in add_edge: ") + toname(bid) + " (<- " + toname(aid) + ")");
      if(directed)
      {
        edge* e = new edge(a, b, b_argpos, a_argpos, l, (forward<L>(p))...);
        a->out.get_layer(l).push_back(e);
        b->in.get_layer(l).push_back(e);
        return e;
      }
      else
      {
        edge* e = new edge(a, b, b_argpos, a_argpos, l,  (forward<L>(p))...);
        edge* f = new edge(b, a, a_argpos, b_argpos, l,  (forward<L>(p))...);
        a->out.get_layer(l).push_back(e);
        b->in.get_layer(l).push_back(e);
        b->out.get_layer(l).push_back(f);
        a->in.get_layer(l).push_back(f);
        return e;
      }
    }

  template <class T, class U, class I, bool directed>
    template <typename...L> 
    typename graph_basic<T,U,I,directed>::node* graph_basic<T,U,I,directed>::add_vert(I v, bool bin, bool bout, L&&... p)  
    {
      node* ptr = new node(v, index++, (forward<L>(p))...);
      verts.insert(typename vertex_container_t::value_type(v, ptr));
      if(bin)
        in.push_back(ptr);
      if(bout)
        out.push_back(ptr);
      classids_current++; 
      return ptr;
    }

  template <class T, class U, class I, bool directed>
    template <typename... L> 
    graph_basic<T,U,I,directed>::node::node(I vid, int idx, L&&... p) : id(vid), out(), in(), data(this, (forward<L>(p))...), index(idx), classid(-1)
    {
    }


  template <class T, class U, class I, bool directed>
    graph_basic<T,U,I,directed>::graph_basic() : in(), out(), verts(), index(0), classids(0), classids_current(0)
  {
  }

  template <class T, class U, class I, bool directed>
    int graph_basic<T,U,I,directed>::node::newid()
    {
      static int id = 0;
      return ++id;
    }

  template <class T, class U, class I, bool directed>
    template <bool Inverted>
    void graph_basic<T,U,I,directed>::crawl_topological_b(function<bool(node*)> f, std::vector<int> layers)
    {
      set<node*> visited;
      bool cont = true;
      for(auto v : verts) //this will ensure that all components will be crawled (not neccessarily in topological order!)
      {
        node* n = v.second;
        if(visited.find(n) == visited.end())
        {
          n->template crawl<true, Inverted>(
              [&](node* n)-> bool { cont = f(n); return cont;}, 
              [&](node* n)->bool{bool res = visited.find(n) == visited.end(); visited.insert(n); return res && cont; } ,
              layers
              );
        }
      }
    }

  template <class T, class U, class I, bool directed>
    template <bool Inverted>
    void graph_basic<T,U,I,directed>::crawl_topological(function<void(node*)> f, std::vector<int> layers)
    {
      crawl_topological_b<Inverted>([=](node* n)->bool{ f(n); return true;}, layers);
    }

  template <class T, class U, class I, bool directed>
    template <bool Inverted>
    void graph_basic<T,U,I,directed>::node::crawl_topological(function<void(node*)> f, std::vector<int> layers)
    {
      int passid = node::newid();

      crawl<true, Inverted>([=](node* n)-> bool { f(n); n->lastpass = passid; return true;}, [=](node* n)->bool{return n->lastpass != passid;}, layers);
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
   *   g ? ... : return
   *   recurse ? search parents : ...
   *   f ? enqueue childs : ...
   *
   * e.g. (with proper f and g)
   * undirected crawl<false,false>(f{return updated || first pass},g{ return true }); performs belman ford algorithm
   * directed   crawl<true, false>(f{return true}                 ,g{  true once  }); sorts a dag topologically (or respectively passes it in topological ordering)
   * */
  template <class T, class U, class I, bool directed>
    template <bool recurse, bool inverse>
    void graph_basic<T,U,I,directed>::node::crawl(function<bool(node*)> f, function<bool(node*)> g, std::vector<int> layers, queue<node*>* q, bool root)
    {
      bool myqueue = false;
      if(q == NULL)
      {
        myqueue = true;
        q = new queue<node*>();
      }
      if(!g(this))
      {
        return;
      }
      if(recurse && directed)
      {
        for(int l : layers)
        {
          for(auto inptr  : (inverse ? out:in).get_layer(l))
          {
            auto ptr = inverse ? inptr->to : inptr->from;
            ptr->template crawl<recurse,inverse>(f, g, layers, q, false);
          }
        }
      }
      if(f(this))
      {
        for(int l : layers)
        {
          for(auto outptr : (inverse ? in:out).get_layer(l))
          {
            auto ptr = inverse ? outptr->from : outptr->to;
            q->push(ptr);
          }
        }
      }
      if(root)
      {
        while(!q->empty())
        {
          node* n = q->front();
          q->pop();
          n->crawl<recurse,inverse>(f,g,layers,q, false);
        }
        if(myqueue)
          delete q;
      }
    }
};

#endif

