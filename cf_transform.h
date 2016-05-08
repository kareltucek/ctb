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
        static string hash;
        static string get_name(){ return "cf";};
      private:
        typedef typename G::graph_t::node_t node;
        typedef typename G::graph_t::fnode_t fnode;
        typedef typename G::graph_t::edge_t edge;
        typedef typename G::vid_t vid_t;
        typedef typename G::opid_t opid_t;

        int new_buff_id() {
          static int id = 0;
          return ++id;
        };

        void balance_buffers(G& generator, bool visual)
        {
          vector<int> order;
          string s;
          for(int i = 0; i < generator.partition_count(); i++)
            order.push_back(0);

          generator.graph.factor.crawl_topological(
              [&](fnode* p){
                int m = 0;
                for(auto* e : p->in)
                  m = m > order[e->from->id] ? m : order[e->from->id];
                order[p->id] = m+1;
              });


          generator.graph.crawl_topological(
              [&](node* n){
                for(auto* e : n->out.get_layer(1))
                {
                  string buffcoef = ctb::to_string(order[e->to->classid] - order[e->from->classid]);
                  e->data.dbglab += string(":buffcoef=") + buffcoef;
                  e->from->data.set_param(s + "buffercoefout" + ctb::to_string(e->from_pos), buffcoef);
                  e->to->data.set_param(s + "buffercoefin" + ctb::to_string(e->to_pos), buffcoef);
                }
              });
        }

        void prepare_inputs(G& generator)
        {
          auto noop = generator.add_vert(hash + "cf_transform_noop_vertex", "_noop");
          int a = 0;
          for(auto* v : generator.graph.in)
            generator.add_edge(v, noop, a++, 0);
        }


        void prepare_outputs(G& generator)
        {
          set<edge*> es;
          for(auto* v : generator.graph.out)
            if(generator.partition_is_topo_max(v->classid))
              for(auto* e : v->in)
                es.insert(e);
          for(auto* e : es)
            transform_buffer_edge(generator, e);
        }

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

          auto st = generator.add_vert(hash + n->id + "_split_st_" + ctb::to_string(buff_id), split_st, toparam("ioindex", buff_id));
          auto ldtrue = generator.add_vert(hash + n->id + "_split_ldtrue_" + ctb::to_string(buff_id), split_ld_true, toparam("ioindex", buff_id));
          auto ldfalse = generator.add_vert(hash + n->id + "_split_ldfalse_" + ctb::to_string(buff_id), split_ld_false, toparam("ioindex", buff_id));

          generator.graph.add_edge(n->in_at(0)->from, st, 0, n->in_at(0)->from_pos, 0);
          generator.graph.add_edge(n->in_at(1)->from, st, 1, n->in_at(1)->from_pos, 0);
          for( auto outvert : n->out)
          {
            if(outvert->from_pos == 0)
              generator.graph.add_edge(ldtrue, outvert->to, outvert->to_pos, 0, 0);
            if(outvert->from_pos == 1)
              generator.graph.add_edge(ldfalse, outvert->to, outvert->to_pos, 0, 0);
          }
          generator.add_edge(st, ldtrue, 0, 0, 1, string("BUFFL_")+ctb::to_string(buff_id));
          generator.add_edge(st, ldfalse, 0, 1, 1, string("BUFFR_")+ctb::to_string(buff_id));

          generator.graph.rm_vert(n);
        };

        void transform_merge(G& generator, node* n, const string& name)
        {
          const auto& expansion = generator.instab.find_expansion(get_name(), name, n->data.get_typespec());
          const vector<opid_t>& ids = expansion.arguments;

          if(ids.size() < 5)
            error("not enough arguments for merge expansion in cf transform");

          int buff_id = new_buff_id();

          opid_t merge_ld = ids[0];
          opid_t merge_proc = ids[1];
          opid_t merge_st_true = ids[2];
          opid_t merge_st_false = ids[3];
          opid_t buff_st = ids[4];

          node* ld = generator.add_vert(hash + n->id + "_merge_ld_" + ctb::to_string(buff_id), merge_ld, toparam("ioindex", buff_id));
          node* sttrue = generator.add_vert(hash + n->id + "_merge_sttrue_" + ctb::to_string(buff_id), merge_st_true, toparam("ioindex", buff_id));
          node* stfalse = generator.add_vert(hash + n->id + "_merge_stfalse_" + ctb::to_string(buff_id), merge_st_false, toparam("ioindex", buff_id));
          node* stbuff = generator.add_vert(hash + n->id + "_merge_stbuff_" + ctb::to_string(buff_id), buff_st, toparam("ioindex", buff_id));
          node* proc = generator.add_vert(hash + n->id + "_merge_proc_" + ctb::to_string(buff_id), merge_proc, toparam("ioindex", buff_id));

          generator.add_edge(n->in_at(0)->from, stbuff,  0, n->in_at(0)->from_pos, 0);
          generator.add_edge(n->in_at(1)->from, sttrue,  0, n->in_at(1)->from_pos, 0);
          generator.add_edge(n->in_at(2)->from, stfalse, 0, n->in_at(2)->from_pos, 0);
          generator.connect_as(ld, n, false, true);

          generator.add_edge(stbuff, proc, 0, 0, 1, string("BUFF_")+ctb::to_string(buff_id));
          generator.add_edge(sttrue, proc, 1, 0, 1, string("BUFFL_")+ctb::to_string(buff_id));
          generator.add_edge(stfalse, proc, 2, 0, 1, string("BUFFR_")+ctb::to_string(buff_id));
          generator.add_edge(proc, ld, 0, 0, 1, string("BUFFM_")+ctb::to_string(buff_id));

          generator.rm_vert(n);
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

          node* st = generator.add_vert(hash + n->id + "_buff_st_" + ctb::to_string(buff_id), buff_st, toparam("ioindex", buff_id));
          node* ld = generator.add_vert(hash + n->id + "_buff_ld_" + ctb::to_string(buff_id), buff_ld, toparam("ioindex", buff_id));

          generator.add_edge(n->in_at(0)->from, st,  0, n->in_at(0)->from_pos, 0);
          generator.connect_as(ld, n, false, true);

          generator.add_edge(st, ld, 0, 0, 1, string("BUFF_")+ctb::to_string(buff_id));

          generator.rm_vert(n);
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

          node* st = generator.add_vert(hash + "generic_cf_node_buff_st_" + ctb::to_string(buff_id), buff_st, toparam("ioindex", buff_id));
          node* ld = generator.add_vert(hash + "generic_cf_node_buff_ld_" + ctb::to_string(buff_id), buff_ld, toparam("ioindex", buff_id));

          generator.add_edge(e->from, st,  0, e->from_pos, 0);
          generator.add_edge(ld, e->to,  e->to_pos, 0, 0);

          generator.add_edge(st, ld, 0, 0, 1, string("BUFF_")+ctb::to_string(buff_id));

          generator.rm_edge(e);
        };


        void remove_cycles(G& generator, bool showsteps)
        {
          AUTO(graph)& g = generator.graph;

          int red,green,blue, yellow;
          auto colourer = [&](node* n)->string{if(n->colourmark == green) return "green"; else if(n->colourmark == yellow) return "yellow"; else if(n->colourmark == red) return "red"; else if(n->colourmark == blue) return "blue"; else return "black";};  // for debug

          fnode* n = NULL;
          int remains = g.verts.size() + 2;
          g.update_factor();
          while(g.factor.cycle_exists(&n) && remains-- > 0)
          {

            if(n->data.out.size() == 0 )
              error("cycle removal problem - found unsolveable situation - component is marked as cycle begin and yet has no layered output - this should not have happened");

            set<edge*> edges;

            for(node* bv : n->data.out)
            {
              queue<node*> q;
              for(edge* e : bv->out.get_layer(1))
                q.push(e->to);

              //then we colour all these and all their children by red colour
              red = node::newid();
              blue = node::newid();
              green = node::newid();
              yellow = node::newid();
              q.front()->template crawl<false,false>(
                  [&](node* n)->bool{ n->colourmark = red; return true;},
                  [&](node* n)->bool{ return n->colourmark != red;},
                  {0,1},
                  &q
                  );

              //and spread green colour from these, by this we identify only those edges that have to be cut! (note that we still dont have minimality)
              auto q2 = n->data.in; //make copy
              q2.insert(bv);
              //colour all predecessors green
              for(node* cv : q2)
              {
                cv->template crawl<true,false>(
                    [&](node* n)->bool{ n->colourmark = green; return false;},
                    [&](node* n)->bool{ return n->colourmark != green && n->colourmark != red;},
                    {0, 1}
                    );
              }
              //spread in component
              g.crawl_topological(
                  [&](node* u) 
                  {
                    if(u->colourmark != green && u->colourmark != red && u->classid == n->id)
                      u->colourmark = green;
                  }
                  );

              if(bv->colourmark == red)
              {
                g.dump_visual(colourer);
                error("cycle removal problem - real cycle seem to exist in the original graph");
              }

              if(showsteps)
              {
                auto colourer2 = [&](node* n)->string{ if(n == bv) return "violet"; else if(n->colourmark == green) return "green"; else if(n->colourmark == red) return "red"; else if(n->colourmark == blue) return "blue"; else return "black";};  // for debug
                g.dump_visual(colourer2);
              }

              //in this pass we get a list of all edges that go from green to red
              //we crawl the current partition, marking crawled vertices as blue
              auto f = [&](node* n)->bool
              { 
                if(n->colourmark == green)
                {
                  for(auto e : n->out)
                    if(e->to->colourmark == red)
                    {
                      edges.insert(e);
                    }
                }
                n->colourmark = blue;
                return true;
              };

              (*n->data.vertices.begin())->template crawl<true, false>(
                  f,
                  [&](node* n)->bool{ return n->colourmark != blue;},
                  {0,1}
                  );
            }
            //now we have a list of edges defining the cut between red and nonred vertices inside of our partition. (actually the entire partition is blue now)


            //we use that list to make the cut
            for(auto e : edges)
              transform_buffer_edge(generator, e);

            g.update_factor();
          }

          if(remains < 0)
          {
            g.dump_visual(colourer);
            error("Failed on graph partitioning (cf transform) - there seems to be a cycle somewhere!");
          }
        };

      public:

        /** 
         * We construct list of nodes that are to be transformed using this tranformer. This consists of pairs of a node pointer and an expansion structure reference)
         * Then we go through these and perform them.
         * */
        void transform(G& generator, const stringlist& args)
        {
          typename G::graph_t& g = generator.graph;

          bool visual = false;
          bool remove_cycles_flag = true;
          for(const auto& arg : args)
          {
            if(arg == "show")
              visual = true;
            if(arg == "expandonly")
              remove_cycles_flag = false;
          }


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

          if(remove_cycles_flag)
          {
            //we need to identify the topologically minimal component somehow, so we need to get rid of cycles first
            remove_cycles(generator, visual);
            prepare_inputs(generator);
            remove_cycles(generator, visual);
            prepare_outputs(generator);
            balance_buffers(generator, visual);
          }
          else
            g.update_factor();
        }
    };

  template <class G>
    string cf_transform<G>::hash = string("_");
};

#endif
