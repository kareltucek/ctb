#ifndef CF_GUARD
#define CF_GUARD

#include "defines.h"

namespace ctb
{
  /**
   * G is a graph type. (That member graph of a generator)
   * */
  template <class G>
    class cf_transform {
      public: 
        static string get_name(){ return "cf";};
      private:
        typedef typename G::graph_t::node_t node;
        typedef typename G::graph_t::edge_t edge;
        typedef typename G::vid_t vid_t;
        typedef typename G::opid_t opid_t;

        int new_buff_id() {
          static int id = 0;
          return ++id;
        };


        void transform_split(G& generator, node* n, const string& name)
        {
          const auto& expansion = generator.instab.find_expansion(get_name(), name, n->data.get_typespec());
          const vector<opid_t>& ids = expansion.arguments;

          if(ids.size() < 3)
            error("not enough arguments for split expansion in cf transform");

          int buff_id = new_buff_id();

          opid_t split_st = ids[0];
          opid_t split_ld_true = ids[1];
          opid_t split_ld_false = ids[2];

          auto st = generator.addvert(n->id + "_split_st_" + ctb::to_string(buff_id), split_st, buff_id);
          auto ldtrue = generator.addvert(n->id + "_split_ldtrue_" + ctb::to_string(buff_id), split_ld_true, buff_id);
          auto ldfalse = generator.addvert(n->id + "_split_ldfalse_" + ctb::to_string(buff_id), split_ld_false, buff_id);

          generator.graph.addedge(n->in_at(0)->from, st, 0, n->in_at(0)->frompos, 0);
          for( auto outvert : n->out)
          {
            if(outvert->frompos == 0)
              generator.graph.addedge(ldtrue, outvert->to, outvert->topos, 0, 0);
            if(outvert->frompos == 1)
              generator.graph.addedge(ldfalse, outvert->to, outvert->topos, 0, 0);
          }
          generator.graph.addedge(st, ldtrue, 0, 0, 1);
          generator.graph.addedge(st, ldfalse, 0, 0, 1);

          generator.graph.rmvert(n);
        };

        void transform_merge(G& generator, node* n, const string& name)
        {
          const auto& expansion = generator.instab.find_expansion(get_name(), name, n->data.get_typespec());
          const vector<opid_t>& ids = expansion.arguments;

          if(ids.size() < 4)
            error("not enough arguments for merge expansion in cf transform");

          int buff_id = new_buff_id();

          opid_t merge_ld = ids[0];
          opid_t merge_st_true = ids[1];
          opid_t merge_st_false = ids[2];
          opid_t buff_st = ids[3];

          node* ld = generator.addvert(n->id + "_merge_ld_" + ctb::to_string(buff_id), merge_ld, buff_id);
          node* sttrue = generator.addvert(n->id + "_merge_sttrue_" + ctb::to_string(buff_id), merge_st_true, buff_id);
          node* stfalse = generator.addvert(n->id + "_merge_stfalse_" + ctb::to_string(buff_id), merge_st_false, buff_id);
          node* stbuff = generator.addvert(n->id + "_merge_stbuff_" + ctb::to_string(buff_id), buff_st, buff_id);

          generator.graph.addedge(n->in_at(0)->from, sttrue,  0, n->in_at(0)->frompos, 0);
          generator.graph.addedge(n->in_at(1)->from, stfalse, 0, n->in_at(1)->frompos, 0);
          generator.graph.addedge(n->in_at(2)->from, stbuff,  0, n->in_at(2)->frompos, 0);
          generator.graph.connect_as(ld, n, false, true);

          generator.graph.addedge(stbuff, ld, 0, 0, 1);
          generator.graph.addedge(sttrue, ld, 0, 0, 1);
          generator.graph.addedge(stfalse, ld, 0, 0, 1);

          generator.graph.rmvert(n);
        };

        void transform_buffer(G& generator, node* n, const string& name)
        {
          const auto& expansion = generator.instab.find_expansion(get_name(), name, n->data.get_typespec());
          const vector<opid_t>& ids = expansion.arguments;

          if(ids.size() < 2)
            error("not enough arguments for merge expansion in cf transform");

          int buff_id = new_buff_id();

          opid_t buff_st = ids[0];
          opid_t buff_ld = ids[1];

          node* st = generator.addvert(n->id + "_buff_st_" + ctb::to_string(buff_id), buff_st, buff_id);
          node* ld = generator.addvert(n->id + "_buff_ld_" + ctb::to_string(buff_id), buff_ld, buff_id);

          generator.graph.addedge(n->in_at(0)->from, st,  0, n->in_at(0)->frompos, 0);
          generator.graph.connect_as(ld, n, false, true);

          generator.graph.addedge(st, ld, 0, 0, 1);

          generator.graph.rmvert(n);
        };

