#ifndef CF_GUARD
#define CF_GUARD

namespace ctb
{
  /**
   * G is a graph type. (That member graph of a generator)
   * */
  template <class G>
    class cf_transform {
      private:
        typedef typename G::node_t node;
        typedef typename G::id_t id_t;

        void transform_split(node* n, const std::vector<id_t>&)
        {
          error("not implemented yet");
        };

        void transform_merge(node* n, const std::vector<id_t>&)
        {
          error("not implemented yet");
        };
      public:

        static std::string get_name(){ return "cf";};

        /** 
         * We construct list of nodes that are to be transformed using this tranformer. This consists of pairs of a node pointer and an expansion structure reference)
         * Then we go through these and perform them.
         * */
        void transform(G& g)
        {
          typedef std::pair<node*,decltype(std::declval<node>().data->op->expansions.r().front())> p;
          std::vector<p> list;
          g.crawl_topological([&](node* n){ for(const auto& e : n->data->op->expansions.r()){ if(e.transformer_name == get_name()) list.push_back(p(n,e)); break;}});
          for(const auto& r : list)
          {
            if(r.second.name == "split")
              transform_split(r.first, r.second.arguments);
            if(r.second.name == "merge")
              transform_merge(r.first, r.second.arguments);
            else
              error(std::string("unknown conversion type: ").append(r.second.name).append(" at transformer: ").append(get_name()));
          }
        }
    };
};

#endif
