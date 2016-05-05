#ifndef XMLLOADER_GUARD
#define XMLLOADER_GUARD

#include "defines.h"
#include "tinyxml2.h"

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { error(string("XML loader error: ").append(to_string(a_eResult))); }
#endif

namespace ctb
{
  /**
   * General loader api
   * ------------------
   * A loader is supposed to provide:
   * - A static get_name() method, which returns a string identifier (which serves for registration in command driven environments).
   * - Four methods for import and export of graph and instruction table. These should be present even when the corresponding operation is unsupported (e.g. throwing an exception). The first argument is supposed to be a reference to the object in question. Other arguments are up to the programmer as long as he plans using the loader directly from a programming environment or as long as he is comfortable with writting his own handling commands. Otherwise api is fixed with second argument being either input or output stream. 
   *
   * General description
   * -------------------
   * Instruction_table xml format honors the structure of the instruction_table class. See that one for more information.
   *
   * Graph is implemented in the most straightforward way possible, which I believe does not need a further comment.
   *
   * See the test files under 'test1_simple/'.
   *
   * Xml format notes
   * ----------------
   * \code{txt}
   * instab format:
   *  root
   *    instruction_list
   *      operation
   *        input (flag boolean) - serves for aliasenv (for generation of code with special semantics for IO ) and for generator, which has different semantics for managed code field.
   *        output (flag boolean) - -||-
   *        debug (flag boolean) - debug operations have special semantics, see instructions.h doc
   *        opid (identifier of logical operation (such as 'integer addition' - e.g. ADDI)
   *        out_type (type_id of returned type)
   *        in_types (list of parameter types; note that this serves mainly for generation of test cases - generation itself is weakly typed)
   *        instruction
   *          width_in (width of vector this instruction accepts (e.g. 4 integers))
   *          width_out (-||-)
   *          code (inner code which is to be used as a part of predefined template - e.g. "$arg1 + $arg2")
   *          /TODO update this/ code_custom (if nonempty, will be used *instead* of 'code' . Custom code does not use a predefined template - e.g. "$name = $arg1 + $arg2;" - can be used if default template does not fit (e.g. function call on output instead of assignment))
   *          /TODO UPDATE THIS/code_declaration can contain code which will be printed into a separate code container, e.g. a code which has to be put into global namespace (w.r.t. generation)
   *          note (an arbitrary note of yours - may be useful for debug or for sorting of your csv files)
   *          tags (comma separated list of tags for this specific code - meant for turning off and on usage of specific instruction sets (such as using only sse <= 2)
   *          rating (if more than one instruction fits, the one with higher rating isused)
   *    type_list
   *      type
   *        type_id (-||-)
   *        type_version
   *          width (width of vector this type represents - e.g. for int[4] this is 4)
   *          bitwidth (hardware bitwidth - will allo limitting the vector width based on bitwidth (otherwise a 128bit vector of bools would iniciate creation of 64 double registers in single instruction)
   *          code (-||-)
   *          note (-||-)
   *        type_conversion (conversion can glue or split types of some width to two different versions of the same type with different width)(see instruction table doc for concrete semantics)
   *          width_in 
   *          width_out
   *          code1 (for gluing this is the result, for splitting this should expand to code generating the first half)
   *          code2 (this to the second half)
   *          code_custom (again nonmanaged version (is not expanded nontrivially))
   *          code_generic (again nonmanaged version)
   *          note (-||-)
   *          tags (-||-)
   *          rating (-||-)
   *
   * graph format:
   *  graph_list
   *    graph
   *      vertex
   *        /TODO update this/ param (reserved for special parameters such as input and output index of LD and ST instructions)
   *        vid (a unique string)
   *        opid (instab operation id)
   *      edge
   *        from (vid)
   *        to  (vid)
   *        to_pos (to which slot the input should go - for distinguishing 'a-b' from 'b-a'
   * \endcode
   * */
  template <class T, class G, class IT>
    class xml_loader
    {
      private:
        static string getstr(tinyxml2::XMLNode * node, string name);
        static string getanystr(tinyxml2::XMLNode * node, string name);
        static int getint(tinyxml2::XMLNode * node, string name);
        static int getanyint(tinyxml2::XMLNode * node, string name, int def = 0);
        static bool hasval(tinyxml2::XMLNode* node, string name);
      public:
        void load_graph(G& graph, istream& stream) ;
        void load_instab(IT& instab, istream& stream) ;
        void export_graph(G& graph, ostream& stream) ;
        void export_instab(IT& instab, ostream& stream) ;
        static string get_name();
        static void self_test() ;
    } ;