        void transform_buffer_edge(G& generator, edge* e)
        {
          const auto& expansion = generator.instab.find_expansion(get_name(), "buffer", {e->from->data.op->out_type});
          const vector<opid_t>& ids = expansion.arguments;

          if(ids.size() < 2)
            error("not enough arguments for merge expansion in cf transform");

          int buff_id = new_buff_id();

          opid_t buff_st = ids[0];
          opid_t buff_ld = ids[1];

          node* st = generator.addvert("generic_cf_node_buff_st_" + ctb::to_string(buff_id), buff_st, buff_id);
          node* ld = generator.addvert("generic_cf_node_buff_ld_" + ctb::to_string(buff_id), buff_ld, buff_id);

          generator.graph.addedge(e->from, st,  0, e->frompos, 0);
          generator.graph.addedge(ld, e->to,  e->topos, 0, 0);

          generator.graph.addedge(st, ld, 0, 0, 1);

          generator.graph.rmedge(e);
        };


        void remove_cycles(G& generator)
        {
          AUTO(graph)& g = generator.graph;

          g.factorize();
          AUTO(node)* n = g.factor.find_cycle_begin({0},{0});
          int remains = g.verts.size() + 2;
          while(n != NULL && remains-- > 0)
          {
            set<node*> outs;
            //will need another crawl here
            for(node* v : n->data.out)
            {
              for(edge* e : v->out.getlevel(1))
              {
                outs.insert(e->to);
              }
            }
            queue<node*> q;
            for(auto v : outs)
              q.push(v);

            if(q.size() == 0 )
              error("cycle removal problem - found unsolveable situation");

            //then we colour all these and all their children by red colour
            int red = node::newid();
            int blue = node::newid();
            q.front()->template crawl<false,false>(
                [&](node* n)->bool{ n->colourmark = red; return true;},
                [&](node* n)->bool{ return n->colourmark != red;},
                {0,1},
                &q
                );

            //for debug
            //auto colourer = [=](node* n)->string{if(n->colourmark == red) return "red"; else if(n->colourmark == blue) return "blue"; else return "black";}; 

            //in this pass we get a list of all edges that go from nonred to red
            //we crawl the current partition, marking crawled vertices as blue
            vector<edge*> edges;
            auto f = [&](node* n)->bool
            { 
              if(n->colourmark != red)
              {
                for(auto e : n->out)
                  if(e->to->colourmark == red)
                  {
                    edges.push_back(e);
                  }
              }
              n->colourmark = blue;
              return true;
            };

            (*n->data.vertices.begin())->template crawl<true, false>(
                f,
                [&](node* n)->bool{ return n->colourmark != blue;},
                {0}
                );
            //now we have a list of edges defining the cut between red and nonred vertices inside of our partition. (actually the entire partition is blue now)

            //we use that list to make the cut
            for(auto e : edges)
              transform_buffer_edge(generator, e);

            g.factorize();
            n = g.factor.find_cycle_begin({0},{0});
          }

          if(remains < 0)
            error("Failed on graph partitioning - there seems to be a cycle somewhere!");
        };

      public:

        /** 
         * We construct list of nodes that are to be transformed using this tranformer. This consists of pairs of a node pointer and an expansion structure reference)
         * Then we go through these and perform them.
         * */
        void transform(G& generator)
        {
          typename G::graph_t& g = generator.graph;

          typedef pair<node*,decltype(declval<node>().data.op->expansions.front())> p;
          vector<p> list;
          g.crawl_topological([&](node* n){ for(const auto& e : n->data.op->expansions){ if(e.transformer_name == get_name()) list.push_back(p(n,e)); break;}});
          for(const auto& r : list)
          {
            try
            {
              if(r.second.name == "split")
                transform_split(generator, r.first, r.second.name);
              else if(r.second.name == "merge")
                transform_merge(generator, r.first, r.second.name);
              else if(r.second.name == "buffer")
                transform_buffer(generator, r.first, r.second.name);
              else
                error(string("unknown conversion type: '") + r.second.name + "' at transformer: " + get_name());
            }
            catch (error_struct& err)
            {
              cerr << "failed while processing cf transformation at vertex " << r.first->id << ":" << endl;
              error(err.first, err.second);
            }
          }

          remove_cycles(generator);
        }
    };
};

#endif
