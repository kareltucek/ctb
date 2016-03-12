
#ifndef WRITER_GUARD
#define WRITER_GUARD

#include "defines.h"
#include "split.h"
#include "datatypes.h"
#include "aliasenv_maker.h"
#include "parser.h"
#include "errorhandling.h"

#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <regex>

namespace ctb
{
  enum dollar_mode { dEat, dLet, dExpand, dIgnore };

  struct static_true{ const static bool value = true;};
  struct static_false{ const static bool value = false;};

  template <class M> class writer_tag { M my_alias;};

  /**
   * general description
   * ===================
   *
   * Writer ensures an user-friendly code output environment. It is basically a container of code, which takes care of autoformatting of the code generated and provides a shell-like expansion environment ('a text preprocessor'), which has a printf-like interface. Unlike printf the reference format is '$<number>', so the arguments may be reused and specified in a static order. These abbreviations are expanded on both sides i.e. in both format string and its argument i.e. if it is your goal, you may create an endless loop. 
   *
   * The writer class is parametrized by a aliasenv class, which provides a translation of references of the form '$<name>' to some new format string, which is then resolved recursively within the original context.
   *
   * dollar import/export modes
   * -------------------------
   *
   * dollar_mode specifies how $ should be treated. These are specified separately for input (from print to stored internal representation) and output (from internal representation to output). Typically one will want dLet as input and dEat as output, which will result in $$a -> $$a -> $a. Modes are:
   *   - dEat - this transforms $$ to $ on the fly and expands $
   *   - dLet - will expand $ and ignore $$
   *   - dExpand - will take all $ and transform them into $$
   *   - dIgnore - will ignore all $ and $$
   *
   *   Thus typical combination will be dLet+dEat for import operations and dLet+dExpand or dIgnore+dExpand for export (import and export of text which contains 'deeper' level expansions - e.g. for preprocessing of csv files which already contain dollar records).
   *
   * the C parameter
   * ---------------
   * Whether to attempt cartesian expansion.
   *
   * The execution scheme is the following
   * -------------------------------------
   * 1. Cartesian expansion - this is done only once.
   * 2. All other types of expansions.
   *    All ${} and $[] expansions are carried out by a new writer with the same alias environment.
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
   *  Note that here we do NOT allow nested {}s
   *
   * explicit recursive expansion
   * ----------------------------
   *  Expansion in form "${ whatever that does not contain '->' }" will evaluate the content of {} and then expand the entire result as if it were standard alias. This allows us to pass arbitrary strings to the aliasenv.
   *
   *  This means that the following steps will take place:
   *
   *  1 Take the content and evaluate it (using all expansions except for the cartesian one.
   *  2 Let the aliasenv do whatever it wants with the output. The only limiting factor is that it has to find out what it wants to do with it itself.
   *  3 Again expand the result while printing it back.
   *
   *  This lets us do all the nice things that are forbidden in the C preprocessor. 
   *
   * arithmetic expression expansion
   * -------------------------------
   * $[ <arithmetic expression> ] resolves to a calculated integer value
   * supported operands are: [0-9]+, (, ), -, +, *, /, % 
   * (operators are always binary)
   *
   * formatting
   * ----------
   * Formatting is driven by a 'language' plugin structure, which is passed in as a part of a model(==aliasenv). Formatting takes place at two different places. First when strings are printed into the internal storage - during this phase, formatting takes care of cutting the input stream into ('formatted') pieces. Second part is output part, when the strings are indented.
   *
   */

