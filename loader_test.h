#ifndef TESTLOADER_GUARD
#define TESTLOADER_GUARD


#include <regex>
#include <fstream>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "datatypes.h"
#include "generator.h"
#include "aliasenv_maker.h"
#include "cartesian_multiplier.h"


namespace ctb
{

  /** 
   * Test loader serves for testing instruction sets. It tries to construct a graph which will contain as many instructions as possible. 
   *
   * Note that due to the nondeterministic nature of the generation in respect to vector widths there is no guarantee that a generated graph will test all width versions of an instruction.
   *  - For testing all (width-wise) instruction versions, generate and test code for all possible widths. 
   *  - For testing all width-conversions, you will need to generate a graph of maximal width with only minimal load/store widths. You can achieve this by using the tag system. This may need some altering of the table thus it is not done automatically.
   *
   * adddebug
   *
   * TODO: fix the following note.
   * Note that this version ignores outputs which cannott be connected directly to an output node. Correction seems to need construction of a shortest path to an output node and construction of such a path (including its dependencies). Does not seem to be worthwile at the moment, since standard instruction set should be able to output all types.
   * */

  template <class T, class G, class IT> 
    class test_loader
    {
      private:
        std::map<typename T::tid_t, std::vector<typename T::opid_t> > ins;
        std::map<typename T::tid_t, std::vector<typename T::opid_t> > outs;
        static std::string get_op_name(typename T::opid_t, const std::string& base, int v = -1);
        void genvert(const std::string& base, int i, typename IT::operation_t* op, const cartesian_multiplier<std::vector<typename T::opid_t> >& it, G& graph);
        void gendbg(G& gen, const IT& it, typename T::vid_t v);
        int oid;
        int pid;
      public:
          void adddebug(G& graph, const IT&, int frame, const stringlist&) ;
          void load_graph(G& graph, const IT&) ;
          void load_instab(IT& instab, std::istream&) ;
          void export_graph(G& instab, std::ostream&) ;
          void export_instab(IT& instab, std::ostream&) ;
          static std::string get_name();
          static void self_test() ;
    } ;

  typedef test_loader<traits, generator_default, instruction_table_default> testloader_default;


  template <class T, class G, class IT>
    std::string test_loader<T,G,IT>::get_name()
    {
      return "test";
    }

    template <class T, class G, class IT>
  void test_loader<T,G,IT>::gendbg(G& gen, const IT& it, typename T::vid_t v)
  {
    static int id = 1;
    typename T::opid_t v_opid = gen.graph->verts.r().find(v)->second->data->opid.r();
    typename T::opid_t debug_id = it.dec(v_opid).get_debug_opid();
    typename T::vid_t name = get_op_name(debug_id,"DEBUG", id++);
    gen.addvert(name, debug_id, 0);
    gen.addedge(v,name,0);
  }


    //bfs so that we cant get exponential...
    template <class T, class G, class IT>
  void test_loader<T,G,IT>::adddebug(G& gen, const IT& it, int depth, const stringlist& v)
  {
    std::set<typename T::vid_t> l;
    std::queue<const typename G::node_t*>* q = new std::queue<const typename G::node_t*>();
    if(v.empty())
    {
      for(auto n : gen.graph->verts.r())
        if(!n.second->data->op->is(fDEBUG) && !n.second->data->op->is(fOUTPUT))
          q->push(n.second);
    }
    else
    {
      for(auto n : v)
      {
        auto itr = gen.graph->verts->find(n);
        if(itr != gen.graph->verts->end())
          q->push(itr->second);
        else
          warning(std::string("vertex for debug not found" ).append(n));
      }
    }
    for(int i = depth; i > 0; i--)
    {
      std::queue<const typename G::node_t*>* qn = new std::queue<const typename G::node_t*>();
      while(!q->empty())
      {
        l.insert(q->front()->id.r());
        for(auto m : q->front()->in.r())
        {
          qn->push(m);
        }
        q->pop();
      }
      delete q;
      q = qn;
    }
    delete q;
    for(auto n : l)
      gendbg(gen, it, n);
    //      l.insert(n.second->id.r());
  }

  template <class T, class G, class IT>
    std::string test_loader<T,G,IT>::get_op_name(typename T::opid_t t, const std::string& base, int v)
    {
      return std::string(base).append(std::to_string(v)).append("_").append(t);
    }

