#ifndef ctb_GUARD
#define ctb_GUARD

#include "graph.h"
#include "datatypes.h"
#include "generator.h"
#include "loader_xml.h"
#include "loader_csv.h"
#include "model_maker.h"
#include "model_bobox.h"
#include "model_simple.h"

namespace ctb
{
  /**
   * This class serves as a library interface.
   *
   * TODO update this doc
   *
   * A custom loader or model has to be registered using the templated method register_model or register_loader in order to be usable from string driven environments.
   *
   * **DEPRECATED**
   * The main two methods are load_instab and process. Both these take list of parameters which is forwarded to the loader methods altogether with a reference to the object to be filled. The loader interface is not fixed except for the first argument.
   *
   * See the ctb::self_test() method for example usage.
   *
   * There is also a default method for commandline option parsing. This comes in two versions since the original method turned out to be too ingeneral.
   *
   * Template parameters are a trait class, which specifies mostly internal id types, an instruction table class, a loader class and a commandline interface. 
   *
   * */


  template <class T, class IT>
    class ctb 
    {
      private:
        typedef generator<T,IT> generator_t;
        IT instab;
        generator_t mygenerator;
    void help_cmdline_old();
    void help_command_stream();
        std::string get_inner_name(std::string fname);

        void fill();

        typedef std::tuple< std::function<void(std::istream&)>, std::function<void(std::istream&)>, std::function<void(std::ostream&)>, std::function<void(std::ostream&)>> loader_record;
        typedef std::function<std::string(std::string)> model_record;
         std::map<std::string, loader_record> hash_loader;
         std::map<std::string, model_record> hash_model;
      public:
        ctb();
        ctb(const ctb&) = delete;
        ctb(ctb&&) = delete;
        template<template <typename ...> class L, typename...P> void        load_instab(P...params) ;
        template<template <typename ...> class L, typename...P> void        load_graph(P...params) ;
        template<template <typename ...> class L, typename...P> void        export_instab(P...params) ;
        template<template <typename ...> class L, typename...P> void        export_graph(P...params) ;
        template<typename M>                             std::string generate(std::string name) ;

        template<template <typename ...> class L, typename M, typename...P> std::string process(std::string name, P...params) ;

        template<template <typename ... > class L> void register_loader() ;
        template<class M> void register_model() ;

        int parse_command_stream(std::istream& );
        int parse_command(std::string);
        int command_stream_cmdline(int count, char ** args);
        int cmdline_old(int count, char ** args);

        static void self_test() ;
    } ;


  typedef ctb<traits, instruction_table<traits> > ctb_default;

    template <class T, class IT>
   ctb<T,IT>::ctb() : instab(), mygenerator(instab), hash_model(), hash_loader()
  {   
    fill();
  }

      template <class T, class IT>
    void ctb<T,IT>::fill()
    {
      register_model<model_simple>();
      register_model<model_bobox>();
      register_loader<csv_loader>();
      register_loader<xml_loader>();
    }

  template <class T, class IT>
    template<template <typename...> class L> void ctb<T,IT>::register_loader()
    {
      if(L<T,generator_t,IT>::get_name() == "")
        error( "unnamed loader passed - (have you defined a 'std::string get_name(){return \"whatever nonempty\";}' method?");
      hash_loader[L<T,generator_t,IT>::get_name()] = loader_record(
          std::bind(&ctb<T,IT>::load_instab<L,std::istream&>  , this, std::placeholders::_1),
          std::bind(&ctb<T,IT>::load_graph<L,std::istream&>   , this, std::placeholders::_1),
          std::bind(&ctb<T,IT>::export_instab<L,std::ostream&>, this, std::placeholders::_1),
          std::bind(&ctb<T,IT>::export_graph<L,std::ostream&> , this, std::placeholders::_1));
    }

  template <class T, class IT>
    template<class M> void ctb<T,IT>::register_model()
    {
      if(M::get_name() == "")
        error( "unnamed model passed - (have you defined a 'std::string get_name(){return \"whatever nonempty\";}' method?");
      hash_model[M::get_name()] = model_record(std::bind(&ctb::generate<M>, this, std::placeholders::_1)); 
    }