  template <class M, dollar_mode I = dLet, dollar_mode O = dEat, bool C = true>
    class writer 
    {
      private:
        typedef typename aliasenv_maker<writer_tag<M>, language_empty, true, M>::noreport aliasenv_cart;
        typedef writer<M, dLet, dLet, false> preprocessor;
        stringlist data; /**holds (formatted) data*/
        bool last_terminated;
        template <dollar_mode dollars, typename ... Types> void preprocessline(string format, Types ... args);
        template <dollar_mode dollars, typename J, typename ... Types> void shake(J itr, J itre, const string& line, Types ... args);
        void trim() ;
        template  void add(string&& str, bool terminal) ;
        template  void process(int& pos, const string& format) ;
        void printnth(int i);
        //printnth is deprecated
        template <typename ... Types> void printnth(int i, const string& str, const Types&... params) ;
        template <typename ... Types> void printnth(int i, const int& num, const Types&... params) ;
        template <typename ... Types> void printnth(int i, const writer& wrt, const Types&... params) ;
        template <dollar_mode dollars> void write_indent(ostream& ss, const string& str, int indent, int nobreak) const ;
        string get_name(const string& format, int& pos);
        template <class N, dollar_mode A, dollar_mode B, bool Q> friend class writer;
        template<dollar_mode dollars, typename ... Types> bool eval_name(const string& name, const string& ctx, int pos, const Types&... params);  
        template<dollar_mode dollars, typename ... Types> void print_internal (const string& format, const Types&... params) ;
        template<dollar_mode dollars, typename ... Types> writer& print_cartesian (const string& format, const Types&... params) ;
        template<dollar_mode dollars = I, typename ... Types> writer& print_type (const string& format, const Types&... params) ;
        template<dollar_mode dollars = I, typename ... Types> writer& print_type (const writer& format, const Types&... params) ;
        template<dollar_mode dollars = I, typename ... Types> writer& print_type (int, const Types&... params) ;
        //template<dollar_mode dollars = I, typename ... Types> writer& print_branch (static_true s,  const Types&... params) ;
        //template<dollar_mode dollars = I, typename ... Types> writer& print_branch (static_false s, const Types&... params) ;
        void getnth(int i, const string*& rs, const writer<M,I,O,C>*& rw);
        template <typename ... Types> void getnth(int i, const string*& rs, const writer<M,I,O,C>*& rw, const writer<M,I,O,C>& a, const Types&... params);
        template <typename ... Types> void getnth(int i, const string*& rs, const writer<M,I,O,C>*& rw, const stringlist& a, const Types&... params);
        writer<M,I,O,C> to_writer(const int& r);
        writer<M,I,O,C> to_writer(const string& r);
        const writer<M,I,O,C>& to_writer(const writer<M,I,O,C>& r);
        const stringlist& to_writer(const stringlist& r);
        string get_string() const; //does handle dollars internally!
      public:
        /*operators*/typedef writer<M, dLet, dEat> basic_importer;
        /*operators*/typedef writer<M, dIgnore, dExpand> basic_ignorant_exporter;
        /*operators*/typedef writer<M, I, O, true> cartesian_allow;
        /*operators*/typedef writer<M, I, O, false> cartesian_deny;
        /*operators*/writer();
        /*operators*/ writer(const stringlist& writer);
        /*operators*/ writer(stringlist&& writer);
        /*operators*/template<typename N> writer(const writer<N>& writer);
        /*operators*/template<typename N> writer(writer<N>&& writer);
        /*operators*/template<typename N> writer& operator=( writer<N>&& w) ;
        /*operators*/template<typename N> writer& operator=( const writer<N>& w) ;
        /*operators*/bool operator==(const writer& w) ;
        /*operators*/bool operator!=(const writer& w) ;
        /*operators*/writer& operator+=(const writer& w) ;
        /*operators*/writer& operator+=(writer&& w) ;
        /*operators*/writer(const initializer_list<string>& init);/*this one is literal! no parsing here*/

        ///*printing*/ template<dollar_mode dollars = I, typename S, typename ... Types> writer& printl (const Types&... params, const S& stringlist) ; /*print using a stringlist at the end of the parameter list - for dynamic numbr of arguments; the last argument has to be a stringlist, but deduction does not like knowing it*/
        /*printing*/ template<dollar_mode dollars = I, typename ... Types> writer& print (const Types&... params) ;
        /*printing*/ template<dollar_mode dollars = I, typename ... Types> writer& printf(const string& filename, const Types&... params) ; /** print, but the first argument is a filename of a file which is to be loaded instead of the format string */
        /*printing*/ template<dollar_mode dollars = I, typename ... Types, typename Type> writer& push  (const Type& format, const Types&... params) ; /** explicit push on a new line. To be used with list_concat for simple creation of delimited lists.*/
        /*printing*/ template<dollar_mode dollars = I, typename ... Types> writer& pushf (const string& filename, const Types&... params) ; /** pushf is again a file-loaded version of push */
        /*printing*/ template<dollar_mode dollars = I, typename Types> writer<M,I,O,C>& append(Types str) ;