  template <typename T, class G, class IT>
    void test_loader<T,G,IT>::genvert(const std::string& base, int i, typename IT::operation_t* op, const cartesian_multiplier<std::vector<typename T::opid_t> >& it, G& graph)
    {
      typename T::vid_t vid = get_op_name(op->opid, base, i);

      graph.addvert(vid, op->opid, 0);
      int j = 0;
      for( typename std::vector<typename std::vector<typename T::opid_t>::iterator>::const_iterator in = it->begin(); in != it->end(); ++in)
      {
        graph.addedge(get_op_name(**in,"INPUT", j), vid, j);
        ++j;
      }
      auto itre = outs.find(op->out_type);
      if(itre != outs.end())
      {
        int j = 0;
        for(auto itro : itre->second)
        {
          std::string n = get_op_name(itro,"OUTPUT", 0).append(get_op_name(op->opid, "MIDDLE",j));
          graph.addvert(n,itro,++oid);
          graph.addedge(vid, n, 0);
          ++j;
        }
      }
    }

  template <class T, class G, class IT>
    void test_loader<T,G,IT>::load_graph(G& graph, const IT& instab)
    {
      oid = 0;
      pid = 0;

      for(auto o : instab.instab.r())
      {
        if(o.second->is(fINPUT))
          ins[o.second->out_type.r()].push_back(o.second->opid.r());
        if(o.second->is(fOUTPUT))
          outs[o.second->out_type.r()].push_back(o.second->opid.r());
      }

      //for ( auto type : outs)
      //  for ( auto op : type.second)
      //    graph.addvert(get_op_name(op, "OUTPUT", 0), op, ++oid);

      for(int i = 0; i < T::maxarity; ++i)
        for ( auto type : ins)
          for ( auto op : type.second)
            graph.addvert(get_op_name(op, "INPUT", i), op, pid++); 

      std::list<typename IT::operation_t*> q;

      for(auto o : instab.instab.r())
      {
        if(o.second->is(fDEBUG))
          continue;
        if(o.second->is(fINPUT))
          continue;
        if(o.second->is(fOUTPUT))
          continue;
        q.push_back(o.second);
      }

      bool c = true;
      while( c )
      {
        c = false;
        for(auto o : q)
        {
          bool s = true;
          cartesian_multiplier<std::vector<typename T::opid_t> > itr;
          for( auto it : o->in_types.r())
          {
            auto l = ins.find(it);
            if(l == ins.end())
            {
              s = false;
              break;
            }
            itr.add(l->second);
          }
          if(!s)
            continue;
          if(ins.find(o->out_type.r()) == ins.end())
          {
            //for(int k = 0; k < T::maxarity; ++k)
            //  genvert("INPUT", k, o, itr, graph);
            ins[o->out_type.r()].push_back(o->opid.r());
          }
          int j = 0;
          while(itr != itr.end())
          {
            genvert("BASE", j, o, itr, graph);
            ++j;
            ++itr;
          }
        }
      }

      /*
         for(auto o : instab.instab.r())
         {
         for(auto i : o.second->versions.r())
         {
         writer_plain::basic_ignorant_exporter w;
         w.push(i.note);
         w.push("instruction");
         w.push(o.second->out_type);
         w.push(writer_plain(o.second->in_types.r()).list_concat(",").write_str());
         w.push(o.second->opid);
         w.push(flags_to_string(o.second->flags));
         w.push(std::to_string(i.width_in));
         w.push(std::to_string(i.width_out));
         w.push(i.code);
         w.push(i.code_custom);
         w.push(i.tags);
         w.push(i.rating);
         s << w.list_concat("\t").write_str() << std::endl;
         }
         }
         */
    }

  template <class T, class G, class IT>
    void test_loader<T,G,IT>::export_graph(G& graph, std::ostream&)
    {
      error( "test loader does not support graph export");
    }

  template <class T, class G, class IT>
    void test_loader<T,G,IT>::export_instab(IT& instab, std::ostream& s)
    {
      error( "test loader does not support instruction table export");
    }

  template <class T, class G, class IT>
    void test_loader<T,G,IT>::load_instab(IT& instab, std::istream& s)
    {
      error( "test loader does not support instruction table load");
    }

  template <class T, class G, class IT>
    void test_loader<T,G,IT>::self_test()
    {
    }

  template class test_loader<traits, generator_default, instruction_table_default> ;
}

#endif 
