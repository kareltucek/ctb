
#ifndef WRITER_GUARD
#define WRITER_GUARD

#include "datatypes.h"

#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <sstream>

namespace ctb
{
  /**
   * Writer ensures an user-friendly code output environment. It is basically a container of code, which takes care of autoformatting of the code generated and provides a shell-like expansion environment ('a text preprocessor'), which has a printf-like interface. Unlike printf the reference format is '$<number>', so the arguments may be reused and specified in a static order. These abbreviations are expanded on both sides i.e. in both format string and its argument i.e. if it is your goal, you may create an endless loop. 
   *
   * The writer class is parametrized by a model class, which provides a translation of references of the form '$<name>' to some new format string, which is then resolved recursively within the original context.
   *
   * Dolar_mode specifies how $ should be treated. These are specified separately for input and output. Typically one will want dLet as input and dEat as output, which will result in $$a -> $$a -> $a. Modes are:
   *   dEat - this transforms $$ to $ on the fly and expands $
   *   dLet - will expand $ and ignore $$
   *   dExpand - will take all $ and transform them into $$
   *   dIgnore - will ignore all $ and $$
   *   Thus typical combination will be dLet+dEat for import operations and dLet+dExpand for export (import and export of text which contains 'deeper' level expansions - e.g. for preprocessing of csv files which already contain dolar records).
   */

  enum dolar_mode { dEat, dLet, dExpand, dIgnore };
  template <class M, dolar_mode I = dLet, dolar_mode O = dEat>
    class writer //will provide methods for outputting the generated code - e.g. autoindent, output to files or stdout
    {
      private:
        std::vector<std::string> data;
        bool last_terminated;
        void trim() ;
        void add(std::string&& str, bool terminal) ;
        void process(int& pos, const std::string& format) ;
        void printnth(int i);
        //printnth is deprecated
        template <typename ... Types> void printnth(int i, const std::string& str, const Types&... params) ;
        template <typename ... Types> void printnth(int i, const int& num, const Types&... params) ;
        template <typename ... Types> void printnth(int i, const writer& wrt, const Types&... params) ;
        template <dolar_mode dolars> void write_indent(std::ostream& ss, const std::string& str, int indent, int nobreak) ;
        std::string get_name(const std::string& format, int& pos);
        template <class N, dolar_mode A, dolar_mode B> friend class writer;
        template<dolar_mode dolars, typename ... Types> writer& print_internal (const std::string& format, const Types&... params) ; //should accept at least string and another writer class e.g. print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j
        writer<M,I,O> getnth(int i);
        template <typename ... Types> const writer<M,I,O>& getnth(int i, const writer<M,I,O>& a, const Types&... params);
        writer<M,I,O> to_writer(const int& r);
        writer<M,I,O> to_writer(const std::string& r);
        const writer<M,I,O>& to_writer(const writer<M,I,O>& r);
        std::string get_string() const; //does handle dolars internally!
      public:
        typedef writer<M, dLet, dEat> basic_importer;
        typedef writer<M, dIgnore, dExpand> basic_ignorant_exporter;
        writer();
        template<typename N> writer(const writer<N>& writer);
        template<typename N> writer(writer<N>&& writer);
        template<typename N> writer& operator=( writer<N>&& w) ;
        template<typename N> writer& operator=( const writer<N>& w) ;
        writer(const std::initializer_list<std::string>& init);/*this one is literal! no parsing here*/
        template<dolar_mode dolars = I, typename ... Types> writer& print (const std::string& format, const Types&... params) ; /**should accept at least string and another writer class e.g. print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j*/
        template<dolar_mode dolars = I, typename ... Types> writer& print (const writer& format, const Types&... params) ;
        template<dolar_mode dolars = I, typename ... Types> writer& print (int, const Types&... params) ;
        template<dolar_mode dolars = I, typename ... Types> writer& printf(const std::string& filename, const Types&... params) ; /** print, but the first argument is a filename of a file which is to be loaded instead of the format string */
        template<dolar_mode dolars = I, typename ... Types, typename Type> writer& push  (const Type& format, const Types&... params) ; /** explicit push on a new line. To be used with list_concat for simple creation of delimited lists.*/
        template<dolar_mode dolars = I, typename ... Types> writer& pushf (const std::string& filename, const Types&... params) ; /** pushf is again a file-loaded version of push */
        template<dolar_mode dolars = I, typename Types> writer<M,I,O>& append(Types str) ;
        writer& list_concat(const std::string& delim);
        static std::string from_file(const std::string& filename);
        static void to_file(const std::string& filename, const std::string& contents);
        template<dolar_mode dolars = O> void write(std::ostream& ss) ;
        template<dolar_mode dolars = O> void write_file(std::string filename) ;
        template<dolar_mode dolars = O> void write_std() ;
        template<dolar_mode dolars = O> std::string write_str() ;
        void clear() ;
        bool operator==(const writer& w) ;
        bool operator!=(const writer& w) ;
        static void self_test() ;
    } ;

