#ifndef CSVLOADER_GUARD
#define CSVLOADER_GUARD


#include <regex>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "datatypes.h"
#include "generator.h"
#include "model_maker.h"


namespace ctb
{

  /** 
   * CSV loader is designed for simplier creation of instruction tables. Specifically it implements a "cartesian" expansion, which simplifies definition of instructions of a shared form.
   *
   *
   * cartesian expansion
   * -------------------
   *
   * Expansion is in form ${ identifier -> value1, value2 ...}, where the ${ ... } will be stored in memory and replaced by $identifier. Later $identifier will be substitued by values in entire string.
   *
   * in example
   *
   *   ${ color -> red, blue } $animal; nice ${ animal -> dog,cat }; 
   *
   * will produce lines
   *    
   *   red dog; nice dog
   *   red cat; nice cat
   *   blue dog; nice dog
   *   blue cat; nice cat
   *
   * these lines will then be parsed and loaded as a plain csv
   *
   *
   * format description
   * --------------------
   * Lines of the csv are of the following types:
   *
   *   instruction code
   *   type specification
   *   type conversion code
   *
   * I.e. only leaf structures are represented. All other structures are inserted implicitly. This format may contain duplicities.
   *
   * the default delimiter is tab
   *
   *
   * field: 1 type = {instruction,type_version,type_conversion}     
   *
   * for the instruction type:
   *          (operation definition)
   *            2 opid
   *            3 output type
   *            4 flags = comma separated list of {input,output}
   *          (instruction definition)
   *            5 width in
   *            6 width out
   *            7 code
   *            8 code custom
   * for the type version:
   *          (logical type definition)
   *            2 type id
   *          (version definition)
   *            3 width
   *            4 code
   * for the type conversion:
   *          (logical type definition)
   *            2 type id
   *          (conversion definition)
   *            3 width in
   *            4 width out
   *            5 code 1
   *            6 code 2
   *            7 code custom
   *              
   *
   *
   * */

  template <char D>
  struct delimiter
  {
    static const char value = D;
  };

  template <class T, class G, class IT, class D = delimiter<'\t'> >
    class csv_loader
    {
      private:
        struct csv_loader_tag{};
        typedef model_maker<csv_loader_tag, language_empty> model_pre;
        typedef writer<model_pre> preprocessor;
        enum cols { cType = 0, cIn = 1, cOut = 2, cOpId = 3, cOutType = 4,};

        //this may (should?) be later generalized into a separate class
        static std::vector<std::string> preprocessline(std::string line);
        template <typename I> static void shake(I itr, I itre, std::string line, std::vector<std::string>& output);

        static void process(IT& instab, std::istream& s);
        static void insert(IT& instab, std::string line);

        static std::map<std::string, int> flags;
      public:
        static void load_graph(G& graph, std::istream&) ;
        static void load_instab(IT& instab, std::istream&) ;
        static void export_graph(G& instab, std::ostream&) ;
        static void export_instab(IT& instab, std::ostream&) ;
        static std::string get_name();
        static void self_test() ;
    } ;

  typedef csv_loader<traits, generator_default, instruction_table_default> csvloader_default;


