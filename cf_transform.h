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
      private:
        typedef typename G::graph_t::node_t node;
        typedef typename G::vid_t vid_t;
        typedef typename G::opid_t opid_t;

        int new_buff_id() {
          static int id = 0;
          return ++id;
        };
        

        void transform_split(G& generator, node* n, const vector<opid_t>& ids)
        {
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

        void transform_merge(G& generator, node* n, const vector<opid_t>& ids)
        {
          if(ids.size() < 3)
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
        
      public:

        static string get_name(){ return "cf";};

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
                transform_split(generator, r.first, r.second.arguments);
              else if(r.second.name == "merge")
                transform_merge(generator, r.first, r.second.arguments);
              else
                error(string("unknown conversion type: '") + r.second.name + "' at transformer: " + get_name());
            }
            catch (error_struct& err)
            {
              cerr << "failed while processing cf transformation at vertex " << r.first->id << ":" << endl;
              error(err.first, err.second);
            }
          }
        }
    };
};

#endif
