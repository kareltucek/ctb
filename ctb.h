#ifndef ctb_GUARD
#define ctb_GUARD

#include "graph.h"
#include "datatypes.h"
#include "generator.h"
#include "loader_xml.h"
#include "loader_csv.h"
#include "loader_test.h"
#include "aliasenv_maker.h"
#include "aliasenv_bobox.h"
#include "aliasenv_simple.h"
#include "aliasenv_simu.h"
#include "cf_transform.h"

namespace ctb
{
  /**
   * General description
   * -------------------
   * This class serves as a library interface.
   *
   * This library can be used in 3 different manners:
   *  - From commandline - in this case, generation is controlled in command - like fashion - refer to the doc obtained by ./ctb -h or in the typename ctb::help_command_stream() method
   *  - From programmin environment using text commands
   *  - By calling template procedures directly
   *
   *  All data structures are stored inside an instance of the ctb class. Loading, exporting and generating can be done by methods:
   *    -load_instab
   *    -load_graph
   *    -export_instab
   *    -export_graph
   *    -generate
   *  Where L stands for a loader class, P stands for Parameters defined on the loader side and M stands for a model(==aliasenv). For all currently implemented versions these are io streams.
   *
   * TODO update this doc
   *
   * Usage
   * -----
   *
   * A custom loader or aliasenv has to be registered using the templated method register_aliasenv or register_loader in order to be usable from string driven environments. Note that this does not mean you have to use the static stream api - you may use the general templates directly from a programming environment.
   *
   * The instruction generation takes place in the generator class and is driven from an aliasenv::generate method.
   *
   * A standard workflow will be:
   * loadinstab (using) xml (loader from file ) <yourpath>
   * loadgraph (using) xml (loader from file ) <yourpath>
   * generate (using) bobox (model(==environment) to file) <yourpath>
   *
   * Generation - related overview
   * -----------------------------
   * - graph - is basically a base of generator
   * - instruction table - defines instructions and serves primarily to the generator
   * - generator - stores the pipeline network and generates an instruction code
   * - writer - serves for storing, autoformatting, printing and preprocessing code. Takes aliasenv (and language) as a plugin. 
   * - aliasenv - defines internal generation environment, but also drives the topmost layers of generation.
   * - language - defines formatting and is passed as a part of aliasenv
   *
   * In hierarchy the highest is aliasenv. What is a bit confusing is a fact that aliasenv both drives everything that happens and at the same time serves as a plugin for the writer (and thus indirectly to generator and itself).
   * thus:
   * \code{txt}
   * aliasenv::generate(gets generator)
   *  -> creates writer and starts composing code
   *  -> uses generator to generate actual instruction code
   *
   * generator (contains pipeline (DAG) description, gets instruction table)
   *  -> when asked, searches its graph and generates instructions into a supplemented writer
   *
   * writer (represents a generated stringlist, gets language and aliasenv)
   *  -> receives print("format", a, b, c, d) calls, expands the format using aliases defined in aliasenv and adds the resulting string to its data.
   * \endcode
   *
   * side notes
   * ----------
   * TODO commandline interface depends on tagmasters features ingenerally!
   * TODO split command line interface from programming interface
   *
   * **DEPRECATED**
   * **the following text discusses mostly the environment called here 'cmdline_old' 
   * The main two methods are load_instab and process. Both these take list of parameters which is forwarded to the loader methods altogether with a reference to the object to be filled. The loader interface is not fixed except for the first argument.
   *
   * See the ctb::self_test() method for example usage.
   *
   * There is also a default method for commandline option parsing. This comes in two versions since the original method turned out to be too ingeneral.
   *
   * Template parameters are a trait class, which specifies mostly internal id types, an instruction table class, a loader class and a commandline interface.
   *
   *
  * */
        enum functor_id{fidli = 0, fidlg = 1, fidei = 2, fideg = 3};

    template <class T, class IT>
    class ctb
    {
      private:
        typedef generator<T,IT> generator_t;
        IT instab;
        generator_t mygenerator;
        std::shared_ptr<tagmaster<uint32_t>> mytags;

