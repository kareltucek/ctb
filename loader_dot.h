#ifndef DOTLOADER_GUARD
#define DOTLOADER_GUARD


#include "defines.h"
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
   template <class T, class G, class IT >
     class dot_loader
     {
       private:
       public:
         void load_graph(G& graph, istream&) ;
         void load_instab(IT& instab, istream&) ;
         void export_graph(G& instab, ostream&) ;
         void export_instab(IT& instab, ostream&) ;
         static string get_name();
         static void self_test() ;
     } ;

   typedef dot_loader<traits, generator_default, instruction_table_default> dotloader_default;


   template <class T, class G, class IT>
     string dot_loader<T,G,IT>::get_name()
     {
       return "dot";
     }

   template <class T, class G, class IT>
     void dot_loader<T,G,IT>::export_graph(G& generator, ostream& o)
     {
       generator.graph.dump(o);
     }

   template <class T, class G, class IT>
     void dot_loader<T,G,IT>::load_graph(G& graph, istream&)
     {
       error( "dot loader does not support graph import"); 
     }

   template <class T, class G, class IT>
     void dot_loader<T,G,IT>::export_instab(IT& instab, ostream& s)
     {
       error( "dot loader does not support instruction table export"); 
     }

   template <class T, class G, class IT>
     void dot_loader<T,G,IT>::load_instab(IT& instab, istream& s)
     {
       error( "dot loader does not support instruction table import"); 
     }

   template class dot_loader<traits, generator_default, instruction_table_default> ;
}

#endif 
