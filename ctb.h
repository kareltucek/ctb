#ifndef ctb_GUARD
#define ctb_GUARD

#include "graph.h"
#include "generator.h"
#include "xmlloader.h"
#include "model.h"
#include "model_bobox.h"
#include "model_simple.h"

template <class T, class IT, class L>
class ctb 
{
private:
  IT instab;
  std::string outpath;
  void help();
  std::string get_inner_name(std::string fname);
public:
  template<typename...P> void load_instab(P...params) ;
  template<typename M, typename...P> void process(std::string name, P...params) ;

  void set_output_path(std::string);
  int cmdline( int count, char ** args);

  static void self_test() ;
} ;

typedef ctb<traits, instruction_table<traits>, loader_default> ctb_default;

template <class T, class IT, class L>
template<typename...P> void ctb<T,IT,L>::load_instab(P...params)
{
  L::load_instab(instab, params...);
}

template <class T, class IT, class L>
template <typename M, typename...P>
void ctb<T,IT,L>::process(std::string name, P...params)
{
  generator<T,IT> g(instab);
  L::load_graph(g, params...);
  auto m = M::generate(g.get_width(), g, name);
  m.write_file(outpath+"/"+name+".h");
}

template <class T, class IT, class L>
void ctb<T,IT,L>::set_output_path(std::string p)
{
  outpath = p;
}

template <class T, class IT, class L>
void ctb<T,IT,L>::self_test()
{
  ctb b;
  b.load_instab("xml/instab.xml");
  b.set_output_path("output");
  b.process<model_simple>( "test_simple", "xml/graph.xml");
  b.process<model_bobox>( "test_bobox", "xml/graph.xml");
}

template <class T, class IT, class L>
std::string ctb<T,IT,L>::get_inner_name(std::string f)
{
  int p = f.find_last_of("\\/");
  if(p > -1)
    f = f.substr(p+1, f.length()-p);

  p = f.find_last_of(".");
  if(p > -1)
    f = f.substr(0, p);
  return f;
}

template <class T, class IT, class L>
void ctb<T,IT,L>::help()
{
  std::cout << "syntax: ctb [options] <instr table> <input file 1> <input file 2> ... " << std::endl;
  std::cout << "options:" << std::endl;
  std::cout << "    -m  {simple|bobox}    output model specification" << std::endl;
  std::cout << "    -o  <directory>       output directory" << std::endl;

}

template <class T, class IT, class L>
int ctb<T,IT,L>::cmdline(int count, char ** args)
{
  int file = 0;
  char model = 's';
  std::vector<char*> files;
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
          help();
          return 1;
        }
        break;
      case 'o':
        i++;
        set_output_path(args[i]);
        break;
      default:
        help();
        return 1;
      }
      break;
    default:
      if(file == 0)
        load_instab(args[i]);
      else
        files.push_back(args[i]);
      file++;

    }
  }
  for(auto f : files)
  {
    switch(model)
    {
    case 's':
      process<model_simple>(get_inner_name(f), f);
      break;
    case 'b':
      process<model_bobox>(get_inner_name(f), f);
      break;
    }
  }
  return 0;
}

template class ctb<traits, instruction_table<traits>, loader_default> ;


#endif 
