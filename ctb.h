#ifndef ctb_GUARD
#define ctb_GUARD

#include "defines.h"
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
        shared_ptr<tagmaster<uint32_t>> mytags;

        void help_cmdline_old();
        void help_command_stream();
        template<functor_id I, typename F, bool input> void command_io(stringlist&& args);
        void command_help(stringlist&& args);
        void command_source(stringlist&& args);
        void command_testgraph(stringlist&& args);
        void command_adddebug(stringlist&& args);
        void command_generate(stringlist&& args);
        void command_transform(stringlist&& args);
        void command_visualize_graph(stringlist&& args);
        static string get_inner_name(string fname);
        static string get_prefix(string fname);

        void fill();
        void fill_commands();

        typedef tuple< function<void(istream&)>, function<void(istream&)>, function<void(ostream&)>, function<void(ostream&)>> loader_record;
        typedef function<string(string)> aliasenv_record;
        typedef pair<function<void(stringlist&&)>,string> command_record;
        typedef function<void()> transform_record;
        map<string, loader_record> hash_loader;
        map<string, aliasenv_record> hash_aliasenv;
        map<string, command_record> hash_command;
        map<string, transform_record> hash_transforms;
      public:
        ctb();
        ctb(const ctb&) = delete;
        ctb(ctb&&) = delete;
        template<template <typename ...> class L, typename...P> void        load_instab(P...params) ;
        template<template <typename ...> class L, typename...P> void        load_graph(P...params) ;
        template<template <typename ...> class L, typename...P> void        export_instab(P...params) ;
        template<template <typename ...> class L, typename...P> void        export_graph(P...params) ;
        template<template <typename ...> class L, typename...P> void        transform_graph(P...params) ;
        template<template <typename ...> class L, typename M, typename...P> string process(string name, P...params) ;
        template<typename M>                                                string generate(string name) ;

        template<template <typename ... > class L> void register_loader() ;
        template<template <typename ... > class L> void register_transform() ;
        template<class M> void register_aliasenv() ;
        void register_command(const string& cmd, function<void(stringlist&&)> f, const string& description);

        int parse_command_stream(istream& );
        int parse_command(string);
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
    template<template <typename ...> class L> void ctb<T,IT>::register_transform()
    {
      if(L<generator_t>::get_name() == "")
        error( "unnamed transformation passed - (have you defined a 'string get_name(){return \"whatever nonempty\";}' method?");
      hash_transforms[L<generator_t>::get_name()] = transform_record(bind(&ctb<T,IT>::transform_graph<L>  , this));
    }

  template <class T, class IT>
    template<template <typename...> class L> void ctb<T,IT>::register_loader()
    {
      if(L<T,generator_t,IT>::get_name() == "")
        error( "unnamed loader passed - (have you defined a 'string get_name(){return \"whatever nonempty\";}' method?");
      hash_loader[L<T,generator_t,IT>::get_name()] = loader_record(
          bind(&ctb<T,IT>::load_instab<L,istream&>  , this, placeholders::_1),
          bind(&ctb<T,IT>::load_graph<L,istream&>   , this, placeholders::_1),
          bind(&ctb<T,IT>::export_instab<L,ostream&>, this, placeholders::_1),
          bind(&ctb<T,IT>::export_graph<L,ostream&> , this, placeholders::_1));
    }

  template <class T, class IT>
    template<class M> void ctb<T,IT>::register_aliasenv()
    {
      if(M::get_name() == "")
        error( "unnamed aliasenv passed - (have you defined a 'string get_name(){return \"whatever nonempty\";}' method?");
      hash_aliasenv[M::get_name()] = aliasenv_record(bind(&ctb::generate<M>, this, placeholders::_1));
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
    string ctb<T,IT>::generate(string name)
    {
      mygenerator.reset();
      auto m = M::generate(mygenerator.get_broadest(), mygenerator, name);
      return m.write_str();
    }

  template <class T, class IT>
    template <template <typename ...> class L, typename M, typename...P>
    string ctb<T,IT>::process(string name, P...params)
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
      cout << "testing ctb" << endl;
      ctb b;
      ifstream file, g1, g2;
      openstream(file,"test1_simple/instab.xml");
      openstream(g1,"test1_simple/graph.xml");
      openstream(g2,"test1_simple/graph.xml");
      b.load_instab<xml_loader,istream&>(file);
      writer_default::to_file("test1_simple/test_simple.h", b.process<xml_loader, aliasenv_simple,istream&>( "test_simple", g1) );
      //writer_default::to_file("test1_simple/test_bobox.h", b.process<xml_loader, aliasenv_bobox,istream&>( "test_bobox", g2) );
      assert(b.get_prefix("/test/test/last") == "/test/test/");
    }

  template <class T, class IT>
    string ctb<T,IT>::get_prefix(string f)
    {
      int p = f.find_last_of("\\/");
      if(p > -1)
        f = f.substr(0, p).append("/");
      else
        error("cant figure out the executable path");
      return f;
    }

  template <class T, class IT>
    string ctb<T,IT>::get_inner_name(string f)
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
      cout << "syntax: ctb [options] <instr table> <input file 1> <input file 2> ... " << endl;
      cout << "options:" << endl;
      cout << "    -m  {simple|bobox}    output aliasenv specification" << endl;
      cout << "    -o  <directory>       output directory" << endl;
    }

  template <class T, class IT>
    void ctb<T,IT>::help_command_stream()
    {
      cout << "syntax: ctb [options]" << endl;
      cout << "options:" << endl;
      cout << "  -f <file>                   read input from file instead from cin" << endl;
      cout << "  -r <comma separated tags>   require all of these tags" << endl;
      cout << "  -a <comma separated tags>   require one of these tags ('allow')" << endl;
      cout << "  -e <comma separated tags>   exclude code with any of these tags  " << endl;
      cout << "  -n <comma separated tags>   require all tags to be subset of this set of tags" << endl;
      cout << "  -c compile test - makes generator output only the first instruction of every vector  " << endl;
      cout << "  -h            show some help" << endl;
      cout << "" << endl;
      cout << "Actions are to be specified by standard input one per line. A '#' can be used as a comment at a beginning of a line." << endl;
      cout << "Actions:" << endl;
      for(auto l : hash_command)
      {
        cout << "  " << l.second.second << endl;
      }
      cout << "Loaders:" << endl;
      for(auto l : hash_loader)
      {
        cout << "  " << l.first << endl;
      }
      cout << "Aliasenvs:" << endl;
      for(auto l : hash_aliasenv)
      {
        cout << "  " << l.first << endl;
      }
      cout << "Transformers:" << endl;
      for(auto l : hash_transforms)
      {
        cout << "  " << l.first << endl;
      }
    }

  template <class T, class IT>
    int ctb<T,IT>::cmdline_old(int count, char ** args)
    {
      int file = 0;
      char aliasenv = 's';
      vector<char*> files;
      char* instab_file = NULL;
      string opath = "output/";
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
      ifstream stream(instab_file);
      load_instab<xml_loader, istream&>(stream);
      for(auto f : files)
      {
        string cont;
        ifstream streamf(f);
        istream& stream = streamf;
        switch(aliasenv)
        {
          case 's':
            cont = process<xml_loader,aliasenv_simple,istream&>(get_inner_name(f), stream);
            break;
          case 'b':
            cont = process<xml_loader,aliasenv_bobox,istream&>(get_inner_name(f), stream);
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
      string allowed,excluded,required,nonexcluded;
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
                    error( string("argument expected after -").append(string(1,args[i][j])).append(" switch"));
                  if(args[i][j+1] != '\0')
                    warn( string("skipping some (misplaced) switches due to argument at: ").append(string(1,args[i][j])));
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
                  error( string("unknown switch: ").append(string(1,args[i][j])));
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
      mytags = make_shared<tagmaster<uint32_t>>(required,allowed,excluded,nonexcluded);
      instab.add_tags(mytags);
      int r = 0;
      if(files.empty())
      {
        r |= parse_command_stream(cin);
      }
      else
      {
        for( auto f : files)
        {
          ifstream s(f);
          r |= parse_command_stream(s);
        }
      }
      return 0;
    }

  template <class T, class IT>
    int ctb<T,IT>::parse_command_stream(istream& stream)
    {
      string line;
      int i = 1;
      int r = 0;
      while(getline(stream, line))
      {
        try
        {
          parse_command(line);
        }
        catch (error_struct& err)
        {
          r = 1;
          stringstream s;
          s << endl << "command line: " << line << " \n    " << err.first;

          if(err.second)
            error(s.str(), err.second);
          else
            cerr << s.str() << endl;
        }
        ++i;
      }
      return r;
    }

    template <class T, class IT>
  void ctb<T,IT>::register_command(const string& cmd, function<void(stringlist&&)> f, const string& description)
  {
    hash_command[cmd] = command_record(f, description); 
  }

  template <class T, class IT>
    void ctb<T,IT>::fill_commands()
    {
      register_command("loadinstab",  bind(&ctb<T,IT>::command_io<fidli,ifstream,true>,  this, placeholders::_1),  "loadinstab   <loader> <input file>");
      register_command("loadgraph",  bind(&ctb<T,IT>::command_io<fidlg,ifstream,true>,  this, placeholders::_1),  "loadgraph    <loader> <input file>");
      register_command("exportinstab",  bind(&ctb<T,IT>::command_io<fidei,ofstream,false>, this, placeholders::_1),  "exportinstab <loader> <output file>");
      register_command("exportgraph",  bind(&ctb<T,IT>::command_io<fideg,ofstream,false>, this, placeholders::_1),  "exportgraph  <loader> <output file>");
      register_command("source",  bind(&ctb<T,IT>::command_source                       , this, placeholders::_1),  "source       <input file>");
      register_command("generate",  bind(&ctb<T,IT>::command_generate                     , this, placeholders::_1),  "generate <output aliasenv> <output file>");
      register_command("testgraph",  bind(&ctb<T,IT>::command_testgraph                     , this, placeholders::_1),  "testgraph - special version of loadgraph which generates a graph based on current instruction table");
      register_command("adddebug",  bind(&ctb<T,IT>::command_adddebug                     , this, placeholders::_1),  "adddebug  [<depth=1> [<list of vertex ids>]] - hooks a debug node to every vertex of currently loaded graph, if list is present, applies only to its vertices");
      register_command("transform",  bind(&ctb<T,IT>::command_transform                     , this, placeholders::_1),  "transform  <transformer> - transforms the loaded graph somehow");
      register_command("visualize",  bind(&ctb<T,IT>::command_visualize_graph                  , this, placeholders::_1),  "visualize - uses csv graph export method and executes dot and gpicview to actually show the result to the user");
      register_command("help",  bind(&ctb<T,IT>::command_help                  , this, placeholders::_1),  "help");
      register_command("?",  bind(&ctb<T,IT>::command_help                  , this, placeholders::_1),  "help");
    }
  template <class T, class IT>
    void ctb<T,IT>::command_generate(stringlist&& args)
    {
      if(args.size() != 3)
        error( string("invalid number of arguments "), false);
      if(hash_aliasenv.find(args[1]) == hash_aliasenv.end())
        error( string("aliasenv not found (did you register it in ctb.h?): ", false).append(args[1]));
      writer_plain::to_file(args[2], hash_aliasenv[args[1]](get_inner_name(args[2])));
    }

  template <class T, class IT>
    void ctb<T,IT>::command_testgraph(stringlist&& args)
    {
      if(args.size() != 1)
        error( string("invalid number of arguments "), false);
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
      error( string("invalid number of arguments "), false);
    hash_transforms[args[1]]();
  }

  template <class T, class IT>
    void ctb<T,IT>::command_source(stringlist&& args)
    {
      if(args.size() != 2)
        error( string("invalid number of arguments "), false);
      ifstream filei;
      openstream(filei,args[1]);
      parse_command_stream(filei);
    }

  template <class T, class IT>
    void ctb<T,IT>::command_help(stringlist&& args)
    {
      help_command_stream();
    }

  template <class T, class IT>
    void ctb<T,IT>::command_visualize_graph(stringlist&& args)
    {
      mygenerator.graph.dump_visual();
    }


  template <class T, class IT>
    template<functor_id I, typename F, bool input> void ctb<T,IT>::command_io(stringlist&& args)
    {
      if(args.size() != 3)
        error( string("invalid number of arguments "), false);
      if(hash_loader.find(args[1]) == hash_loader.end())
        error( string("loader not found (did you register it in ctb.h?): ", false).append(args[1]));
      F file;
      openstream(file, args[2], input);
      get<I>(hash_loader[args[1]])(file);
    }

  template <class T, class IT>
    int ctb<T,IT>::parse_command(string line)
    {
      stringlist words = split(line, ' ',true);
      if(words.empty())
        return 0;
      if(!words[0].empty() && words[0][0] == '#')
        return 0;
      if(hash_command.find(words[0]) == hash_command.end())
        error( string("invalid command: ").append(line), false);
      cout << "processing command " << words[0] << "...";
      (hash_command[words[0]].first)(move(words));
      cout << "... done!" << endl;
      return 0;
    }



  template class ctb<traits, instruction_table<traits>> ;
  //template class ctb<traits_conv_test, instruction_table<traits_conv_test>> ;
};


#endif