        void help_cmdline_old();
        void help_command_stream();
        template<functor_id I, typename F, bool input> void command_io(stringlist&& args);
        void command_help(stringlist&& args);
        void command_source(stringlist&& args);
        void command_testgraph(stringlist&& args);
        void command_adddebug(stringlist&& args);
        void command_generate(stringlist&& args);
        void command_transform(stringlist&& args);
        static std::string get_inner_name(std::string fname);
        static std::string get_prefix(std::string fname);

        void fill();
        void fill_commands();

        typedef std::tuple< std::function<void(std::istream&)>, std::function<void(std::istream&)>, std::function<void(std::ostream&)>, std::function<void(std::ostream&)>> loader_record;
        typedef std::function<std::string(std::string)> aliasenv_record;
        typedef std::pair<std::function<void(stringlist&&)>,std::string> command_record;
        typedef std::function<void()> transform_record;
        std::map<std::string, loader_record> hash_loader;
        std::map<std::string, aliasenv_record> hash_aliasenv;
        std::map<std::string, command_record> hash_command;
        std::map<std::string, transform_record> hash_transforms;
      public:
        ctb();
        ctb(const ctb&) = delete;
        ctb(ctb&&) = delete;
        template<template <typename ...> class L, typename...P> void        load_instab(P...params) ;
        template<template <typename ...> class L, typename...P> void        load_graph(P...params) ;
        template<template <typename ...> class L, typename...P> void        export_instab(P...params) ;
        template<template <typename ...> class L, typename...P> void        export_graph(P...params) ;
        template<template <typename ...> class L, typename...P> void        transform_graph(P...params) ;
        template<template <typename ...> class L, typename M, typename...P> std::string process(std::string name, P...params) ;
        template<typename M>                                                std::string generate(std::string name) ;

        template<template <typename ... > class L> void register_loader() ;
        template<template <typename ... > class L> void register_transform() ;
        template<class M> void register_aliasenv() ;
        void register_command(const std::string& cmd, std::function<void(stringlist&&)> f, const std::string& description);

        int parse_command_stream(std::istream& );
        int parse_command(std::string);
        int command_stream_cmdline(int count, char ** args);
        int cmdline_old(int count, char ** args);

        static void self_test() ;
    } ;



  typedef ctb<traits, instruction_table<traits> > ctb_default;
  //typedef ctb<traits_conv_test, instruction_table<traits_conv_test> > ctb_conv_test;

  template <class T, class IT>
    ctb<T,IT>::ctb() : instab(), mygenerator(instab), hash_aliasenv(), hash_loader()
  {
    fill();
    fill_commands();
  }

  template <class T, class IT>
    void ctb<T,IT>::fill()
    {
      register_aliasenv<aliasenv_simple>();
      register_aliasenv<aliasenv_simu>();
      register_aliasenv<aliasenv_bobox>();
      register_loader<csv_loader>();
      register_loader<xml_loader>();
      register_transform<cf_transform>();
    }

