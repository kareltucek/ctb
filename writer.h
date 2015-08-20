
#ifndef WRITER_GUARD
#define WRITER_GUARD

#include "datatypes.h"
#include "aliasenv_maker.h"
#include "parser.h"

#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <regex>

namespace ctb
{
  enum dolar_mode { dEat, dLet, dExpand, dIgnore };

  struct static_true{ const static bool value = true;};
  struct static_false{ const static bool value = false;};

  template <class M, dolar_mode I, dolar_mode O, class P> class writer_tag { };

  /**
   * general description
   * ===================
   *
   * Writer ensures an user-friendly code output environment. It is basically a container of code, which takes care of autoformatting of the code generated and provides a shell-like expansion environment ('a text preprocessor'), which has a printf-like interface. Unlike printf the reference format is '$<number>', so the arguments may be reused and specified in a static order. These abbreviations are expanded on both sides i.e. in both format string and its argument i.e. if it is your goal, you may create an endless loop. 
   *
   * The writer class is parametrized by a aliasenv class, which provides a translation of references of the form '$<name>' to some new format string, which is then resolved recursively within the original context.
   *
   * dolar import/export modes
   * -------------------------
   *
   * Dolar_mode specifies how $ should be treated. These are specified separately for input (from print to stored internal representation) and output (from internal representation to output). Typically one will want dLet as input and dEat as output, which will result in $$a -> $$a -> $a. Modes are:
   *   - dEat - this transforms $$ to $ on the fly and expands $
   *   - dLet - will expand $ and ignore $$
   *   - dExpand - will take all $ and transform them into $$
   *   - dIgnore - will ignore all $ and $$
   *
   *   Thus typical combination will be dLet+dEat for import operations and dLet+dExpand or dIgnore+dExpand for export (import and export of text which contains 'deeper' level expansions - e.g. for preprocessing of csv files which already contain dolar records).
   *
   * the P parameter
   * ---------------
   * Determines whether postprocessing (cartesian and arithmetic expansion) is allowed. Without this there would be an infinite preprocessing->postprocessing template recursion since postprocessing uses preprocessing of a recursively constructed writer.
   *
   * The execution scheme is the following
   * -------------------------------------
   * 1. number and alias expansion - first print_internal with 'preprocess = true' prints output to an intermediate member string called 'buffer', then the commit method is called
   * 2. cartesian expansion - (performed from the body of the commit method) takes the buffer string, finds all substitutions, constructs a new generic aliasenv and writer with which it will print a new serie of strings and return them as a stringlist
   * 3. returned stringlist is then printed into data with 'preprocess = false' 
   * 5. arithmetic expression expansion takes place during this printing phase
   * 6. during the last phase of this printing formatting takes place
   *
   * formal description
   * ==================
   *
   * number expansion
   * ----------------
   * $<n> (e.g. "$2") resolves to the nth argument - e.g. print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j"
   *
   * alias expansion
   * ---------------
   * $<[a-zA-Z]+> asks aliasenv to resolve the supplemented name - e.g. print("$animal is an animal") -> "dog is an animal" if the supplemented aliasenv (M) contains alias animal resolved to dog
   *
   * cartesian expansion
   * -------------------
   * Expansion is in form ${ identifier -> value1, value2 ...}, where the ${ ... } will be stored in memory and replaced by $identifier. Later $identifier will be substitued by values in entire string.
   *
   * in example
   * \code{txt}  
   *   ${ color -> red, blue } $animal; nice ${ animal -> dog,cat }; 
   * \endcode
   *
   * will produce lines
   * \code{txt}  
   *   red dog; nice dog
   *   red cat; nice cat
   *   blue dog; nice dog
   *   blue cat; nice cat
   * \endcode
   *
   * Expansion can also take tuples. In that case only first alias is used at the position of braces e.g.
   * \code{txt}
   *  ${ color,animal ->blue,dog,red,cat}ish $animal
   *  \endcode
   *  will produce:
   *  \code{txt}
   *  blueish dog
   *  redish cat
   *  \endcode
   *
   *
   * arithmetic expression expansion
   * -------------------------------
   * $[ <arithmetic expression> ] resolves to a calculated integer value
   * supported operands are: [0-9]+, (, ), -, +, *, / 
   * (operators are always binary)
   *
   * formatting
   * ----------
   * Formatting is driven by a 'language' plugin structure, which is passed in as a part of a model(==aliasenv). Formatting takes place at two different places. First when strings are printed into the internal storage - during this phase, formatting takes care of cutting the input stream into ('formatted') pieces. Second part is output part, when the strings are indented.
   *
   */