  template <class T, class G, class IT, class D>
    std::string csv_loader<T,G,IT,D>::get_name()
    {
      return "csv";
    }

  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::export_graph(G& graph, std::ostream&)
    {
      throw "csv loader does not support graph export"; 
    }

  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::load_graph(G& graph, std::istream&)
    {
      throw "csv loader does not support graph load"; 
    }

  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::export_instab(IT& instab, std::ostream& s)
    {
      for(auto o : instab.instab.r())
      {
        for(auto i : o.second->versions.r())
        {
          writer_plain w;
          w.push("instruction");
          w.push(o.second->opid);
          w.push(o.second->out_type);
          w.push(flags_to_string(o.second->flags));
          w.push(i.width_in);
          w.push(i.width_out);
          w.push(i.code);
          s << w.list_concat("\t").write_str() << std::endl;
        }
      }
      for(auto t : instab.typetab.r())
      {
        for(auto v : t.second->versions.r())
        {
          writer_plain w;
          w.push("type_version");
          w.push(t.second->tid);
          w.push(v.width);
          w.push(v.code);
          s << w.list_concat("\t").write_str() << std::endl;
        }
        for(auto v : t.second->conversions.r())
        {
          writer_plain w;
          w.push("type_version");
          w.push(t.second->tid);
          w.push(v.width_in);
          w.push(v.width_out);
          w.push(v.code1);
          w.push(v.code2);
          s << w.list_concat("\t").write_str() << std::endl;
        }
      }
    }

  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::load_instab(IT& instab, std::istream& s)
    {
      process(instab, s);
    }

  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::self_test()
    {
      stringlist a = split("a;b;;c",';'); 
      int i = 0;
      assert(a[i++]=="a");
      assert(a[i++]=="b");
      assert(a[i++]=="");
      assert(a[i++]=="c");

      stringlist b = csvloader_default::preprocessline("${color-> red,blue} ${animal->cat,dog} $color");
      i=0;
      assert(b[i++] == "red cat red");
      assert(b[i++] == "red dog red");
      assert(b[i++] == "blue cat blue");
      assert(b[i++] == "blue dog blue");

      instruction_table_default tab;
      csvloader_default l;
      std::ifstream s("xml/instab.csv");
      l.load_instab(tab, s);
    }

  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::insert(IT& instab, std::string line)
    {
      if(line.size() > 0 && line[0] == '#')
        return;

      stringlist data = split(line, D::value);
      if(data[0] == "instruction")
      {
        int f = string_to_flags<typename T::flag_t>(data[4]);
        auto operation = instab.addoperation(data[2],data[3],f);
        operation.addcode(ctb::stoi(data[5]),ctb::stoi(data[6]),data[7]);
      }
      else if (data[0] == "type_version")
      {
        auto type = instab.addtype(data[2]);
        type.addcode_type(ctb::stoi(data[3]), data[4]);
      }
      else if(data[0] == "type_conversion")
      {
        auto type = instab.addtype(data[2]);
        type.addcode_conversion(ctb::stoi(data[3]), ctb::stoi(data[4]),data[5],data[6]);
      }
      else
      {
        throw std::string("unknown line type at: ").append(line);
      }
    }


  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::process(IT& instab, std::istream& s)
    {
      std::string line;
      while(std::getline(s, line))
      {
        auto lines = preprocessline(line);
        for( auto l : lines)
          insert(instab, l);
      }
    }

  template <class T, class G, class IT, class D>
    std::vector<std::string> csv_loader<T,G,IT,D>::preprocessline(std::string line)
    {
      typedef std::pair<std::string, std::vector<std::string> > sub_t; //substitution
      typedef std::vector<sub_t> subtab_t;
      subtab_t subtab;
      std::smatch m;
      std::regex e ("\\$\\{ *([^ }]+) *->([^}]+)\\}");
      while (std::regex_search (line,m,e)) 
      {
        subtab.push_back(sub_t(m[1], split(m[2], ',')));
        line = std::regex_replace(line, e, "$$$1", std::regex_constants::format_first_only);
      }
      std::vector<std::string> results;
      shake(subtab.begin(), subtab.end(), line, results);
      return results;
    }

  template <class T, class G, class IT, class D>
    template <typename I>
    void csv_loader<T,G,IT,D>::shake(I itr, I itre, std::string line, std::vector<std::string>& output)
    {
      if(itr == itre)
        output.push_back(preprocessor().print(line).write_str());
      else
      {
        for(auto val : itr->second)
        {
          model_pre::access(itr->first) = val;
          shake(itr + 1, itre, line, output);
        }
      }

    }


  template class csv_loader<traits, generator_default, instruction_table_default> ;
}

#endif 