  template <class M, dolar_mode I, dolar_mode O>
    std::string writer<M,I,O>::get_name(const std::string& format, int& pos)
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

  template <class M, dolar_mode I, dolar_mode O>
    template <typename N>
    writer<M,I,O>::writer(writer<N>&& w) : data(std::move(w.data)), last_terminated(w.last_terminated)
    {
    }

  template <class M, dolar_mode I, dolar_mode O>
    template <typename N>
    writer<M,I,O>::writer(const writer<N>& w) : data(w.data), last_terminated(w.last_terminated)
    {
    }

  template <class M, dolar_mode I, dolar_mode O>
    template <typename N>
    writer<M,I,O>& writer<M,I,O>::operator=(writer<N>&& w)
    {
      if(&w == this)
        return *this;

      data = std::move(w.data);
      last_terminated = w.last_terminated;
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O>
    template <typename N>
    writer<M,I,O>& writer<M,I,O>::operator=(const writer<N>& w)
    {
      if((writer<M,I,O>*)&w == this)
        return *this;

      data.clear();
      data = w.data;
      last_terminated = w.last_terminated;
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O>
    void writer<M,I,O>::trim()
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

  template <class M, dolar_mode I, dolar_mode O>
    void writer<M,I,O>::add(std::string&& str, bool terminal)
    {
      if(last_terminated)
        data.push_back(std::move(str));
      else
        data[data.size()-1].append(std::move(str));
      if(terminal)
        trim();
      last_terminated = terminal;
    }

  template <class M, dolar_mode I, dolar_mode O>
    void writer<M,I,O>::process(int& pos, const std::string& format)
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
            add(std::move(format.substr(from, pos-from)), false);
            return;
          }
        }
        else
        {
          bool brbf, braf; //break before, break after
          M::language::shouldbreak(pos, format, brbf, braf);

          if(brbf) 
          {
            if( from == pos)
            {
              last_terminated = true;
            }
            else
            {
              add(std::move(format.substr(from, pos-from)), true);
              return;
            }
          }
          ++pos;
          if(braf)
          {
            add(std::move(format.substr(from, pos-from)), true);
            return;
          }
        }
      }
      if(pos != from)
        add(std::move(format.substr(from, pos-from)), false);
    }

  template <class M, dolar_mode I, dolar_mode O>
    writer<M,I,O> writer<M,I,O>::getnth(int i)
    {
      error( "out of range on pintnth");
      return writer();
    }


  template <class M, dolar_mode I, dolar_mode O>
    template <typename ... Types> const writer<M,I,O>& writer<M,I,O>::getnth(int i, const writer<M,I,O>& a, const Types&... params)
    {
      if(i==1)
        return a;
      else
        return getnth(i-1, params...); //well if the reference arrived valid here, it will remain valid after return
    }

  template <class M, dolar_mode I, dolar_mode O>
    void writer<M,I,O>::printnth(int i)
    {
      error( "out of range on pintnth");
    }

  template <class M, dolar_mode I, dolar_mode O>
    template <typename ... Types> void writer<M,I,O>::printnth(int i, const std::string& str, const Types&... params)
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

  template <class M, dolar_mode I, dolar_mode O>
    template <typename ... Types> void writer<M,I,O>::printnth(int i, const int& num, const Types&... params)
    {
      if(i==1)
      {
        printnth(i, std::to_string(num));
      }
      else
        return printnth(i-1, params...);
    }

  template <class M, dolar_mode I, dolar_mode O>
    template <typename ... Types> void writer<M,I,O>::printnth(int i, const writer& wrt, const Types&... params)
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

  template <class M, dolar_mode I, dolar_mode O>
    template <dolar_mode dolars> 
    void writer<M,I,O>::write_indent(std::ostream& ss, const std::string& str, int indent, int nobreak)
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

  template <class M, dolar_mode I, dolar_mode O>
    writer<M,I,O>::writer() : data(), last_terminated(true)
  {
  }

  template <class M, dolar_mode I, dolar_mode O>
    writer<M,I,O>::writer(const std::initializer_list<std::string>& init) : data(init), last_terminated(true)/*the strings are taken literary - id est not formatted*/
  {
  }

  template <class M, dolar_mode I, dolar_mode O>
    std::string writer<M,I,O>::get_string() const
    {
      std::string str = "";
      for(auto line : data)
        str.append(line);
      return str;
    }

  template <class M, dolar_mode I, dolar_mode O>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O>& writer<M,I,O>::print(const writer& format, const Types&... params)
    {
      if(&format != this)
        for(auto line : format.data)
          print<dolars>(line, params...);
      else
        print<dolars>(format.get_string(), params...);

      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O>
    writer<M,I,O> writer<M,I,O>::to_writer(const int& r)
    {
      return to_writer(std::to_string(r));
    }

  template <class M, dolar_mode I, dolar_mode O>
    writer<M,I,O> writer<M,I,O>::to_writer(const std::string& r)
    {
      return writer<M,I,O>({r}); //be careful about using print - we dont want to evaluate '$$2' acidentaly
    }

  template <class M, dolar_mode I, dolar_mode O>
    const writer<M,I,O>& writer<M,I,O>::to_writer(const writer<M,I,O>& r)
    {
      return r;
    }

  template <class M, dolar_mode I, dolar_mode O>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O>& writer<M,I,O>::print_internal(const std::string& format, const Types&... params)  //print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j
    {
      int from = 0;
      if(format.length() == 0) //ensure handling of empty pushes
      {
        add("", false);
      }
      while(from < format.length())
      {
        if(from + 1 < format.length() && format[from] == '$' && format[from+1] == '$') //handle $$
        {
          switch(dolars)
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
        else if(from + 1 < format.length() && format[from] == '$') //handle $
        {
          std::string a = get_name(format, from);
          switch (dolars)
          {
            case dEat:
            case dLet:
              if(a.empty())
                error( std::string("bad '$' in: ").append(format));
              else if( -1 == a.find_first_not_of("0123456789"))
                print<dolars>(getnth(std::stoi(a), params...), params...);
              else
                print<dolars>(M::alias(a), params...);
              break;
            case dIgnore:
              add("$", false);
              add(std::string(a), false);
              break;
            case dExpand:
              add("$$", false);
              add(std::string(a), false);
              break;
          }
        }
        else //handle others
        {
          process(from, format);
        }
      }
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O>& writer<M,I,O>::print(int num, const Types&... params)  
    {
      print<dolars>(std::to_string(num), params...); 
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O>& writer<M,I,O>::print(const std::string& format, const Types&... params)  //print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j
    {
      print_internal<dolars>(format, to_writer(params)...); //the purpose of this circus is not forcing (constant) writers to make several copies during a single call
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O>& writer<M,I,O>::pushf(const std::string& filename, const Types&... params)
    {
      last_terminated = true;
      return printf<dolars>(filename, params...);
    }

  template <class M, dolar_mode I, dolar_mode O>
    template<dolar_mode dolars , typename ... Types, typename Type> writer<M,I,O>& writer<M,I,O>::push(const Type& format, const Types&... params)
    {
      //trim(); //oh no, not here!
      last_terminated = true;
      return print<dolars>(format, params...);
    }

  template <class M, dolar_mode I, dolar_mode O>
    template<dolar_mode dolars, typename ... Types> writer<M,I,O>& writer<M,I,O>::printf(const std::string& filename, const Types&... params)
    {
      return print<dolars>(from_file(filename), params...);
    }

  template <class M, dolar_mode I, dolar_mode O>
    template <dolar_mode dolars , typename Type> writer<M,I,O>& writer<M,I,O>::append(Type str)
    {
      print<dolars>("$1", str);
      return *this;
    }

  template <class M, dolar_mode I, dolar_mode O>
    void writer<M,I,O>::to_file(const std::string& filename,const std::string& contents)
    {
      std::ofstream ofs(filename);
      ofs << contents;
      ofs.close();
    }

  template <class M, dolar_mode I, dolar_mode O>
    std::string writer<M,I,O>::from_file(const std::string& filename)
    {
      std::ifstream ifs;
      openstream(ifs,filename);
      std::string file( (std::istreambuf_iterator<char>(ifs) ), (std::istreambuf_iterator<char>() ) );
      return file;
    }

  template <class M, dolar_mode I, dolar_mode O>
    writer<M,I,O> &writer<M,I,O>::list_concat(const std::string& delim)
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

  template <class M, dolar_mode I, dolar_mode O>
    template <dolar_mode dolars>
    void writer<M,I,O>::write(std::ostream& ss) 
    {
      if(data.empty())
        return;
      trim();
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

  template <class M, dolar_mode I, dolar_mode O>
    template <dolar_mode dolars> 
    void writer<M,I,O>::write_file(std::string filename)  
    {
      std::ofstream f(filename);
      write<dolars>(f);
      f.close();
    }

  template <class M, dolar_mode I, dolar_mode O>
    template <dolar_mode dolars> 
    std::string writer<M,I,O>::write_str()
    {
      std::stringstream os;
      write<dolars>(os);
      return os.str();
    }

  template <class M, dolar_mode I, dolar_mode O>
    template <dolar_mode dolars> 
    void writer<M,I,O>::write_std()  
    {
      write<dolars>(std::cout);
    }

  template <class M, dolar_mode I, dolar_mode O>
    void writer<M,I,O>::clear()  
    {
      data.clear();
      last_terminated = true;
    }

  template <class M, dolar_mode I, dolar_mode O>
    bool writer<M,I,O>::operator==(const writer& w)
    {
      return w.data == data;
    }

  template <class M, dolar_mode I, dolar_mode O>
    bool writer<M,I,O>::operator!=(const writer& w)
    {
      return w.data != data;
    }

  template <class M, dolar_mode I, dolar_mode O>
    void writer<M,I,O>::self_test()
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
    }
};

#endif

