#ifndef XMLLOADER_GUARD
#define XMLLOADER_GUARD

#include "tinyxml2.h"

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { error(std::string("XML loader error: ").append(std::to_string(a_eResult))); }
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
   * See the test files under 'unit_test1/'.
   *
   * Xml format notes
   * ----------------
   * \code{txt}
   * instab format:
   *  root
   *    instruction_list
   *      operation
   *        input (flag boolean)
   *        output (flag boolean)
   *        opid (identifier of logical operation (such as 'integer addition' - e.g. ADDI)
   *        out_type (typeid of returned type)
   *        in_types (list of parameter types; note that this serves mainly for generation of test cases - generation itself is weakly typed)
   *        instruction
   *          width_in (width of vector this instruction accepts (e.g. 4 integers))
   *          width_out (-||-)
   *          code (inner code which is to be used as a part of predefined template - e.g. "$arg1 + $arg2")
   *          code_custom (if nonempty, will be used *instead* of 'code' . Custom code does not use a predefined template - e.g. "$name = $arg1 + $arg2;" - can be used if default template does not fit (e.g. function call on output instead of assignment))
   *          note (an arbitrary note of yours - may be useful for debug or for sorting of your csv files)
   *          tags (comma separated list of tags for this specific code - meant for turning off and on usage of specific instruction sets (such as using only sse <= 2)
   *          rating (if more than one instruction fits, the one with higher rating isused)
   *    type_list
   *      type
   *        typeid (-||-)
   *        type_version
   *          width (width of vector this type represents - e.g. for int[4] this is 4)
   *          bitwidth (hardware bitwidth - will allo limitting the vector width based on bitwidth (otherwise a 128bit vector of bools would iniciate creation of 64 double registers in single instruction)
   *          code (-||-)
   *          note (-||-)
   *        type_conversion (conversion can glue or split types of some width to two different versions of the same type with different width)
   *          width_in 
   *          width_out
   *          code1 (for gluing this is the result, for splitting this should expand to code generating the first half)
   *          code2 (this to the second half)
   *          code_custom (again nonmanaged version)
   *          note (-||-)
   *          tags (-||-)
   *          rating (-||-)
   *
   * graph format:
   *  graph_list
   *    graph
   *      vertex
   *        param (reserved for special parameters such as input and output index of LD and ST instructions)
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
        static std::string getstr(tinyxml2::XMLNode * node, std::string name);
        static std::string getanystr(tinyxml2::XMLNode * node, std::string name);
        static int getint(tinyxml2::XMLNode * node, std::string name);
        static int getanyint(tinyxml2::XMLNode * node, std::string name, int def = 0);
        static bool hasval(tinyxml2::XMLNode* node, std::string name);
      public:
        void load_graph(G& graph, std::istream& stream) ;
        void load_instab(IT& instab, std::istream& stream) ;
        void export_graph(G& graph, std::ostream& stream) ;
        void export_instab(IT& instab, std::ostream& stream) ;
        static std::string get_name();
        static void self_test() ;
    } ;

  typedef xml_loader<traits, generator_default, instruction_table_default> loader_default;

  template <class T, class G, class IT>
    std::string xml_loader<T,G,IT>::get_name()
    {
      return "xml";
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::export_instab(IT& instab, std::ostream& stream)
    {
      error( "xml export not (yet) supported");
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::export_graph(G& graph, std::ostream& stream)
    {
      error( "xml export not (yet) supported");
    }

  template <class T, class G, class IT>
    int xml_loader<T,G,IT>::getanyint(tinyxml2::XMLNode * node, std::string name, int def)
    {
      if(hasval(node, name))
        return getint(node, name);
      else
        return def;
    }

  template <class T, class G, class IT>
    std::string xml_loader<T,G,IT>::getanystr(tinyxml2::XMLNode * node, std::string name)
    {
      if(hasval(node, name))
        return getstr(node, name);
      else
        return "";
    }

  template <class T, class G, class IT>
    std::string xml_loader<T,G,IT>::getstr(tinyxml2::XMLNode * node, std::string name)
    {
      if(node->FirstChildElement(name.c_str()) != NULL)
        return node->FirstChildElement(name.c_str())->GetText() == NULL ? "" :  std::string(node->FirstChildElement(name.c_str())->GetText());
      else
      {
        tinyxml2::XMLElement * ptr = node->ToElement();
        return ptr->Attribute(name.c_str());
      }
    }

  template <class T, class G, class IT>
    int xml_loader<T,G,IT>::getint(tinyxml2::XMLNode * node, std::string name)
    {
      std::string str = getstr(node, name.c_str());
      if(str.empty())
        return 0;
      return std::stoi(str);
    }

  template <class T, class G, class IT>
    bool xml_loader<T,G,IT>::hasval(tinyxml2::XMLNode * node, std::string name)
    {
      if(node->FirstChildElement(name.c_str()) != NULL)
        return true;
      else
        return node->ToElement()->Attribute(name.c_str()) != NULL;
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::load_graph(G& graph, std::istream& stream)
    {
      std::string xml{ std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };
      using namespace tinyxml2;
      XMLDocument xmlDoc;
      XMLCheckResult(xmlDoc.Parse(xml.c_str()));
      XMLNode * graphnode = xmlDoc.FirstChildElement("graph_list")->FirstChildElement("graph");
      for(XMLElement * itr = graphnode->FirstChildElement("vertex"); itr != NULL; itr = itr->NextSiblingElement("vertex"))
      {
        if(hasval(itr, "param"))
          graph.addvert( getstr(itr, "vid"), getstr(itr, "opid"), getint(itr, "param"));
        else
          graph.addvert( getstr(itr, "vid"), getstr(itr, "opid"));
      }
      for(XMLElement * itr = graphnode->FirstChildElement("edge"); itr != NULL; itr = itr->NextSiblingElement("edge"))
      {
        graph.addedge( getstr(itr, "from"), getstr(itr, "to"), getint(itr, "to_pos"));
      }
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::load_instab(IT& instab, std::istream& stream)
    {
      std::string xml{ std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };
      using namespace tinyxml2;
      XMLDocument xmlDoc;
      //XMLCheckResult(xmlDoc.LoadFile(filename.c_str()));
      XMLCheckResult(xmlDoc.Parse(xml.c_str()));
      XMLNode * root = xmlDoc.FirstChildElement("root");
      XMLNode * inslist = root->FirstChildElement("instruction_list");
      XMLNode * typelist = root->FirstChildElement("type_list");
      for(XMLElement * itr = typelist->FirstChildElement("type"); itr != NULL; itr = itr->NextSiblingElement("type"))
      {
        typename IT::type_t& t = instab.addtype( getstr(itr, "typeid"), getanyint(itr,"bitwidth",32));
        for(XMLElement * itr2 = itr->FirstChildElement("type_version"); itr2 != NULL; itr2 = itr2->NextSiblingElement("type_version"))
          t.addcode_type(getint(itr2, "width"), getstr(itr2, "code"),getanystr(itr2,"note"));
        for(XMLElement * itr2 = itr->FirstChildElement("type_conversion"); itr2 != NULL; itr2 = itr2->NextSiblingElement("type_conversion"))
          t.addcode_conversion(getint(itr2, "width_in"), getint(itr2, "width_out"), getanystr(itr2, "code1"), getanystr(itr2, "code2"),getanystr(itr2,"code_custom"),getanystr(itr2,"note"),getanystr(itr2,"tags"),getanyint(itr2,"rating"));
      }
      for(XMLElement * itr = inslist->FirstChildElement("operation"); itr != NULL; itr = itr->NextSiblingElement("operation"))
      {
        typename T::flag_t f = ((getint(itr, "input")) * fINPUT) |((getint(itr, "output")) * fOUTPUT);
        typename IT::operation_t& t = instab.addoperation( getstr(itr, "opid"), getstr(itr, "out_type"), split(getanystr(itr,"in_types"),','), f);
        for(XMLElement * itr2 = itr->FirstChildElement("instruction"); itr2 != NULL; itr2 = itr2->NextSiblingElement("instruction"))
          t.addcode(getint(itr2, "width_in"),getint(itr2, "width_out"), getstr(itr2, "code"),getanystr(itr2,"code_custom"),getanystr(itr2,"note"),getanystr(itr2,"tags"),getanyint(itr2,"rating"));
      }
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::self_test()
    {
      instruction_table_default tab;
      generator_default g(tab);
      xml_loader l;
      std::ifstream i_xml("unit_test1/instab.xml");
      l.load_instab(tab, i_xml);
      std::ifstream g_xml("unit_test1/graph.xml");
      l.load_graph(g, g_xml);
    }

  template class xml_loader<traits, generator_default, instruction_table_default> ;
}

#endif 
