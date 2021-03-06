#ifndef CSVLOADER_GUARD
#define CSVLOADER_GUARD


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
   *            10 code declaration
   *            11 tags
   *            12 rating
   * for expansion:
   *          (operation definition)
   *            2 output types
   *            3 input types (in-order comma separated list)
   *            4 opid
   *          (expansion definition)
   *            6 expansion name
   *            7 transformer's name
   *            8 args = comma separated list for the transformer
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
         enum cols_instruction {ciNote,ciType,ciOutType,ciInTypes,ciOpId,ciFlags,ciWIn,ciWOut,ciCode,ciTag,ciRating,ciSize};
         enum cols_expansion   {ceNote,ceType,ceOutType,ceInTypes,ceOpId,ceName,ceTransformer,ceArgs,ceSize};
         enum cols_version     {cvNote,cvType,cvTId,cvBW,cvW,cvCode,cvSize};
       enum cols_conversion  {ccNote,ccType,ccTId,ccBW,ccWIn,ccWOut,ccCode1,ccCode2,ccCodeCustom,ccCodeGeneric,ccTag,ccRating,ccSize};

       static writer_plain preprocessline(string line);
       static void process(IT& instab, istream& s);
       static void insert(IT& instab, string line);
       static void checksize(int size, const stringlist& data);

       static map<string, int> flags;
       static bool empty(const string& line);
     public:
       void load_graph(G& graph, istream&) ;
       void load_instab(IT& instab, istream&) ;
       void export_graph(G& instab, ostream&) ;
       void export_instab(IT& instab, ostream&) ;
       static string get_name();
       static void self_test() ;
   } ;

 typedef csv_loader<traits, generator_default, instruction_table_default> csvloader_default;


 template <class T, class G, class IT, class D>
   string csv_loader<T,G,IT,D>::get_name()
   {
     return "csv";
   }

 template <class T, class G, class IT, class D>
   void csv_loader<T,G,IT,D>::export_graph(G& generator, ostream& o)
   {
     error( "csv loader does not support graph export"); 
   }

 template <class T, class G, class IT, class D>
   void csv_loader<T,G,IT,D>::load_graph(G& graph, istream&)
   {
     error( "csv loader does not support graph load"); 
   }

 template <class T, class G, class IT, class D>
   void csv_loader<T,G,IT,D>::export_instab(IT& instab, ostream& s)
   {
     s << "#note\ttype\toutput type\tinput types\top id\tflags\twidth in\twidth out\tcode\ttags\trating\tcode_custom[0]\tcode_custom[1]..." << endl;
     for(auto o : instab.instab)
     {
       if(o.second->opid.empty() || o.second->opid[0] == '_')
         continue;
       for(auto i : o.second->versions)
       {
         writer_plain::basic_ignorant_exporter w;
         w.push(i.note);
         w.push("instruction");
         w.push(o.second->out_type);
         w.push(writer_plain(o.second->in_types).list_concat(",").write_str());
         w.push(o.second->opid);
         w.push(flags_to_string(o.second->flags));
         w.push(l_to_string(i.widths_in));
         w.push(to_string(i.width_out));
         w.push(i.code);
         w.push(i.tags);
         w.push(i.rating);
         for(const auto& cc : i.code_custom)
           if(!cc.code.empty())
             w.push(cc.name + ":" + cc.code);
         s << w.list_concat("\t").write_str() << endl;
       }
     }
     //e.g. split  expansion  int,int  int,bool  SPLIT_INT  none  split  cf  SPLIT_ST_INT,SPLIT_LD_INT,SPLIT_LD_INT"
     //e.g. split  expansion  int,int,bool  int  MERGE_INT  none  merge  cf  MERGE_ST_INT,MERGE_LD_INT,MERGE_LD_INT,BUFF_ST_INT"
     s << "#note\ttype\toutput type\tinput types\top id\tflags\tname\ttransformer\targs" << endl;
     for(auto o : instab.instab)
     {
       for(auto i : o.second->expansions)
       {
         writer_plain::basic_ignorant_exporter w;
         w.push(i.note);
         w.push("expansion");
         w.push(o.second->out_type);
         w.push(writer_plain(i.in_types).list_concat(",").write_str());
         w.push(o.second->opid);
         w.push(flags_to_string(o.second->flags));
         w.push(i.name);
         w.push(i.transformer_name);
         w.push(writer_plain(i.arguments).list_concat(",").write_str());
         s << w.list_concat("\t").write_str() << endl;
       }
     }
     s << "#note\ttype\ttype id\tbitwidth\twidth\tcode" << endl;
     for(auto t : instab.typetab)
     {
       if(t.second->tid.empty() || t.second->tid[0] == '_')
         continue;
       for(auto v : t.second->versions)
       {
         writer_plain::basic_ignorant_exporter w;
         w.push(v.note);
         w.push("type_version");
         w.push(t.second->tid);
         w.push(t.second->bitwidth);
         w.push(to_string(v.width));
         w.push(v.code);
         s << w.list_concat("\t").write_str() << endl;
       }
     }
     s << "#note\ttype\ttype id\tbitwidth\twidth in\twidth out\tcode1\tcode2\tcode custom\tcode generic\ttags\trating" << endl;
     for(auto t : instab.typetab)
     {
       for(auto v : t.second->conversions)
       {
         writer_plain::basic_ignorant_exporter w;
         w.push(v.note);
         w.push("type_conversion");
         w.push(t.second->tid);
         w.push(t.second->bitwidth);
         w.push(to_string(v.width_in));
         w.push(to_string(v.width_out));
         w.push(v.code1);
         w.push(v.code2);
         w.push(v.code_custom);
         w.push(v.code_generic);
         w.push(v.tags);
         w.push(v.rating);
         s << w.list_concat("\t").write_str() << endl;
       }
     }
   }

 template <class T, class G, class IT, class D>
   void csv_loader<T,G,IT,D>::load_instab(IT& instab, istream& s)
   {
     process(instab, s);
   }

 template <class T, class G, class IT, class D>
   void csv_loader<T,G,IT,D>::self_test()
   {
     cout << "testing csv loader" << endl;

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
     ifstream s("test1_simple/instab.csv");
     l.load_instab(tab, s);
   }

 template <class T, class G, class IT, class D>
   bool csv_loader<T,G,IT,D>::empty(const string& line)
   {
     for(int i = 0; i < line.size(); ++i)
     {
       if(line[i] != '\t' && line[i] != '\n' && line[i] != '\r')
         return false;
     }
     return true;
   }


 template <class T, class G, class IT, class D>
   void csv_loader<T,G,IT,D>::checksize(int size, const stringlist& data)
   {
     if(data.size() < size)
       error("line too short");
   }

 template <class T, class G, class IT, class D>
   void csv_loader<T,G,IT,D>::insert(IT& instab, string line)
   {
     if(line.size() == 0 || line[0] == '#' || empty(line))
       return;

     stringlist data = split(line, D::value);
     if(data.size() < ciType + 1)
     {
       error("line too short (expected object type)");
     }
     if(data[ciType] == "instruction")
     {
       checksize(ciSize, data);
       int f = string_to_flags<typename T::flag_t>(data[ciFlags]);
       stringlist ccode = stringlist();
       for(int i = ciSize; i < data.size(); i++)
         if(!data[i].empty())
           ccode.push_back(data[i]);
       auto in_widths = ::ctb::lstoi(splitlist(data[ciWIn]));
       instab.add_type(data[ciOutType]);
       typename IT::operation_t& operation = instab.add_operation(data[ciOpId],data[ciOutType],splitlist(data[ciInTypes]),f);
       operation.add_code(in_widths,::ctb::stoi_def(data[ciWOut], 0),data[ciCode],ccode,data[ciNote],data[ciTag],::ctb::stoi(data[ciRating]));
     }
     else if(data[ciType] == "expansion")
     {
       checksize(ceSize, data);
       int f = fEXPANSION;
       instab.add_type(data[ceOutType]);
       auto in_types = splitlist(data[ceInTypes]);
       auto out_types = splitlist(data[ceOutType]);
       //cout << "adding expansion with types: " << data[ceInTypes] << " " << data[ceOutType] << endl;
       typename IT::operation_t& operation = instab.add_operation(data[ceOpId],data[ceOutType],in_types,f);
       operation.add_expansion(data[ceName],data[ceTransformer],splitlist(data[ceArgs]),data[ceNote], in_types, out_types);
     }
     else if (data[cvType] == "type_version")
     {
       checksize(cvSize, data);
       typename IT::type_t& type = instab.add_type(data[cvTId],::ctb::stoi(data[cvBW]));
       type.add_code_type(::ctb::stoi(data[cvW]), data[cvCode], data[cvNote]);
     }
     else if(data[ccType] == "type_conversion")
     {
       checksize(ccSize, data);
       typename IT::type_t& type = instab.add_type(data[ccTId],::ctb::stoi(data[ccBW]));
       type.add_code_conversion(::ctb::stoi(data[ccWIn]), ::ctb::stoi(data[ccWOut]),data[ccCode1],data[ccCode2],data[ccCodeCustom],data[ccCodeGeneric],data[ccNote],data[ccTag],::ctb::stoi(data[ccRating]));
       }
       else
       {
         error( string("unknown line type "), false);
       }
     }


   template <class T, class G, class IT, class D>
     void csv_loader<T,G,IT,D>::process(IT& instab, istream& s)
     {
       string line;
       int i = 1;
       while(getline(s, line))
       {
         while(line.back() == '\\')
         {
           string line2;
           if(!getline(s, line2))
             error("expected line continuation, but eof came");
           line = line.substr(0, line.size()-1) + line2;
           ++i;
         }
         writer_plain lines = preprocessline(line);
         for(int j = 0; j < lines.size(); j++)
         {
           string l = lines.write_line(j);
           try
           {
             insert(instab, l);
           }
           RETHROW(string("at line ") + to_string(i) + ": " + line + "\n expanded to: " + l);
         }
         ++i;
       }
     }

   template <class T, class G, class IT, class D>
     writer_plain csv_loader<T,G,IT,D>::preprocessline(string line)
     {
       return writer_plain().print(line.append("\n"));
     }

   template class csv_loader<traits, generator_default, instruction_table_default> ;
}

#endif 
