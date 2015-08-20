#ifndef TESTLOADER_GUARD
#define TESTLOADER_GUARD


#include <regex>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "datatypes.h"
#include "generator.h"
#include "aliasenv_maker.h"


namespace ctb
{

  /** 
   * Test loader serves for testing instruction sets. It tries to construct a graph which will contain as many instructions as possible. 
   *
   * Note that due to the nondeterministic nature of the generation in respect to vector widths there is no guarantee that a generated graph will test all width versions of an instruction.
   *  - For testing all (width-wise) instruction versions, generate and test code for all possible widths. 
   *  - For testing all width-conversions, you will need to generate a graph of maximal width with only minimal load/store widths. You can achieve this by using the tag system. This may need altering the table so it is not done automatically.
   * */

  template <class T, class G, class IT> 
    class test_loader
    {
      public:
        static void load_graph(G& graph, const IT&) ;
        static void load_instab(IT& instab, std::istream&) ;
        static void export_graph(G& instab, std::ostream&) ;
        static void export_instab(IT& instab, std::ostream&) ;
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
    void test_loader<T,G,IT>::load_graph(G& graph, const IT& instab)
    {
      
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
