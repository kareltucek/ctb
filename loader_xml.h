#ifndef XMLLOADER_GUARD
#define XMLLOADER_GUARD

#include "tinyxml2.h"

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { error(std::string("XML loader error: ").append(std::to_string(a_eResult))); }
#endif

namespace ctb
{
  /**
   * Instruction_table format honors the structure of the instruction_table class. See that one for more information.
   *
   * Graph is implemented in the most straightforward way possible, which I believe does not need a further comment.
   *
   * See the test files under 'xml/'.
   * */
  template <class T, class G, class IT>
    class xml_loader
    {
      private:
        static std::string getstr(tinyxml2::XMLNode * node, std::string name);
        static std::string getanystr(tinyxml2::XMLNode * node, std::string name);
        static int getint(tinyxml2::XMLNode * node, std::string name);
        static bool hasval(tinyxml2::XMLNode* node, std::string name);
      public:
        static void load_graph(G& graph, std::istream& stream) ;
        static void load_instab(IT& instab, std::istream& stream) ;
        static void export_graph(G& graph, std::ostream& stream) ;
        static void export_instab(IT& instab, std::ostream& stream) ;
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
        typename IT::type_t& t = instab.addtype( getstr(itr, "typeid"));
        for(XMLElement * itr2 = itr->FirstChildElement("type_version"); itr2 != NULL; itr2 = itr2->NextSiblingElement("type_version"))
          t.addcode_type((getint(itr2, "width")), getstr(itr2, "code"));
        for(XMLElement * itr2 = itr->FirstChildElement("type_conversion"); itr2 != NULL; itr2 = itr2->NextSiblingElement("type_conversion"))
          t.addcode_conversion((getint(itr2, "width_in")), getint(itr2, "width_out"), getanystr(itr2, "code1"), getanystr(itr2, "code2"));
      }
      for(XMLElement * itr = inslist->FirstChildElement("operation"); itr != NULL; itr = itr->NextSiblingElement("operation"))
      {
        typename T::flag_t f = ((getint(itr, "input")) * fINPUT) |((getint(itr, "output")) * fOUTPUT);
        typename IT::operation_t& t = instab.addoperation( getstr(itr, "opid"), getstr(itr, "out_type"), f);
        for(XMLElement * itr2 = itr->FirstChildElement("instruction"); itr2 != NULL; itr2 = itr2->NextSiblingElement("instruction"))
          t.addcode((getint(itr2, "width_in")),(getint(itr2, "width_out")), getstr(itr2, "code"));
      }
    }

  template <class T, class G, class IT>
    void xml_loader<T,G,IT>::self_test()
    {
      instruction_table_default tab;
      generator_default g(tab);
      xml_loader l;
      std::ifstream i_xml("xml/instab.xml");
      l.load_instab(tab, i_xml);
      std::ifstream g_xml("xml/graph.xml");
      l.load_graph(g, g_xml);
    }

  template class xml_loader<traits, generator_default, instruction_table_default> ;
}

#endif 