  template <class T, class IT>
    template<template <typename...> class L> void ctb<T,IT>::register_transform()
    {
      if(L<generator_t>::get_name() == "")
        error( "unnamed transformation passed - (have you defined a 'std::string get_name(){return \"whatever nonempty\";}' method?");
      hash_transforms[L<generator_t>::get_name()] = transform_record(std::bind(&ctb<T,IT>::transform_graph<L>  , this));
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
    template<class M> void ctb<T,IT>::register_aliasenv()
    {
      if(M::get_name() == "")
        error( "unnamed aliasenv passed - (have you defined a 'std::string get_name(){return \"whatever nonempty\";}' method?");
      hash_aliasenv[M::get_name()] = aliasenv_record(std::bind(&ctb::generate<M>, this, std::placeholders::_1));
    }

  template <class T, class IT>
    template<template <typename ...> class L, typename...P> void ctb<T,IT>::export_instab(P...params)
    {
      L<T, generator_t, IT> loader;
      loader.export_instab(instab, params...);
    }

  template <class T, class IT>
    template<template <typename ...> class L, typename...P> void ctb<T,IT>::export_graph(P...params)
    {
      L<T, generator_t, IT> loader;
      loader.export_graph(mygenerator, params...);
    }

  template <class T, class IT>
    template<template <typename ...> class L, typename...P> void ctb<T,IT>::transform_graph(P...params)
    {
      mygenerator.template transform<L,P...>(params...);
    }

  template <class T, class IT>
    template<template <typename ...> class L, typename...P> void ctb<T,IT>::load_instab(P...params)
    {
      instab.clear();
      L<T, generator_t, IT> loader;
      loader.load_instab(instab, params...);
      mygenerator.update();
    }

  template <class T, class IT>
    template<template <typename ...> class L, typename...P> void ctb<T,IT>::load_graph(P...params)
    {
      mygenerator.clear();
      if(instab.empty())
        warn("Warning: loading graph while instruction table is empty. Graph construction depends on correct input/output flags!");
      L<T, generator_t, IT> loader;
      loader.load_graph(mygenerator, params...);
    }

  template <class T, class IT>
    template <typename M>
    std::string ctb<T,IT>::generate(std::string name)
    {
      mygenerator.reset();
      auto m = M::generate(mygenerator.get_broadest(), mygenerator, name);
      return m.write_str();
    }

  template <class T, class IT>
    template <template <typename ...> class L, typename M, typename...P>
    std::string ctb<T,IT>::process(std::string name, P...params)
    {
      generator_t g(instab);
      L<T, generator_t, IT> l;
     l.load_graph(g, params...);
      auto m = M::generate(g.get_broadest(), g, name);
      return m.write_str();
    }

  template <class T, class IT>
    void ctb<T,IT>::self_test()
    {
      ctb b;
      std::ifstream file, g1, g2;
      openstream(file,"unit_test1/instab.xml");
      openstream(g1,"unit_test1/graph.xml");
      openstream(g2,"unit_test1/graph.xml");
      b.load_instab<xml_loader,std::istream&>(file);
      writer_default::to_file("unit_test1/test_simple.h", b.process<xml_loader, aliasenv_simple,std::istream&>( "test_simple", g1) );
      //writer_default::to_file("unit_test1/test_bobox.h", b.process<xml_loader, aliasenv_bobox,std::istream&>( "test_bobox", g2) );
      assert(b.get_prefix("/test/test/last") == "/test/test/");
    }

  template <class T, class IT>
    std::string ctb<T,IT>::get_prefix(std::string f)
    {
      int p = f.find_last_of("\\/");
      if(p > -1)
        f = f.substr(0, p).append("/");
      else
        error("cant figure out the executable path");
      return f;
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
      std::cout << "    -m  {simple|bobox}    output aliasenv specification" << std::endl;
      std::cout << "    -o  <directory>       output directory" << std::endl;
    }

  template <class T, class IT>
    void ctb<T,IT>::help_command_stream()
    {
      std::cout << "syntax: ctb [options]" << std::endl;
      std::cout << "options:" << std::endl;
      std::cout << "  -f <file>                   read input from file instead from cin" << std::endl;
      std::cout << "  -r <comma separated tags>   require all of these tags" << std::endl;
      std::cout << "  -a <comma separated tags>   require one of these tags ('allow')" << std::endl;
      std::cout << "  -e <comma separated tags>   exclude code with any of these tags  " << std::endl;
      std::cout << "  -n <comma separated tags>   require all tags to be subset of this set of tags" << std::endl;
      std::cout << "  -c compile test - makes generator output only the first instruction of every vector  " << std::endl;
      std::cout << "  -h            show some help" << std::endl;
      std::cout << "" << std::endl;
      std::cout << "Actions are to be specified by standard input one per line. A '#' can be used as a comment at a beginning of a line." << std::endl;
      std::cout << "Actions:" << std::endl;
      for(auto l : hash_command)
      {
        std::cout << "  " << l.second.second << std::endl;
      }
      std::cout << "Loaders:" << std::endl;
      for(auto l : hash_loader)
      {
        std::cout << "  " << l.first << std::endl;
      }
      std::cout << "Aliasenvs:" << std::endl;
      for(auto l : hash_aliasenv)
      {
        std::cout << "  " << l.first << std::endl;
      }
      std::cout << "Transformers:" << std::endl;
      for(auto l : hash_transforms)
      {
        std::cout << "  " << l.first << std::endl;
      }
    }

  template <class T, class IT>
    int ctb<T,IT>::cmdline_old(int count, char ** args)
    {
      int file = 0;
      char aliasenv = 's';
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
                  aliasenv = 'b';
                else if(strcmp(args[i], "simple") == 0)
                  aliasenv = 's';
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
        switch(aliasenv)
        {
          case 's':
            cont = process<xml_loader,aliasenv_simple,std::istream&>(get_inner_name(f), stream);
            break;
          case 'b':
            cont = process<xml_loader,aliasenv_bobox,std::istream&>(get_inner_name(f), stream);
            break;
        }
        writer_default::to_file(opath + "/" + get_inner_name(f) + ".h", cont);
      }
      return 0;
    }