  template <class M, dolar_mode I = dLet, dolar_mode O = dEat, class P = static_true>
    class writer 
    {
      private:
        stringlist data; /**holds (formatted) data*/
        std::string buffer; /** serves as an intermediate buffer for multiple processing phases*/
        bool last_terminated;
        static std::vector<std::string> preprocessline(std::string line);
        template <typename J, typename N> static void shake(J itr, J itre, std::string line, std::vector<std::string>& output);
        void trim() ;
        template <bool preprocess> void add(std::string&& str, bool terminal) ;
        template <bool preprocess> void process(int& pos, const std::string& format) ;
        void printnth(int i);
        //printnth is deprecated
        template <typename ... Types> void printnth(int i, const std::string& str, const Types&... params) ;
        template <typename ... Types> void printnth(int i, const int& num, const Types&... params) ;
        template <typename ... Types> void printnth(int i, const writer& wrt, const Types&... params) ;
        template <dolar_mode dolars> void write_indent(std::ostream& ss, const std::string& str, int indent, int nobreak) const ;
        std::string get_name(const std::string& format, int& pos);
        template <class N, dolar_mode A, dolar_mode B, class Q> friend class writer;
        template<dolar_mode dolars, bool preprocess, typename ... Types> writer& print_internal (const std::string& format, const Types&... params) ; 
        template<dolar_mode dolars = I, bool preprocess, typename ... Types> writer& print_type (const std::string& format, const Types&... params) ; 
        template<dolar_mode dolars = I, bool preprocess, typename ... Types> writer& print_type (const writer& format, const Types&... params) ;
        template<dolar_mode dolars = I, bool preprocess, typename ... Types> writer& print_type (int, const Types&... params) ;
        template<dolar_mode dolars = I, typename ... Types> writer& print_branch (static_true s,  const Types&... params) ; 
        template<dolar_mode dolars = I, typename ... Types> writer& print_branch (static_false s, const Types&... params) ; 
        writer<M,I,O,P> getnth(int i);
        template <typename ... Types> const writer<M,I,O,P>& getnth(int i, const writer<M,I,O,P>& a, const Types&... params);
        writer<M,I,O,P> to_writer(const int& r);
        writer<M,I,O,P> to_writer(const std::string& r);
        const writer<M,I,O,P>& to_writer(const writer<M,I,O,P>& r);
        std::string get_string() const; //does handle dolars internally!
        template <dolar_mode dolars> void commit();
      public:
        /*operators*/typedef writer<M, dLet, dEat, P> basic_importer;
        /*operators*/typedef writer<M, dIgnore, dExpand, P> basic_ignorant_exporter;
        /*operators*/typedef writer<M, I, O, static_true> postprocess_allow;
        /*operators*/typedef writer<M, I, O, static_false> postprocess_deny;
        /*operators*/writer();
        /*operators*/ writer(const stringlist& writer);
        /*operators*/ writer(stringlist&& writer);
        /*operators*/template<typename N> writer(const writer<N>& writer);
        /*operators*/template<typename N> writer(writer<N>&& writer);
        /*operators*/template<typename N> writer& operator=( writer<N>&& w) ;
        /*operators*/template<typename N> writer& operator=( const writer<N>& w) ;
        /*operators*/bool operator==(const writer& w) ;
        /*operators*/bool operator!=(const writer& w) ;
        /*operators*/writer(const std::initializer_list<std::string>& init);/*this one is literal! no parsing here*/