        /*output*/   template<dollar_mode dollars = O> void write(ostream& ss) const ;
        /*output*/   template<dollar_mode dollars = O> void write_file(string filename) const ;
        /*output*/   template<dollar_mode dollars = O> void write_std() const ;
        /*output*/   template<dollar_mode dollars = O> string write_str() const ;
        /*output*/   template<dollar_mode dollars = O> string write_line(int i) const ;

        /*others*/   writer& list_concat(const string& delim);
        /*others*/   void clear() ;
        /*others*/   int size() ;
        /*others*/   const stringlist& strings();

        /*static*/   static void to_file(const string& filename, const string& contents);
        /*static*/   static string from_file(const string& filename);
        /*static*/   static void cartesian_test() ;
        /*static*/   static void self_test() ;
    } ;

  typedef aliasenv_maker<empty_tag, language_cpp> aliasenv_empty_cpp;
  typedef writer<aliasenv_empty_cpp> writer_default;
  typedef writer<aliasenv_empty> writer_plain;


    template <class M, dollar_mode I, dollar_mode O, bool C>
    string to_string(const writer<M,I,O,C>& w)
    {
      return w.write_str();
    }

  template<bool dollars = false, typename ... Types> string print (const Types&... params)
  {
    return writer<aliasenv_empty>().print(params...).write_str();
  }

      template <class M, dollar_mode I, dollar_mode O, bool C>
  writer<M,I,O,C>& writer<M,I,O,C>::operator+=(writer&& w)
  {
    if(&w == this)
      error("writer does not support appending to itself");
    for(auto itr = w.data.begin(); itr != w.data.end(); ++itr)
    {
      if(last_terminated)
      {
        data.push_back(move(*itr));
      }
      else
      {
        data.back().append(move(*itr));
        last_terminated = true; 
      }
      last_terminated = w.last_terminated;
    }
    return *this;
  }

      template <class M, dollar_mode I, dollar_mode O, bool C>
  writer<M,I,O,C>& writer<M,I,O,C>::operator+=(const writer& w)
  {
    if(&w == this)
      error("writer does not support appending to itself");
    for(auto itr = w.data.begin(); itr != w.data.end(); ++itr)
    {
      if(last_terminated)
      {
        data.push_back(*itr);
      }
      else
      {
        data.back().append(*itr);
        last_terminated = true; 
      }
      last_terminated = w.last_terminated;
    }
    return *this;
  }