  template <class T, class IT>
    template<template <typename ...> class L, typename...P> void ctb<T,IT>::export_instab(P...params)
    {
      L<T, generator_t, IT>::export_instab(instab, params...);
    }

  template <class T, class IT>
    template<template <typename ...> class L, typename...P> void ctb<T,IT>::export_graph(P...params)
    {
      L<T, generator_t, IT>::export_graph(mygenerator, params...);
    }

  template <class T, class IT>
    template<template <typename ...> class L, typename...P> void ctb<T,IT>::load_instab(P...params)
    {
      instab.clear();
      L<T, generator_t, IT>::load_instab(instab, params...);
    }

  template <class T, class IT>
    template<template <typename ...> class L, typename...P> void ctb<T,IT>::load_graph(P...params)
    {
      mygenerator.clear();
      L<T, generator_t, IT>::load_graph(mygenerator, params...);
    }

  template <class T, class IT>
    template <typename M>
    std::string ctb<T,IT>::generate(std::string name)
    {
      auto m = M::generate(mygenerator.get_broadest(), mygenerator, name);
      return m.write_str();
    }

  template <class T, class IT>
    template <template <typename ...> class L, typename M, typename...P>
    std::string ctb<T,IT>::process(std::string name, P...params)
    {
      generator_t g(instab);
      L<T, generator_t, IT>::load_graph(g, params...);
      auto m = M::generate(g.get_broadest(), g, name);
      return m.write_str();
    }

  template <class T, class IT>
    void ctb<T,IT>::self_test()
    {
      ctb b;
      b.load_instab<xml_loader>(std::ifstream("xml/instab.xml"));
      writer_default::to_file( b.process<xml_loader, model_simple>( "test_simple", std::ifstream("xml/graph.xml")), "output/test_simple.h");
      writer_default::to_file( b.process<xml_loader, model_bobox>( "test_bobox", std::ifstream("xml/graph.xml")), "output/test_bobox.h");
    }

  template <class T, class IT>
    std::string ctb<T,IT>::get_inner_name(std::string f)
    {
      int p = f.find_last_of("\\/");
      if(p > -1)
        f = f.substr(p+1, f.length()-p);

      p = f.find_last_of(".");
      if(p > -1)
        f = f.substr(0, p);
      return f;
    }

  template <class T, class IT>
    void ctb<T,IT>::help_cmdline_old()
    {
      std::cout << "syntax: ctb [options] <instr table> <input file 1> <input file 2> ... " << std::endl;
      std::cout << "options:" << std::endl;
      std::cout << "    -m  {simple|bobox}    output model specification" << std::endl;
      std::cout << "    -o  <directory>       output directory" << std::endl;
    }

  template <class T, class IT>
    void ctb<T,IT>::help_command_stream()
    {
      std::cout << "syntax: ctb [options]" << std::endl;
      std::cout << "options:" << std::endl;
      std::cout << "  -f <file>     read input from file instead from cin" << std::endl;
      std::cout << "  -h            show some help" << std::endl;
      std::cout << "" << std::endl;
      std::cout << "Actions are to be specified by standard input one per line. A '#' can be used as a comment at a beginning of a line." << std::endl;
      std::cout << "Actions:" << std::endl;
      std::cout << "  loadinstab <loader> <file>" << std::endl;
      std::cout << "  loadgraph <loader <file>" << std::endl;
      std::cout << "  generate <output model> <output file>" << std::endl;
      std::cout << "  exportinstab <loader <output file>" << std::endl;
      std::cout << "  exportgraph <loader <output file>" << std::endl;
      std::cout << "Loaders:" << std::endl;
      for(auto l : hash_loader)
      {
        std::cout << "  " << l.first << std::endl;
      }
      std::cout << "Models:" << std::endl;
      for(auto l : hash_model)
      {
        std::cout << "  " << l.first << std::endl;
      }
    }