        /*printing*/ template<dolar_mode dolars = I, typename ... Types> writer& print (const Types&... params) ;
        /*printing*/ template<dolar_mode dolars = I, typename ... Types> writer& printf(const std::string& filename, const Types&... params) ; /** print, but the first argument is a filename of a file which is to be loaded instead of the format string */
        /*printing*/ template<dolar_mode dolars = I, typename ... Types, typename Type> writer& push  (const Type& format, const Types&... params) ; /** explicit push on a new line. To be used with list_concat for simple creation of delimited lists.*/
        /*printing*/ template<dolar_mode dolars = I, typename ... Types> writer& pushf (const std::string& filename, const Types&... params) ; /** pushf is again a file-loaded version of push */
        /*printing*/ template<dolar_mode dolars = I, typename Types> writer<M,I,O,P>& append(Types str) ;

        /*output*/   template<dolar_mode dolars = O> void write(std::ostream& ss) const ;
        /*output*/   template<dolar_mode dolars = O> void write_file(std::string filename) const ;
        /*output*/   template<dolar_mode dolars = O> void write_std() const ;
        /*output*/   template<dolar_mode dolars = O> std::string write_str() const ;
        /*output*/   template<dolar_mode dolars = O> std::string write_line(int i) const ;

        /*others*/   writer& list_concat(const std::string& delim);
        /*others*/   void clear() ;
        /*others*/   int size() ;
        /*others*/   const stringlist& strings();

        /*static*/   static void to_file(const std::string& filename, const std::string& contents);
        /*static*/   static std::string from_file(const std::string& filename);
        /*static*/   static void cartesian_test() ;
        /*static*/   static void self_test() ;
    } ;

  typedef aliasenv_maker<empty_tag, language_empty> aliasenv_empty;
  typedef aliasenv_maker<empty_tag, language_cpp> aliasenv_empty_cpp;
  typedef writer<aliasenv_empty_cpp> writer_default;
  typedef writer<aliasenv_empty> writer_plain;

    template <class M, dolar_mode I, dolar_mode O, class P>
    std::string to_string(const writer<M,I,O,P>& w)
    {
      return w.write_str();
    }

  template<bool dolars = false, typename ... Types> std::string print (const Types&... params)
  {
    return writer<aliasenv_empty>().print(params...).write_str();
  }

    template <class M, dolar_mode I, dolar_mode O, class P>
  int writer<M,I,O,P>::size()
  {
    return data.size();
  }