  typedef xml_loader<traits, generator_default, instruction_table_default> loader_default;

  template <class T, class G, class IT>
    string xml_loader<T,G,IT>::get_name()
    {
      return "xml";
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::export_instab(IT& instab, ostream& stream)
    {
      error( "xml export not (yet) supported");
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::export_graph(G& graph, ostream& stream)
    {
      error( "xml export not (yet) supported");
    }

  template <class T, class G, class IT>
    int xml_loader<T,G,IT>::getanyint(tinyxml2::XMLNode * node, string name, int def)
    {
      if(hasval(node, name))
        return getint(node, name);
      else
        return def;
    }

  template <class T, class G, class IT>
    string xml_loader<T,G,IT>::getanystr(tinyxml2::XMLNode * node, string name)
    {
      if(hasval(node, name))
        return getstr(node, name);
      else
        return "";
    }

  template <class T, class G, class IT>
    string xml_loader<T,G,IT>::getstr(tinyxml2::XMLNode * node, string name)
    {
      tinyxml2::XMLElement * ptr = node->ToElement();
      if( ptr->Attribute(name.c_str()) != NULL)
        return ptr->Attribute(name.c_str());
      else if(node->FirstChildElement(name.c_str()) != NULL)
        return node->FirstChildElement(name.c_str())->GetText() == NULL ? "" :  string(node->FirstChildElement(name.c_str())->GetText());
      error(string("xml loader could not retrieve any string, when required, at  ")+name);
      return "";
    }

  template <class T, class G, class IT>
    int xml_loader<T,G,IT>::getint(tinyxml2::XMLNode * node, string name)
    {
      string str = getstr(node, name.c_str());
      if(str.empty())
        return 0;
      return ctb::stoi(str);
    }

  template <class T, class G, class IT>
    bool xml_loader<T,G,IT>::hasval(tinyxml2::XMLNode * node, string name)
    {
      if(node->FirstChildElement(name.c_str()) != NULL)
        return true;
      else
        return node->ToElement()->Attribute(name.c_str()) != NULL;
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::load_graph(G& graph, istream& stream)
    {
      string xml{ istreambuf_iterator<char>(stream), istreambuf_iterator<char>() };
      using namespace tinyxml2;
      XMLDocument xmlDoc;
      XMLCheckResult(xmlDoc.Parse(xml.c_str()));
      XMLNode * graphnode = xmlDoc.FirstChildElement("graph_list")->FirstChildElement("graph");
      for(XMLElement * itr = graphnode->FirstChildElement("vertex"); itr != NULL; itr = itr->NextSiblingElement("vertex"))
      {
        graph.add_vert( getstr(itr, "vid"), getstr(itr, "opid"), split_params(getanystr(itr, "params")));
      }
      for(XMLElement * itr = graphnode->FirstChildElement("edge"); itr != NULL; itr = itr->NextSiblingElement("edge"))
      {
        graph.addedge( getstr(itr, "from"), getstr(itr, "to"), getint(itr, "to_pos"), getanyint(itr, "from_pos", 0));
      }
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::load_instab(IT& instab, istream& stream)
    {
      string xml{ istreambuf_iterator<char>(stream), istreambuf_iterator<char>() };
      using namespace tinyxml2;
      XMLDocument xmlDoc;
      //XMLCheckResult(xmlDoc.LoadFile(filename.c_str()));
      XMLCheckResult(xmlDoc.Parse(xml.c_str()));
      XMLNode * root = xmlDoc.FirstChildElement("root");
      XMLNode * inslist = root->FirstChildElement("instruction_list");
      XMLNode * explist = root->FirstChildElement("expansion_list");
      XMLNode * typelist = root->FirstChildElement("type_list");
      //TYPES
      if(typelist != NULL)
      {
        for(XMLElement * itr = typelist->FirstChildElement("type"); itr != NULL; itr = itr->NextSiblingElement("type"))
        {
          typename IT::type_t& t = instab.addtype( getstr(itr, "type_id"), getanyint(itr,"bitwidth",32));
          for(XMLElement * itr2 = itr->FirstChildElement("type_version"); itr2 != NULL; itr2 = itr2->NextSiblingElement("type_version"))
            t.add_code_type(getint(itr2, "width"), getstr(itr2, "code"),getanystr(itr2,"note"));
          for(XMLElement * itr2 = itr->FirstChildElement("type_conversion"); itr2 != NULL; itr2 = itr2->NextSiblingElement("type_conversion"))
            t.add_code_conversion(getint(itr2, "width_in"), getint(itr2, "width_out"), getanystr(itr2, "code1"), getanystr(itr2, "code2"),getanystr(itr2,"code_custom"),getanystr(itr2,"code_generic"),getanystr(itr2,"note"),getanystr(itr2,"tags"),getanyint(itr2,"rating"));
        }
      }
      else
        warn("no type list present!");

      //OPERATIONS
      if(inslist != NULL)
      {
        for(XMLElement * itr = inslist->FirstChildElement("operation"); itr != NULL; itr = itr->NextSiblingElement("operation"))
        {
          typename T::flag_t f =  ((getanyint(itr, "debug")) * fDEBUG) | ((getint(itr, "input")) * fINPUT) |((getint(itr, "output")) * fOUTPUT);
          auto in_types = splitlist(getanystr(itr,"in_types"));
          typename IT::operation_t& t = instab.add_operation( getstr(itr, "opid"), getstr(itr, "out_type"), in_types, f);

          for(XMLElement * itr2 = itr->FirstChildElement("instruction"); itr2 != NULL; itr2 = itr2->NextSiblingElement("instruction"))
          {
            stringlist ccode;
            if(hasval(itr2, "code_custom"))
              ccode.push_back(getstr(itr2, "code_custom"));
            for(XMLElement * itr3 = itr->FirstChildElement("code_custom"); itr3 != NULL; itr3 = itr3->NextSiblingElement("code_custom"))
              ccode.push_back(itr3->GetText());
            t.add_code(getint(itr2, "width_in"),getint(itr2, "width_out"), getstr(itr2, "code"),ccode,getanystr(itr2,"note"),getanystr(itr2,"tags"),getanyint(itr2,"rating"));
          }
        }
      }
      else
        warn("no type list present!");

      //EXPANSIONS
      if(explist != NULL)
      {
        for(XMLElement * itr = explist->FirstChildElement("expansion"); itr != NULL; itr = itr->NextSiblingElement("expansion"))
        {
          typename T::flag_t f =  fEXPANSION;
          auto in_types = splitlist(getanystr(itr,"in_types"));
          auto out_types = splitlist(getanystr(itr,"out_types"));
          typename IT::operation_t& t = instab.add_operation( getstr(itr, "opid"), "", in_types, f);
          t.add_expansion(getstr(itr, "name"),getstr(itr, "transformer_name"), splitlist(getstr(itr, "arguments")), getanystr(itr,"note"), in_types, out_types);
        }
      }
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::self_test()
    {
      cout << "testing xml loader" << endl;
      instruction_table_default tab;
      generator_default g(tab);
      xml_loader l;
      ifstream i_xml("test1_simple/instab.xml");
      l.load_instab(tab, i_xml);
      ifstream g_xml("test1_simple/graph.xml");
      l.load_graph(g, g_xml);
    }
}

#endif 