  template <class T, class IT>
    int ctb<T,IT>::cmdline_old(int count, char ** args)
    {
      int file = 0;
      char model = 's';
      std::vector<char*> files;
      char* instab_file = NULL;
      std::string opath = "output/";
      for(int i = 1; i < count; i++)
      {
        switch(args[i][0])
        {
          case '-':
            switch(args[i][1])
            {
              case 'm':
                i++;
                if(strcmp(args[i], "bobox") == 0)
                  model = 'b';
                else if(strcmp(args[i], "simple") == 0)
                  model = 's';
                else
                {
                  help_cmdline_old();
                  return 1;
                }
                break;
              case 'o':
                i++;
                opath = args[i];
                break;
              default:
                help_cmdline_old();
                return 1;
            }
            break;
          default:
            if(file == 0)
              instab_file = args[i];
            else
              files.push_back(args[i]);
            file++;
        }
      }
      std::ifstream stream(instab_file);
      load_instab<xml_loader, std::istream&>(stream);
      for(auto f : files)
      {
        std::string cont;
        std::ifstream streamf(f);
        std::istream& stream = streamf;
        switch(model)
        {
          case 's':
            cont = process<xml_loader,model_simple,std::istream&>(get_inner_name(f), stream);
            break;
          case 'b':
            cont = process<xml_loader,model_bobox,std::istream&>(get_inner_name(f), stream);
            break;
        }
        writer_default::to_file(opath + "/" + get_inner_name(f) + ".h", cont);
      }
      return 0;
    }

  template <class T, class IT>
    int ctb<T,IT>::command_stream_cmdline(int count, char ** args)
    {
      stringlist files;
      for(int i = 1; i < count; ++i)
      {
        switch (args[i][0])
        {
          case '-':
            for(int j = 0; args[i][j] != '\0'; ++j)
            {
              switch (args[i][j])
              {
                case 'f':
                  if(i+1 >= count)
                    error( "filename expected after -f switch");
                  files.push_back( args[i+1] ); 
                  break;
                case 'h':
                  help_command_stream();
                  return 0;
                default:
                  help_command_stream();
                  return 1;
              }
            }
            break;
          default:
            help_command_stream();
            return 1;
        }
      }
      if(files.empty())
      {
        parse_command_stream(std::cin);
      }
      else
      {
        for( auto f : files)
        {
          std::ifstream s(f);
          parse_command_stream(s);
        }
      }
      return 0;
    }

  template <class T, class IT>
    int ctb<T,IT>::parse_command_stream(std::istream& stream)
    {
      std::string line;
      while(std::getline(stream, line))
      {
        try
        {
        parse_command(line);
        }
        catch (error_struct& err)
        {
          if(err.second)
            throw;
          else
            std::cerr << err.first << std::endl;
        }
      }
      return 0;
    }

  template <class T, class IT>
    int ctb<T,IT>::parse_command(std::string line)
    {
        stringlist words = split(line, ' ',true);
        if(words.empty())
          return 0;
        if(!words[0].empty() && words[0][0] == '#')
          return 0;
        if(words[0] == "help" || words[0] == "?")
        {
          help_command_stream();
          return 0;
        }
        if(words.size() != 3)
          error( std::string("invalid number of arguments at line: ").append(line), false);
        switch(cmd_id_hash[words[1]])   
        {
          case fidli:
          case fidlg:
          case fidei:
          case fideg:
            if(hash_loader.find(words[1]) == hash_loader.end())
              error( std::string("loader not found (did you register it in ctb.h?): ", false).append(words[1]));
            break;
          case fidg:
            if(hash_model.find(words[1]) == hash_model.end())
              error( std::string("model not found (did you register it in ctb.h?): ", false).append(words[1]));
            break;
          default:
            error( std::string("invalid parameter at line: ").append(line));
        }
        switch(cmd_id_hash[words[0]])   
        {
          case fidli:
            {
            std::ifstream file(words[2]);
            std::get<fidli>(hash_loader[words[1]])(file);
            }
            break;
          case fidlg:
            {
              std::ifstream file(words[2]);
              std::get<fidlg>(hash_loader[words[1]])(file);
            }
            break;
          case fidei:
            {
              std::ofstream file(words[2]);
              std::get<fidei>(hash_loader[words[1]])(file);
            }
            break;
          case fideg:
            {
              std::ofstream file(words[2]);
              std::get<fideg>(hash_loader[words[1]])(file);
            }
            break;
          case fidg:
            writer_plain::to_file(hash_model[words[1]](get_inner_name(words[2])),words[2]);
            break;
          default:
            error( std::string("unknown action: "  ).append(line));
            break;
        }
      return 0;
    }



  template class ctb<traits, instruction_table<traits>> ;
};


#endif 