  template <class M, dolar_mode I, dolar_mode O, class P>
    const stringlist& writer<M,I,O,P>::strings()
    {
      return data;    
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    std::string writer<M,I,O,P>::get_name(const std::string& format, int& pos)
    {
      ++pos;
      std::string buffer;
      if(isdigit(format[pos]))
      {
        while(pos < format.length() && isdigit(format[pos] ))
        {
          buffer += format[pos];
          ++pos;
        }
      }
      else
      {

        while(pos < format.length() && (isalnum(format[pos])))
        {
          buffer += format[pos];
          ++pos;
        }
      }
      return buffer;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    writer<M,I,O,P>::writer(const stringlist& s) : data(s), last_terminated(true)
    {
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    writer<M,I,O,P>::writer(stringlist&& s) : data(std::move(s)), last_terminated(true)
    {
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <typename N>
    writer<M,I,O,P>::writer(writer<N>&& w) : data(std::move(w.data)), last_terminated(w.last_terminated)
    {
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <typename N>
    writer<M,I,O,P>::writer(const writer<N>& w) : data(w.data), last_terminated(w.last_terminated)
    {
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <typename N>
    writer<M,I,O,P>& writer<M,I,O,P>::operator=(writer<N>&& w)
    {
      if(&w == this)
        return *this;

      data = std::move(w.data);
      last_terminated = w.last_terminated;
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <typename N>
    writer<M,I,O,P>& writer<M,I,O,P>::operator=(const writer<N>& w)
    {
      if((writer<M,I,O,P>*)&w == this)
        return *this;

      data.clear();
      data = w.data;
      last_terminated = w.last_terminated;
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    void writer<M,I,O,P>::trim()
    {
      if(data.empty())
        return;
      int end = data.back().find_last_not_of(" \n\r");
      int begin = data.back().find_first_not_of(" \n\r");
      if(end == -1)
      {
        data.pop_back();
        last_terminated = true;
      }
      else
        data.back() = data.back().substr(begin, end-begin+1);
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <bool preprocess>
    void writer<M,I,O,P>::add(std::string&& str, bool terminal)
    {
      if(preprocess)
      {
        buffer.append(std::move(str));
      }
      else
      {
        if(last_terminated)
          data.push_back(std::move(str));
        else
          data[data.size()-1].append(std::move(str));
        if(terminal)
          trim();
        last_terminated = terminal;
      }
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <bool preprocess>
    void writer<M,I,O,P>::process(int& pos, const std::string& format)
    {
      int from = pos;
      while(pos < format.length())
      {
        if(format[pos] == '$')
        {
          if(pos == from)
          {
            ++pos;
          }
          else
          {
            add<preprocess>(std::move(format.substr(from, pos-from)), false);
            return;
          }
        }
        else
        {
          bool brbf, braf; //break before, break after
          M::language::shouldbreak(pos, format, brbf, braf);

          if(brbf && !preprocess)
          {
            if( from == pos)
            {
              last_terminated = true;
            }
            else
            {
              add<preprocess>(std::move(format.substr(from, pos-from)), true);
              return;
            }
          }
          ++pos;
          if(braf && !preprocess)
          {
            add<preprocess>(std::move(format.substr(from, pos-from)), true);
            return;
          }
        }
      }
      if(pos != from)
        add<preprocess>(std::move(format.substr(from, pos-from)), false);
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    writer<M,I,O,P> writer<M,I,O,P>::getnth(int i)
    {
      error( "out of range on printnth");
      return writer();
    }


  template <class M, dolar_mode I, dolar_mode O, class P>
    template <typename ... Types> const writer<M,I,O,P>& writer<M,I,O,P>::getnth(int i, const writer<M,I,O,P>& a, const Types&... params)
    {
      if(i==1)
        return a;
      else
        return getnth(i-1, params...); //well if the reference arrived valid here, it will remain valid after return
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    void writer<M,I,O,P>::printnth(int i)
    {
      error( "out of range on printnth");
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <typename ... Types> void writer<M,I,O,P>::printnth(int i, const std::string& str, const Types&... params)
    {
      if(i==1)
      {
        int from = 0;
        while(from < str.length())
          process(from, str);
      }
      else
        return printnth(i-1, params...);
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <typename ... Types> void writer<M,I,O,P>::printnth(int i, const int& num, const Types&... params)
    {
      if(i==1)
      {
        printnth(i, std::to_string(num));
      }
      else
        return printnth(i-1, params...);
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <typename ... Types> void writer<M,I,O,P>::printnth(int i, const writer& wrt, const Types&... params)
    {
      if(i==1)
      {
        for(const auto& str : wrt.data)
          add(std::string(str), true);
        last_terminated = wrt.last_terminated;
      }
      else
        return printnth(i-1, params...);
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <dolar_mode dolars> 
    void writer<M,I,O,P>::write_indent(std::ostream& ss, const std::string& str, int indent, int nobreak) const
    {
      static int mynobreak = -1;

      if(mynobreak <= 0)
        for(int i = 0; i < 2 * indent; ++i)
          ss << ' ';

      for(int i = 0; i < str.length();)
      {
        if(str[i] == '$')
        {
          if(str.length() > i+1 && str[i+1] == '$')
          {
            switch (dolars)
            {
              case dExpand:
                ss << "$$$$";
                break;
              case dEat:
                ss << "$";
                break;
              case dLet:
              case dIgnore:
                ss << "$$";
                break;
            }
            i+=2;
          }
          else
          {
            switch (dolars)
            {
              case dExpand:
                ss << "$$";
                break;
              case dEat:
              case dLet:
              case dIgnore:
                ss << "$";
                break;
            }
            i+=1;
          }
        }
        else
        {
          ss << str[i];
          i+=1;
        }
      }

      mynobreak--;
      if(nobreak > 0)
        mynobreak = nobreak;

      if(mynobreak <= 0)
        ss << std::endl;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    writer<M,I,O,P>::writer() : data(), last_terminated(true)
  {
  }

  template <class M, dolar_mode I, dolar_mode O, class P>
    writer<M,I,O,P>::writer(const std::initializer_list<std::string>& init) : data(init), last_terminated(true)/*the strings are taken literary - id est not formatted*/
  {
  }

  template <class M, dolar_mode I, dolar_mode O, class P>
    std::string writer<M,I,O,P>::get_string() const
    {
      std::string str = "";
      for(auto line : data)
        str.append(line);
      return str;
    }


  template <class M, dolar_mode I, dolar_mode O, class P>
    writer<M,I,O,P> writer<M,I,O,P>::to_writer(const int& r)
    {
      return to_writer(std::to_string(r));
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    writer<M,I,O,P> writer<M,I,O,P>::to_writer(const std::string& r)
    {
      return writer<M,I,O,P>({r}); //be careful about using print - we dont want to evaluate '$$2' acidentaly
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    const writer<M,I,O,P>& writer<M,I,O,P>::to_writer(const writer<M,I,O,P>& r)
    {
      return r;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    std::vector<std::string> writer<M,I,O,P>::preprocessline(std::string line)
    {
      typedef typename aliasenv_maker<writer_tag<M,I,O,P>, language_empty>::noreport aliasenv_pre;
      aliasenv_pre::clear();
      typedef std::pair<std::vector<std::string>, std::vector<std::string> > sub_t; //substitution
      typedef std::vector<sub_t> subtab_t;
      subtab_t subtab;
      std::smatch m;
      std::regex e ("\\$\\{ *([^ }]+) *->([^}]+)\\}");
      std::regex f ("\\$\\{ *([^ },]+)[^}]*->([^}]+)\\}");
      while (std::regex_search (line,m,e))
      {
        subtab.push_back(sub_t(split(m[1],','), split(m[2], ',')));
        line = std::regex_replace(line, f, "$$$1", std::regex_constants::format_first_only);
      }
      std::vector<std::string> results;
      shake<subtab_t::iterator,aliasenv_pre>(subtab.begin(), subtab.end(), line, results);
      return results;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <typename J,typename N>
    void writer<M,I,O,P>::shake(J itr, J itre, std::string line, std::vector<std::string>& output)
    {
      typedef writer<N, dLet, dLet, static_false> preprocessor;
      if(itr == itre)
        output.push_back(preprocessor().print(line).write_str());
      else
      {
        for(auto vali = itr->second.begin(); vali != itr->second.end();)
        {
          for(auto aliasi = itr->first.begin(); aliasi != itr->first.end() && vali != itr->second.end(); ++aliasi)
          {
            N::access(*aliasi) = *vali;
            ++vali;
          }
          shake<J,N>(itr + 1, itre, line, output);
        }
      }
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <dolar_mode dolars>
    void writer<M,I,O,P>::commit()
    {
      stringlist lines = preprocessline(buffer);
      for(std::string& l : lines)
      {
        print_internal<dolars, false>(l);
      }
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template<dolar_mode dolars, bool preprocess, typename ... Types> writer<M,I,O,P>& writer<M,I,O,P>::print_internal(const std::string& format, const Types&... params)  //print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j
    {
      int from = 0;
      if(format.length() == 0) //ensure handling of empty pushes
      {
        add<preprocess>("", false);
      }
      while(from < format.length())
      {
        if(from + 1 < format.length() && format[from] == '$' && format[from+1] == '$') //handle $$
        {
          switch(dolars)
          {
          case dEat:
            add<preprocess>("$", false);
            break;
          case dLet:
          case dIgnore:
            add<preprocess>("$$", false);
            break;
          case dExpand:
            add<preprocess>("$$$$", false);
            break;
            }
          from += 2;
        }
        else if(from + 1 < format.length() && format[from] == '$' && (format[from+1] == '[' || format[from+1] == '{')) //handle $[] and ${}
        {
          if(format[from+1] == '[' && !preprocess && P::value)
          {
            int start = from+2;
            from += 2;
            while(from < format.length() && format[from] != ']')
              from++;
            if(format[from] != ']')
              error("unterminated ']' found");
            int end = from;
            from++;
            add<preprocess>(std::to_string(parser::calculate(format.substr(start,end-start))), false);
          }
          else
          {
            add<preprocess>("$", false);
            ++from;
          }
        }
        else if(from < format.length() && format[from] == '$') //handle $
        {
          int lastfrom = from;
          std::string a = get_name(format, from);
          switch (dolars)
          {
            case dEat:
            case dLet:
              if(a.empty())
                error( std::string("bad '$' in: ").append(format));
              else if( -1 == a.find_first_not_of("0123456789"))
                print_type<dolars,preprocess>(getnth(std::stoi(a), params...), params...);
              else
              {
                bool s;
                std::string e = M::alias(a,&s);
                if(s)
                  print_type<dolars,preprocess>(M::alias(a), params...);
                else
                {
                  add<preprocess>("$", false);
                  from = lastfrom+1;
                }
              }
              break;
            case dIgnore:
              add<preprocess>("$", false);
              from = lastfrom+1;
              break;
            case dExpand:
              add<preprocess>("$$", false);
              from = lastfrom+1;
              break;
          }
        }
        else //handle others
        {
          process<preprocess>(from, format);
        }
      }
      return *this;
    }


  template <class M, dolar_mode I, dolar_mode O, class P>
    template<dolar_mode dolars, bool preprocess, typename ... Types> writer<M,I,O,P>& writer<M,I,O,P>::print_type(int num, const Types&... params)  
    {
      print_type<dolars,preprocess>(std::to_string(num), params...); 
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template<dolar_mode dolars, bool preprocess, typename ... Types> writer<M,I,O,P>& writer<M,I,O,P>::print_type(const writer& format, const Types&... params)
    {
      if(&format != this)
        for(auto line : format.data)
          print_type<dolars,preprocess>(line, params...);
      else
        print_type<dolars,preprocess>(format.get_string(), params...);
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template<dolar_mode dolars, bool preprocess, typename ... Types> writer<M,I,O,P>& writer<M,I,O,P>::print_type(const std::string& format, const Types&... params)  
    {
      return print_internal<dolars,preprocess>(format, to_writer(params)...); //the purpose of this circus is not forcing (constant) writers to make several copies during a single call
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O,P>& writer<M,I,O,P>::print_branch(static_true postprocess, const Types&... params)
    {
      buffer.clear();
      print_type<dolars, true>( params...); //now we convert the first type to string
      commit<dolars>();
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O,P>& writer<M,I,O,P>::print_branch(static_false postprocess, const Types&... params)
    {
      print_type<dolars, false>( params...);
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O,P>& writer<M,I,O,P>::print(const Types&... params)  
    {
    #ifdef TESTOVANI
      std::tuple<const Types&...> t(params...);
      std::string format = ctb::to_string(std::get<0>(t));
    #endif
      return print_branch<dolars>(P(), params...); //here the P determines whether we want function with or without postprocessing (we need it to stop infinite recursion)
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O,P>& writer<M,I,O,P>::pushf(const std::string& filename, const Types&... params)
    {
      last_terminated = true;
      return printf<dolars>(filename, params...);
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template<dolar_mode dolars , typename ... Types, typename Type> writer<M,I,O,P>& writer<M,I,O,P>::push(const Type& format, const Types&... params)
    {
      //trim(); //oh no, not here!
      last_terminated = true;
      return print<dolars>(format, params...);
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O,P>& writer<M,I,O,P>::printf(const std::string& filename, const Types&... params)
    {
      return print<dolars>(from_file(filename), params...);
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <dolar_mode dolars , typename Type> writer<M,I,O,P>& writer<M,I,O,P>::append(Type str)
    {
      print<dolars>("$1", str);
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    void writer<M,I,O,P>::to_file(const std::string& filename,const std::string& contents)
    {
      std::ofstream ofs;
      openstream(ofs,filename);
      ofs << contents;
      ofs.close();
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    std::string writer<M,I,O,P>::from_file(const std::string& filename)
    {
      std::ifstream ifs;
      openstream(ifs,filename);
      std::string file( (std::istreambuf_iterator<char>(ifs) ), (std::istreambuf_iterator<char>() ) );
      return file;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    writer<M,I,O,P> &writer<M,I,O,P>::list_concat(const std::string& delim)
    {
      if(data.size() <= 1)
        return *this;
      bool first = true;
      std::string buffer;
      for(const auto& line : data)
      {
        if(!first)
          buffer.append(delim);
        buffer.append(line);
        first = false;
      }
      data.clear();
      data.push_back(buffer);
      return *this;
    }
  
    template <class M, dolar_mode I, dolar_mode O, class P>
    template <dolar_mode dolars>
  std::string writer<M,I,O,P>::write_line(int i) const
  {
    int indent = 0; 
    int nobreak = i == data.size() - 1 && !last_terminated ? 1 : 0;
    std::stringstream ss;
    write_indent<dolars>(ss, data[i], indent, nobreak);
    return ss.str();
  }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <dolar_mode dolars>
    void writer<M,I,O,P>::write(std::ostream& ss)  const
    {
      if(data.empty())
        return;
      //trim();
      int indent = 0;
      int nobreak = 0;
      for(auto itr = data.begin(); itr != data.end(); ++itr)
      {
        int outindent;
        M::language::shouldindent(*itr, outindent, indent, nobreak);
        if(nobreak <= 0 && itr + 1 == data.end() && !last_terminated)
          nobreak = 1;
        write_indent<dolars>(ss, *itr, outindent, nobreak);
      }
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <dolar_mode dolars> 
    void writer<M,I,O,P>::write_file(std::string filename)   const
    {
      std::ofstream f(filename);
      write<dolars>(f);
      f.close();
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <dolar_mode dolars> 
    std::string writer<M,I,O,P>::write_str() const
    {
      std::stringstream os;
      write<dolars>(os);
      return os.str();
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    template <dolar_mode dolars> 
    void writer<M,I,O,P>::write_std()   const
    {
      write<dolars>(std::cout);
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    void writer<M,I,O,P>::clear()  
    {
      data.clear();
      last_terminated = true;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    bool writer<M,I,O,P>::operator==(const writer& w)
    {
      return w.data == data;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    bool writer<M,I,O,P>::operator!=(const writer& w)
    {
      return w.data != data;
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    void writer<M,I,O,P>::cartesian_test()
    {
      int i=0;
      stringlist b = preprocessline("${color-> red,blue} ${animal->cat,dog} $color");
      assert(b[i++] == "red cat red");
      assert(b[i++] == "red dog red");
      assert(b[i++] == "blue cat blue");
      assert(b[i++] == "blue dog blue");
      i=0;
      stringlist c = preprocessline("${color,animal->blue,cat,red,dog} $animal");
      assert(c[i++] == "blue cat");
      assert(c[i++] == "red dog");
    }

  template <class M, dolar_mode I, dolar_mode O, class P>
    void writer<M,I,O,P>::self_test()
    {
      writer a;
      writer b;
      a.print("a[$1] = a[$1] $$ $2;{test();}", "i", "j");
      b.print("class $1{$2$2}", "test", a, a);
      int i = 0;
      assert(a.data[i++] == "a[i] = a[i] $$ j;"); //the $$ is just internal representation - there will be just '$' on output
      assert(a.data[i++] == "{");
      assert(a.data[i++] == "test();");
      assert(a.data[i++] == "}");
      i=0;
      assert(b.data[i++] == "class test");
      assert(b.data[i++] == "{");
      assert(b.data[i++] == "a[i] = a[i] $$ j;");
      assert(b.data[i++] == "{");
      assert(b.data[i++] == "test();");
      assert(b.data[i++] == "}");
      assert(b.data[i++] == "a[i] = a[i] $$ j;");
      assert(b.data[i++] == "{");
      assert(b.data[i++] == "test();");
      assert(b.data[i++] == "}");
      assert(b.data[i++] == "}");
      i=0;
      writer c ;
      c.print("class test{$1$1}", writer().print("void test(){test();switch(1){case 'a': break;case 'b': break;}}"));
      std::stringstream os;
      c.write(os);
      i=0;
      writer d;
      d.print("test $1 $2 $3", 1, writer().print("$1 $2", 2, 2),3);
      assert(d.data[i++] == "test 1 2 2 3");
      writer f( {"line1;","line2"});
      writer g;
      g.print("line1;line2");
      assert(g==f);
      writer h;
      h.print("$1", "a");
      assert(h.data[0]=="a");
      cartesian_test();
      writer k;
      k.print("$[${w->1}]");
      assert(k.data[0] == "1");
    }
};

#endif

