#ifndef CSVLOADER_GUARD
#define CSVLOADER_GUARD


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
   * General loader api
   * ------------------
   * see documentation of the xml loader
   *
   * General description
   * -------------------
   * CSV loader is designed for simplier creation of instruction tables. Specifically it implements a "cartesian" expansion, which simplifies definition of instructions of a shared form.
   *
   * format description
   * --------------------
   * Lines of the csv are of the following types:
   *
   *   -instruction code
   *   -type specification
   *   -type conversion code
   *
   * I.e. only leaf structures are represented. All other structures are inserted implicitly. This format *does* contain duplicities.
   *
   * the default delimiter is tab
   *
   * \code{txt}
   * field:
   *          0 note = arbitrary note. Since csv is outputted depending on the instruction table structure, you may use this field for sorting the table...
   *          1 type = {instruction,type_version,type_conversion}
   *
   * for the instruction type:
   *          (operation definition)
   *            2 output type
   *            3 input types (in-order comma separated list)
   *            4 opid
   *            5 flags = comma separated list of {input,output}
   *          (instruction definition)
   *            6 width in
   *            7 width out
   *            8 code
   *            9 code custom
   *            10 tags
   *            11 rating
   * for the type version:
   *          (logical type definition)
   *            2 type id
   *            3 bitwidth
   *          (version definition)
   *            4 width
   *            5 code
   * for the type conversion:
   *          (logical type definition)
   *            2 type id
   *          (conversion definition)
   *            3 width in
   *            4 width out
   *            5 code 1
   *            6 code 2
   *            7 code custom
   *            8 code generic
   *            9 tags
   *            10 rating
   * \endcode
   *
   * for more detailed description please see the xml loader documentation
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
        enum cols_instruction {ciNote,ciType,ciOutType,ciInTypes,ciOpId,ciFlags,ciWIn,ciWOut,ciCode,ciCodeCustom,ciTag,ciRating};
        enum cols_version     {cvNote,cvType,cvTId,cvBW,cvW,cvCode};
        enum cols_conversion  {ccNote,ccType,ccTId,ccBW,ccWIn,ccWOut,ccCode1,ccCode2,ccCodeCustom,ccCodeGeneric,ccTag,ccRating};

        static writer_plain preprocessline(std::string line);
        static void process(IT& instab, std::istream& s);
        static void insert(IT& instab, std::string line);

        static std::map<std::string, int> flags;
        static bool empty(const std::string& line);
      public:
        void load_graph(G& graph, std::istream&) ;
        void load_instab(IT& instab, std::istream&) ;
        void export_graph(G& instab, std::ostream&) ;
        void export_instab(IT& instab, std::ostream&) ;
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
      error( "csv loader does not support graph export");
    }

  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::load_graph(G& graph, std::istream&)
    {
      error( "csv loader does not support graph load"); 
    }

  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::export_instab(IT& instab, std::ostream& s)
    {
      s << "#note\ttype\toutput type\tinput types\top id\tflags\twidth in\twidth out\tcode\tcode custom\ttags\trating" << std::endl;
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
      s << "#note\ttype\ttype id\tbitwidth\twidth\tcode" << std::endl;
      for(auto t : instab.typetab.r())
      {
        for(auto v : t.second->versions.r())
        {
          writer_plain::basic_ignorant_exporter w;
          w.push(v.note);
          w.push("type_version");
          w.push(t.second->tid);
          w.push(t.second->bitwidth);
          w.push(std::to_string(v.width));
          w.push(v.code);
          s << w.list_concat("\t").write_str() << std::endl;
        }
      }
      s << "#note\ttype\ttype id\tbitwidth\twidth in\twidth out\tcode1\tcode2\tcode custom\tcode generic\ttags\trating" << std::endl;
      for(auto t : instab.typetab.r())
      {
        for(auto v : t.second->conversions.r())
        {
          writer_plain::basic_ignorant_exporter w;
          w.push(v.note);
          w.push("type_conversion");
          w.push(t.second->tid);
          w.push(t.second->bitwidth);
          w.push(std::to_string(v.width_in));
          w.push(std::to_string(v.width_out));
          w.push(v.code1);
          w.push(v.code2);
          w.push(v.code_custom);
          w.push(v.code_generic);
          w.push(v.tags);
          w.push(v.rating);
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

      stringlist d = split("\t\t", '\t');
      assert(d.size() == 3);

      auto e = preprocessline("${animal->cat,dog,fly}");
      assert(e.strings().size() >= 3);

      instruction_table_default tab;
      csvloader_default l;
      std::ifstream s("unit_test1/instab.csv");
      l.load_instab(tab, s);
    }

  template <class T, class G, class IT, class D>
    bool csv_loader<T,G,IT,D>::empty(const std::string& line)
    {
      for(int i = 0; i < line.size(); ++i)
      {
        if(line[i] != '\t' && line[i] != '\n' && line[i] != '\r')
          return false;
      }
      return true;
    }

  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::insert(IT& instab, std::string line)
    {
      if(line.size() == 0 || line[0] == '#' || empty(line))
        return;

      stringlist data = split(line, D::value);
      if(data[ciType] == "instruction")
      {
        int f = string_to_flags<typename T::flag_t>(data[ciFlags]);
        instab.addtype(data[ciOutType]);
        typename IT::operation_t& operation = instab.addoperation(data[ciOpId],data[ciOutType],split(data[ciInTypes],','),f);
        operation.addcode(ctb::stoi(data[ciWIn]),ctb::stoi(data[ciWOut]),data[ciCode],data[ciCodeCustom],data[ciNote],data[ciTag],ctb::stoi(data[ciRating]));
      }
      else if (data[cvType] == "type_version")
      {
        typename IT::type_t& type = instab.addtype(data[cvTId],ctb::stoi(data[cvBW]));
        type.addcode_type(ctb::stoi(data[cvW]), data[cvCode], data[cvNote]);
      }
      else if(data[ccType] == "type_conversion")
      {
        typename IT::type_t& type = instab.addtype(data[ccTId],ctb::stoi(data[ccBW]));
        type.addcode_conversion(ctb::stoi(data[ccWIn]), ctb::stoi(data[ccWOut]),data[ccCode1],data[ccCode2],data[ccCodeCustom],data[ccCodeGeneric],data[ccNote],data[ccTag],ctb::stoi(data[ccRating]));
      }
      else
      {
        error( std::string("unknown line type "), false);
      }
    }


  template <class T, class G, class IT, class D>
    void csv_loader<T,G,IT,D>::process(IT& instab, std::istream& s)
    {
      std::string line;
      int i = 1;
      while(std::getline(s, line))
      {
        writer_plain lines = preprocessline(line);
        for(int j = 0; j < lines.size(); j++)
        {
          std::string l = lines.write_line(j);
          try
          {
            insert(instab, l);
          }
          catch(const error_struct& e)
          {
            error(std::string("at line ").append(std::to_string(i)).append(": ").append(line).append("\n    expanded to:").append(l).append("\n    ").append(e.first ), false);
          }
          catch(std::exception& e)
          {
            error(std::string("at line ").append(std::to_string(i)).append(": ").append(line).append("\n    expanded to:").append(l).append("\n    ").append(e.what() ), false);
          }
        }
        ++i;
      }
    }

  template <class T, class G, class IT, class D>
    writer_plain csv_loader<T,G,IT,D>::preprocessline(std::string line)
    {
      return writer_plain().print(line.append("\n"));
    }

  template class csv_loader<traits, generator_default, instruction_table_default> ;
}

#endif 