  template <class T, class IT>
    int ctb<T,IT>::command_stream_cmdline(int count, char ** args)
    {
      exec_path = get_prefix(args[0]);
      std::string allowed,excluded,required,nonexcluded;
      stringlist files;
      for(int i = 1; i < count; ++i)
      {
        switch (args[i][0])
        {
          case '-':
            for(int j = 1; args[i][j] != '\0'; ++j)
            {
              switch (args[i][j])
              {
                case 'f':
                case 'a':
                case 'e':
                case 'r':
                  if(i+1 >= count)
                    error( std::string("argument expected after -").append(std::string(1,args[i][j])).append(" switch"));
                  if(args[i][j+1] != '\0')
                    warn( std::string("skipping some (misplaced) switches due to argument at: ").append(std::string(1,args[i][j])));
                  switch(args[i][j])
                  {
                    case 'a':
                      if(!allowed.empty())
                        allowed.append(",");
                      allowed.append(args[i+1]);
                      break;
                    case 'e':
                      if(!excluded.empty())
                        excluded.append(",");
                      excluded.append(args[i+1]);
                      break;
                    case 'n':
                      if(!nonexcluded.empty())
                        nonexcluded.append(",");
                      nonexcluded.append(args[i+1]);
                      break;
                    case 'r':
                      if(!required.empty())
                        required.append(",");
                      required.append(args[i+1]);
                      break;
                    case 'f':
                      files.push_back( args[i+1] );
                      break;
                  }
                  i+=1;
                  goto start;
                  break;
                  case 'c':
               mygenerator.set_compiletest(true);
                break;
                case 'h':
                  help_command_stream();
                  return 0;
                default:
                  help_command_stream();
                  error( std::string("unknown switch: ").append(std::string(1,args[i][j])));
                  return 1;
              }
            }
            break;
          default:
            help_command_stream();
            return 1;
        }
start:;
      }
      mytags = std::make_shared<tagmaster<uint32_t>>(required,allowed,excluded,nonexcluded);
      instab.add_tags(mytags);
      int r = 0;
      if(files.empty())
      {
        r |= parse_command_stream(std::cin);
      }
      else
      {
        for( auto f : files)
        {
          std::ifstream s(f);
          r |= parse_command_stream(s);
        }
      }
      return 0;
    }

  template <class T, class IT>
    int ctb<T,IT>::parse_command_stream(std::istream& stream)
    {
      std::string line;
      int i = 1;
      int r = 0;
      while(std::getline(stream, line))
      {
        try
        {
          parse_command(line);
        }
        catch (error_struct& err)
        {
          r = 1;
          std::stringstream s;
          s << std::endl << "command line: " << line << " \n    " << err.first;

          if(err.second)
            error(s.str(), err.second);
          else
            std::cerr << s.str() << std::endl;
        }
        ++i;
      }
      return r;
    }

    template <class T, class IT>
  void ctb<T,IT>::register_command(const std::string& cmd, std::function<void(stringlist&&)> f, const std::string& description)
  {
    hash_command[cmd] = command_record(f, description); 
  }