    template <class M, dollar_mode I, dollar_mode O, bool C>
  int writer<M,I,O,C>::size()
  {
    return data.size();
  }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    const stringlist& writer<M,I,O,C>::strings()
    {
      return data;    
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    string writer<M,I,O,C>::get_name(const string& format, int& pos)
    {
      ++pos;
      string buffer;
      if(isdigit(format[pos]))
      {
        while(pos < format.length() && isdigit(format[pos] ))
        {
          buffer += format[pos];
          ++pos;
        }
      }
      /* //now we handle this case as the arithmetic expansion
      else if(format[pos] == '{')
      {
        ++pos;
        while(pos < format.length() && format[pos] != '}' )
        {
          buffer += format[pos];
          ++pos;
        }
        ++pos;
      }
      */
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

  template <class M, dollar_mode I, dollar_mode O, bool C>
    writer<M,I,O,C>::writer(const stringlist& s) : data(s), last_terminated(true)
    {
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    writer<M,I,O,C>::writer(stringlist&& s) : data(move(s)), last_terminated(true)
    {
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <typename N>
    writer<M,I,O,C>::writer(writer<N>&& w) : data(move(w.data)), last_terminated(w.last_terminated)
    {
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <typename N>
    writer<M,I,O,C>::writer(const writer<N>& w) : data(w.data), last_terminated(w.last_terminated)
    {
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <typename N>
    writer<M,I,O,C>& writer<M,I,O,C>::operator=(writer<N>&& w)
    {
      if(&w == this)
        return *this;

      data = move(w.data);
      last_terminated = w.last_terminated;
      return *this;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <typename N>
    writer<M,I,O,C>& writer<M,I,O,C>::operator=(const writer<N>& w)
    {
      if((writer<M,I,O,C>*)&w == this)
        return *this;

      data.clear();
      data = w.data;
      last_terminated = w.last_terminated;
      return *this;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    void writer<M,I,O,C>::trim()
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

  template <class M, dollar_mode I, dollar_mode O, bool C>
    void writer<M,I,O,C>::add(string&& str, bool terminal)
    {
        if(last_terminated)
          data.push_back(move(str));
        else
          data[data.size()-1].append(move(str));
        if(terminal)
          trim();
        last_terminated = terminal;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    void writer<M,I,O,C>::process(int& pos, const string& format)
    {
      static const bool preprocess = false;
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
            add(move(format.substr(from, pos-from)), false);
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
              add(move(format.substr(from, pos-from)), true);
              return;
            }
          }
          ++pos;
          if(braf && !preprocess)
          {
            add(move(format.substr(from, pos-from)), true);
            return;
          }
        }
      }
      if(pos != from)
        add(move(format.substr(from, pos-from)), false);
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    void writer<M,I,O,C>::getnth(int i, const string*& rs, const writer<M,I,O,C>*& rw)
    {
      warning( "out of range on printnth");
      rs = NULL;
      rw = NULL;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <typename ... Types> void writer<M,I,O,C>::getnth(int i, const string*& rs, const writer<M,I,O,C>*& rw, const stringlist& a, const Types&... params)
    {
      if(i > a.size())
      {
        getnth(i-a.size(), rs, rw, params...);
      }
      else
      {
        rs = &a[i-1];
        rw = NULL;
      }
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <typename ... Types> void writer<M,I,O,C>::getnth(int i, const string*& rs, const writer<M,I,O,C>*& rw, const writer<M,I,O,C>& a, const Types&... params)
    {
      if(i==1)
      {
        rs = NULL;
        rw = &a;
      }
      else
      {
        getnth(i-1, rs, rw, params...);
      }
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    void writer<M,I,O,C>::printnth(int i)
    {
      error( "out of range on printnth");
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <typename ... Types> void writer<M,I,O,C>::printnth(int i, const string& str, const Types&... params)
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

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <typename ... Types> void writer<M,I,O,C>::printnth(int i, const int& num, const Types&... params)
    {
      if(i==1)
      {
        printnth(i, to_string(num));
      }
      else
        return printnth(i-1, params...);
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <typename ... Types> void writer<M,I,O,C>::printnth(int i, const writer& wrt, const Types&... params)
    {
      if(i==1)
      {
        for(const auto& str : wrt.data)
          add(string(str), true);
        last_terminated = wrt.last_terminated;
      }
      else
        return printnth(i-1, params...);
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <dollar_mode dollars> 
    void writer<M,I,O,C>::write_indent(ostream& ss, const string& str, int indent, int nobreak) const
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
            switch (dollars)
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
            switch (dollars)
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
        ss << endl;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    writer<M,I,O,C>::writer() : data(), last_terminated(true)
  {
  }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    writer<M,I,O,C>::writer(const initializer_list<string>& init) : data(init), last_terminated(true)/*the strings are taken literary - id est not formatted*/
  {
  }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    string writer<M,I,O,C>::get_string() const
    {
      string str = "";
      for(auto line : data)
        str.append(line);
      return str;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    const stringlist& writer<M,I,O,C>::to_writer(const stringlist& r)
    {
      return r;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    writer<M,I,O,C> writer<M,I,O,C>::to_writer(const int& r)
    {
      return to_writer(to_string(r));
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    writer<M,I,O,C> writer<M,I,O,C>::to_writer(const string& r)
    {
      return writer<M,I,O,C>({r}); //be careful about using print - we dont want to evaluate '$$2' acidentaly
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    const writer<M,I,O,C>& writer<M,I,O,C>::to_writer(const writer<M,I,O,C>& r)
    {
      return r;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <dollar_mode dollars, typename ... Types>
    void writer<M,I,O,C>::preprocessline(string line, Types ... args)
    {
      typedef pair<vector<string>, vector<string> > sub_t; //substitution
      typedef vector<sub_t> subtab_t;
      subtab_t subtab;
      smatch m;
#ifdef HAS_WORKING_STD_LIBS
      static const regex e ("\\$\\{ *([^ }]+) *->([^}]+)\\}");
      static const regex f ("\\$\\{ *([^ },]+)[^}]*->([^}]+)\\}");
      while (regex_search (line,m,e))
      {
        subtab.push_back(sub_t(split(m[1],','), split(m[2], ',')));
        line = regex_replace(line, f, string("$$$1"), regex_constants::format_first_only);
      }
#else
      {
        int offset = 0;
        int s,a,e;
        string g1,g2,pre,pos;
        while(true)
        {
          s = line.find("${", offset);
          if(s == string::npos) break;
          a = line.find("->", s);
          if(a == string::npos) break;
          e = line.find("}", s);
          if(e == string::npos) break;
          g1 = ::ctb::trim(line.substr(s+2, a-s-2));
          g2 = ::ctb::trim(line.substr(a+2, e-a-2));
          if(g1.empty() || a > e)
          {
            offset = s+1;
            continue;
          }
          pre = line.substr(0,s);
          pos = line.substr(e+1,line.length()-e-1);
          subtab.push_back(sub_t(split(g1,','), split(g2, ',')));
          line = pre + "$" + split(g1,',')[0] + pos;
        }
      }
#endif
      if(!subtab.empty())
        line = line + "\n";
      shake<dollars, subtab_t::iterator>(subtab.begin(), subtab.end(), line, args...);
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <dollar_mode dollars, typename J, typename ... Types>
    void writer<M,I,O,C>::shake(J itr, J itre, const string& line, Types... args)
    {
      if(itr == itre)
      {
        print_internal<dollars>(line, args...);
      }
      else
      {
        for(auto vali = itr->second.begin(); vali != itr->second.end();)
        {
          for(auto aliasi = itr->first.begin(); aliasi != itr->first.end() && vali != itr->second.end(); ++aliasi)
          {
            aliasenv_cart::access(*aliasi) = *vali;
            ++vali;
          }
          shake<dollars,J,Types...>(itr + 1, itre, line, args...);
        }
      }
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> writer<M,I,O,C>& writer<M,I,O,C>::print_cartesian(const string& format, const Types&... params)  
    {
      try
      {
      if(C)
      {
        aliasenv_cart::push();
        preprocessline<dollars>(format, params...);
        aliasenv_cart::pop();
      }
      else
      {
        print_internal<dollars>(format, params...); //here the P determines whether we want function with or without postprocessing (we need it to stop infinite recursion)
      }
      }
      RETHROW(string("while evaluating ") + format );
      return *this;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> bool writer<M,I,O,C>::eval_name(const string& name, const string& ctx, int pos, const Types&... params)  
    {
      try
      {
        if(name.empty())
        {
          error( string("bad '$' expression: ").append(name));
        }
        else if( -1 == name.find_first_not_of("0123456789"))
        {
          int n = ctb::stoi(name);
          const string* sptr;
          const writer<M,I,O,C> * wptr;
          getnth(n, sptr, wptr, params...);
          if(sptr)
            print_type<dollars>(*sptr, params...);
          if(wptr)
            print_type<dollars>(*wptr, params...);
        }
        else
        {
          bool s;
          string e = aliasenv_cart::alias(name,&s);
          if(s)
            print_internal<dollars>(e, params...);
          else
          {
            add("$", false);
            return false;
          }
        }
      }
      RETHROW(string("at position ") + ctb::to_string(pos) + " in expression: " + ctx);
        return true;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> void writer<M,I,O,C>::print_internal(const string& format, const Types&... params)  //print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j
    {
#ifdef TESTOVANI
      const char* f = format.c_str();
#endif
      int from = 0;
      if(format.length() == 0) //ensure handling of empty pushes
      {
        add("", false);
      }
      while(from < format.length())
      {
        if(from + 1 < format.length() && format[from] == '$' && format[from+1] == '$') //handle $$
        {
          switch(dollars)
          {
          case dEat:
            add("$", false);
            break;
          case dLet:
          case dIgnore:
            add("$$", false);
            break;
          case dExpand:
            add("$$$$", false);
            break;
            }
          from += 2;
        }
        else if(from + 1 < format.length() && format[from] == '$' && (format[from+1] == '[' || format[from+1] == '{')) //handle $[] and ${}
        {
          char pb = format[from+1];
          char pe = format[from+1] == '[' ? ']' : '}';
          int lastfrom = from;

          if(format[from+1] == pb) // && !preprocess && P::value)
          {
            int indent = 1;
            int start = from+2;
            from += 2;
            while(from < format.length() && indent > 0)
            {
              if(format[from] == pb)
                indent++;
              if(format[from] == pe)
                indent--;
              from++;
            }
            if(format[from-1] != pe)
              error(string("unterminated ") + pe +" found");
            int end = from-1;
            preprocessor w;
            w.print(format.substr(start,end-start), params...);
            if(pb == '[')
              add(to_string(parser::calculate(w.write_str())), false);
            else if(pb == '{')
            {
              if(!eval_name<dollars>(w.write_str(), format, from, params...) )
                from = lastfrom+1;
            }
          }
          else
          {
            add("$", false);
            ++from;
          }
        }
        else if(from < format.length() && format[from] == '$') //handle $
        {
          int lastfrom = from;
          string name = get_name(format, from);
          switch (dollars)
          {
            case dEat:
            case dLet:
              if(!eval_name<dollars>(name, format, from, params...))
                from = lastfrom+1;
              break;
            case dIgnore:
              add("$", false);
              from = lastfrom+1;
              break;
            case dExpand:
              add("$$", false);
              from = lastfrom+1;
              break;
          }
        }
        else //handle others
        {
          process(from, format);
        }
      }
    }


  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> writer<M,I,O,C>& writer<M,I,O,C>::print_type(int num, const Types&... params)
    {
      print_type<dollars>(to_string(num), params...);
      return *this;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> writer<M,I,O,C>& writer<M,I,O,C>::print_type(const writer& format, const Types&... params)
    {
      if(&format != this)
      {
        for(auto line : format.data)
        {
          print_type<dollars>( line, params...);
        }
      }
      else
      {
        print_type<dollars>( format.get_string(), params...);
      }
      return *this;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> writer<M,I,O,C>& writer<M,I,O,C>::print_type( const string& format, const Types&... params)
    {
      return print_cartesian<dollars>( format, to_writer(params)...); //the purpose of this circus is not forcing (constant) writers to make several copies during a single call
    }

  /*
  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> writer<M,I,O,C>& writer<M,I,O,C>::print_branch(static_true postprocess, const Types&... params)
    {
      return *this;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> writer<M,I,O,C>& writer<M,I,O,C>::print_branch(static_false postprocess, const Types&... params)
    {
      print_type<dollars, false>( params...);
      return *this;
    }
    */

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> writer<M,I,O,C>& writer<M,I,O,C>::print(const Types&... params)
    {
    #ifdef TESTOVANI
      tuple<const Types&...> t(params...);
      string format = ctb::to_string(get<0>(t));
    #endif
      return print_type<dollars>(params...);
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> writer<M,I,O,C>& writer<M,I,O,C>::pushf(const string& filename, const Types&... params)
    {
      last_terminated = true;
      return printf<dollars>(filename, params...);
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars , typename ... Types, typename Type> writer<M,I,O,C>& writer<M,I,O,C>::push(const Type& format, const Types&... params)
    {
      //trim(); //oh no, not here!
      last_terminated = true;
      return print<dollars>(format, params...);
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template<dollar_mode dollars, typename ... Types> writer<M,I,O,C>& writer<M,I,O,C>::printf(const string& filename, const Types&... params)
    {
      return print<dollars>(from_file(filename), params...);
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <dollar_mode dollars , typename Type> writer<M,I,O,C>& writer<M,I,O,C>::append(Type str)
    {
      print<dollars>("$1", str);
      return *this;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    void writer<M,I,O,C>::to_file(const string& filename,const string& contents)
    {
      ofstream ofs;
      openstream(ofs,filename);
      ofs << contents;
      ofs.close();
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    string writer<M,I,O,C>::from_file(const string& filename)
    {
      ifstream ifs;
      openstream(ifs,filename);
      string file( (istreambuf_iterator<char>(ifs) ), (istreambuf_iterator<char>() ) );
      return file;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    writer<M,I,O,C> &writer<M,I,O,C>::list_concat(const string& delim)
    {
      if(data.size() <= 1)
        return *this;
      bool first = true;
      string buffer;
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
  
    template <class M, dollar_mode I, dollar_mode O, bool C>
    template <dollar_mode dollars>
  string writer<M,I,O,C>::write_line(int i) const
  {
    int indent = 0; 
    int nobreak = i == data.size() - 1 && !last_terminated ? 1 : 0;
    stringstream ss;
    write_indent<dollars>(ss, data[i], indent, nobreak);
    return ss.str();
  }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <dollar_mode dollars>
    void writer<M,I,O,C>::write(ostream& ss)  const
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
        write_indent<dollars>(ss, *itr, outindent, nobreak);
      }
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <dollar_mode dollars> 
    void writer<M,I,O,C>::write_file(string filename)   const
    {
      ofstream f(filename);
      write<dollars>(f);
      f.close();
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <dollar_mode dollars> 
    string writer<M,I,O,C>::write_str() const
    {
      stringstream os;
      write<dollars>(os);
      return os.str();
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    template <dollar_mode dollars> 
    void writer<M,I,O,C>::write_std()   const
    {
      write<dollars>(cout);
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    void writer<M,I,O,C>::clear()  
    {
      data.clear();
      last_terminated = true;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    bool writer<M,I,O,C>::operator==(const writer& w)
    {
      return w.data == data;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    bool writer<M,I,O,C>::operator!=(const writer& w)
    {
      return w.data != data;
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    void writer<M,I,O,C>::cartesian_test()
    {
      int i=0;
      writer w;
      stringlist b = w.print("${color-> red,blue} ${animal->cat,dog} $color").data;
      assert(b[i++] == "red cat red");
      assert(b[i++] == "red dog red");
      assert(b[i++] == "blue cat blue");
      assert(b[i++] == "blue dog blue");
      i=0;
      writer v;
      stringlist c = v.print("${color,animal->blue,cat,red,dog} $animal").data;
      assert(c[i++] == "blue cat");
      assert(c[i++] == "red dog");
    }

  template <class M, dollar_mode I, dollar_mode O, bool C>
    void writer<M,I,O,C>::self_test()
    {
      cout << "testing testing writer" << endl;
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
      stringstream os;
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
      writer l;
      stringlist s = {"b", "c"};
      l.print("$1 $2 $3 $4 $5 $6", "a", s, "d", s);
      assert(l.data[0] == "a b c d b c");


      writer m;
      m.print("${b -> cat} ${ a -> b } ${ba->$1} - ${$a} ${${a}a}", "snake");
      assert(m.data[0] == "cat b snake - cat snake");

      writer n;
      n.print("${b -> 33} / 11 + $1 = $[ $b / 11 + $1 ]", 1);
      assert(n.data[0] == "33 / 11 + 1 = 4");
    }
};

#endif