  template <class T, class IT>
    void ctb<T,IT>::fill_commands()
    {
      register_command("loadinstab",  std::bind(&ctb<T,IT>::command_io<fidli,std::ifstream,true>,  this, std::placeholders::_1),  "loadinstab   <loader> <input file>");
      register_command("loadgraph",  std::bind(&ctb<T,IT>::command_io<fidlg,std::ifstream,true>,  this, std::placeholders::_1),  "loadgraph    <loader> <input file>");
      register_command("exportinstab",  std::bind(&ctb<T,IT>::command_io<fidei,std::ofstream,false>, this, std::placeholders::_1),  "exportinstab <loader> <output file>");
      register_command("exportgraph",  std::bind(&ctb<T,IT>::command_io<fideg,std::ofstream,false>, this, std::placeholders::_1),  "exportgraph  <loader> <output file>");
      register_command("source",  std::bind(&ctb<T,IT>::command_source                       , this, std::placeholders::_1),  "source       <input file>");
      register_command("generate",  std::bind(&ctb<T,IT>::command_generate                     , this, std::placeholders::_1),  "generate <output aliasenv> <output file>");
      register_command("testgraph",  std::bind(&ctb<T,IT>::command_testgraph                     , this, std::placeholders::_1),  "testgraph - special version of loadgraph which generates a graph based on current instruction table");
      register_command("adddebug",  std::bind(&ctb<T,IT>::command_adddebug                     , this, std::placeholders::_1),  "adddebug  [<depth=1> [<list of vertex ids>]] - hooks a debug node to every vertex of currently loaded graph, if list is present, applies only to its vertices");
      register_command("transform",  std::bind(&ctb<T,IT>::command_transform                     , this, std::placeholders::_1),  "transform  <transformer> - transforms the loaded graph somehow");
      register_command("help",  std::bind(&ctb<T,IT>::command_help                  , this, std::placeholders::_1),  "help");
      register_command("?",  std::bind(&ctb<T,IT>::command_help                  , this, std::placeholders::_1),  "help");
    }
  template <class T, class IT>
    void ctb<T,IT>::command_generate(stringlist&& args)
    {
      if(args.size() != 3)
        error( std::string("invalid number of arguments "), false);
      if(hash_aliasenv.find(args[1]) == hash_aliasenv.end())
        error( std::string("aliasenv not found (did you register it in ctb.h?): ", false).append(args[1]));
      writer_plain::to_file(args[2], hash_aliasenv[args[1]](get_inner_name(args[2])));
    }

  template <class T, class IT>
    void ctb<T,IT>::command_testgraph(stringlist&& args)
    {
      if(args.size() != 1)
        error( std::string("invalid number of arguments "), false);
      test_loader<T,generator_t,IT> l;
      l.load_graph(mygenerator,instab);
    }

  template <class T, class IT>
    void ctb<T,IT>::command_adddebug(stringlist&& args)
  {
      test_loader<T,generator_t,IT> l;
      args.erase(args.begin());
      int depth = args.empty() ? 1 : ::ctb::stoi(args[0]);
      args.erase(args.begin());
      l.adddebug(mygenerator,instab,depth,args);
  }

  template <class T, class IT>
    void ctb<T,IT>::command_transform(stringlist&& args)
  {
    if(args.size() != 2)
      error( std::string("invalid number of arguments "), false);
    hash_transforms[args[1]]();
  }

  template <class T, class IT>
    void ctb<T,IT>::command_source(stringlist&& args)
    {
      if(args.size() != 2)
        error( std::string("invalid number of arguments "), false);
      std::ifstream filei;
      openstream(filei,args[1]);
      parse_command_stream(filei);
    }

  template <class T, class IT>
    void ctb<T,IT>::command_help(stringlist&& args)
    {
      help_command_stream();
    }


  template <class T, class IT>
    template<functor_id I, typename F, bool input> void ctb<T,IT>::command_io(stringlist&& args)
    {
      if(args.size() != 3)
        error( std::string("invalid number of arguments "), false);
      if(hash_loader.find(args[1]) == hash_loader.end())
        error( std::string("loader not found (did you register it in ctb.h?): ", false).append(args[1]));
      F file;
      openstream(file, args[2], input);
      std::get<I>(hash_loader[args[1]])(file);
    }

  template <class T, class IT>
    int ctb<T,IT>::parse_command(std::string line)
    {
      stringlist words = split(line, ' ',true);
      if(words.empty())
        return 0;
      if(!words[0].empty() && words[0][0] == '#')
        return 0;
      if(hash_command.find(words[0]) == hash_command.end())
        error( std::string("invalid command: ").append(line), false);
      std::cout << "processing command " << words[0] << "...";
      (hash_command[words[0]].first)(std::move(words));
      std::cout << "... done!" << std::endl;
      return 0;
    }



  template class ctb<traits, instruction_table<traits>> ;
  //template class ctb<traits_conv_test, instruction_table<traits_conv_test>> ;
};


#endif
