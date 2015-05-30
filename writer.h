
#ifndef WRITER_GUARD
#define WRITER_GUARD

#include "datatypes.h"

#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <sstream>

template <class M>
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
    void write_indent(std::ostream& ss, const std::string& str, int indent, int nobreak) ;
    std::string get_name(const std::string& format, int& pos);
    template <class N> friend class writer;
    template<bool dolars, typename ... Types> writer& print_internal (const std::string& format, const Types&... params) ; //should accept at least string and another writer class e.g. print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j
    writer<M> getnth(int i);
    template <typename ... Types> const writer<M>& getnth(int i, const writer<M>& a, const Types&... params);
    writer<M> to_writer(const int& r);
    writer<M> to_writer(const std::string& r);
    const writer<M>& to_writer(const writer<M>& r);
    std::string get_string() const; //does handle dolars internally!
  public:
    writer();
    template<typename N> writer(const writer<N>& writer);
    template<typename N> writer(writer<N>&& writer);
    template<typename N> writer& operator=( writer<N>&& w) ;
    template<typename N> writer& operator=( const writer<N>& w) ;
    writer(const std::initializer_list<std::string>& init);/*this one is literal! no parsing here*/
    template<bool dolars = false, typename ... Types> writer& print (const std::string& format, const Types&... params) ; //should accept at least string and another writer class e.g. print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j
    template<bool dolars = false, typename ... Types> writer& print (const writer& format, const Types&... params) ;
    template<bool dolars = false, typename ... Types> writer& printf(const std::string& filename, const Types&... params) ;
    template<bool dolars = false, typename ... Types> writer& pushf (const std::string& filename, const Types&... params) ;
    template<bool dolars = false, typename ... Types, typename Type> writer& push  (const Type& format, const Types&... params) ;
    writer& list_concat(const std::string& delim);
    template <bool dolars = false, typename Types> writer<M>& append(Types str) ;
    static std::string from_file(const std::string& filename);
    void write(std::ostream& ss) ;
    void write_file(std::string filename) ; 
    void write_std() ; 
    std::string write_str() ;
    void clear() ;
    bool operator==(const writer& w) ;
    bool operator!=(const writer& w) ;
    static void self_test() ;
} ;

  template <class M>
std::string writer<M>::get_name(const std::string& format, int& pos)
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

template <class M>
template <typename N>
writer<M>::writer(writer<N>&& w) : data(std::move(w.data)), last_terminated(w.last_terminated)
{
}

template <class M>
template <typename N>
writer<M>::writer(const writer<N>& w) : data(w.data), last_terminated(w.last_terminated)
{
}

template <class M>
template <typename N>
writer<M>& writer<M>::operator=(writer<N>&& w)
{
  if(&w == this)
    return *this;

  data = std::move(w.data);
  last_terminated = w.last_terminated;
  return *this;
}

template <class M>
template <typename N>
writer<M>& writer<M>::operator=(const writer<N>& w)
{
  if((writer<M>*)&w == this)
    return *this;

  data.clear();
  data = w.data;
  last_terminated = w.last_terminated;
  return *this;
}

template <class M>
void writer<M>::trim()
{
  if(data.empty())
    return;
  int end = data.back().find_last_not_of(" \n\r\t");
  int begin = data.back().find_first_not_of(" \n\t\t");
  if(end == -1)
  {
    data.pop_back();
    last_terminated = true;
  }
  else
    data.back() = data.back().substr(begin, end-begin+1);
}

template <class M>
void writer<M>::add(std::string&& str, bool terminal)
{
  if(last_terminated)
    data.push_back(std::move(str));
  else
    data[data.size()-1].append(std::move(str));
  if(terminal)
    trim();
  last_terminated = terminal;
}

template <class M>
void writer<M>::process(int& pos, const std::string& format)
{
  int from = pos;
  while(pos < format.length())
  {
    switch(format[pos])
    {
    case '$':
      if(pos == from)
        break;
      add(std::move(format.substr(from, pos-from)), false);
      return;
    case ':':
      if((pos > 0 && format[pos-1] == ':') || ( format.size() > pos && format[pos+1] == ':'))
        break;
    case ';':
    case '\n':
    case '\r':
    case '?':
      ++pos;
      add(std::move(format.substr(from, pos-from)), true);
      return;
    case '}':
    case '{':
      if(pos == from)
      {
        last_terminated = true;
        ++pos;
      }
      add(std::move(format.substr(from, pos-from)), true);
      return;
    }
    ++pos;
  }
  if(pos != from)
    add(std::move(format.substr(from, pos-from)), false);
}

template <class M>
writer<M> writer<M>::getnth(int i)
{
  throw "out of range on pintnth";
}


template <class M>
template <typename ... Types> const writer<M>& writer<M>::getnth(int i, const writer<M>& a, const Types&... params)
{
  if(i==1)
    return a;
  else
    return getnth(i-1, params...); //well if the reference arrived valid here, it will remain valid after return
}

template <class M>
void writer<M>::printnth(int i)
{
  throw "out of range on pintnth";
}

template <class M>
template <typename ... Types> void writer<M>::printnth(int i, const std::string& str, const Types&... params)
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

template <class M>
template <typename ... Types> void writer<M>::printnth(int i, const int& num, const Types&... params)
{
  if(i==1)
  {
    printnth(i, std::to_string(num));
  }
  else
    return printnth(i-1, params...);
}

template <class M>
template <typename ... Types> void writer<M>::printnth(int i, const writer& wrt, const Types&... params)
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

template <class M>
void writer<M>::write_indent(std::ostream& ss, const std::string& str, int indent, int nobreak)
{
  static int mynobreak = -1;

  if(mynobreak <= 0)
    for(int i = 0; i < 2 * indent; ++i)
      ss << ' ';

  if(str.find("$$") != std::string::npos)
  {
    std::string out = str;
    int pos = out.find("$$");
    while(pos != std::string::npos)
    {
      out = out.replace(pos, 2, "$");
      pos = out.find("$$");
    }
    ss << out;
  }
  else
    ss << str;

  mynobreak--;
  if(nobreak > 0)
    mynobreak = nobreak;

  if(mynobreak <= 0)
    ss << std::endl;
}

template <class M>
writer<M>::writer() : data(), last_terminated(true)
{
}

template <class M>
writer<M>::writer(const std::initializer_list<std::string>& init) : data(init), last_terminated(true)/*the strings are taken literary - id est not formatted*/
{
}

template <class M>
std::string writer<M>::get_string() const
{
  std::string str = "";
  for(auto line : data)
    str.append(line);
  return str;
}

template <class M>
template<bool dolars, typename ... Types> writer<M>& writer<M>::print(const writer& format, const Types&... params)
{
  if(&format != this)
    for(auto line : format.data)
      print<dolars>(line, params...);
  else
    print<dolars>(format.get_string(), params...);

  return *this;
}

template <class M>
writer<M> writer<M>::to_writer(const int& r)
{
  return to_writer(std::to_string(r));
}

template <class M>
writer<M> writer<M>::to_writer(const std::string& r)
{
  return writer<M>({r}); //be careful about using print - we dont want to evaluate '$$2' acidentaly
}

template <class M>
const writer<M>& writer<M>::to_writer(const writer<M>& r)
{
  return r;
}

template <class M>
template<bool dolars, typename ... Types> writer<M>& writer<M>::print_internal(const std::string& format, const Types&... params)  //print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j
{
  int from = 0;
  while(from < format.length())
  {
    if(from + 1 < format.length() && format[from] == '$' && format[from+1] == '$')
    {
      if(dolars)
        add("$", false);
      else
        add("$$", false);
      from += 2;
    }
    else if(from + 1 < format.length() && format[from] == '$')
    {
      std::string a = get_name(format, from);
      if(a.empty())
        throw std::string("bad '$' in: ").append(format);
      else if( -1 == a.find_first_not_of("0123456789"))
        print<dolars>(getnth(std::stoi(a), params...), params...);
      else
        print<dolars>(M::alias(a), params...);
    }
    else
    {
      process(from, format);
    }
  }
  return *this;
}

template <class M>
template<bool dolars, typename ... Types> writer<M>& writer<M>::print(const std::string& format, const Types&... params)  //print("a[$1] = a[$1] $ $2", i, j) -> "a[i] = a[i] $ j
{
  print_internal<dolars>(format, to_writer(params)...); //the purpose of this circus is not forcing (constant) writers to make several copies during a single call
  return *this;
}

template <class M>
template<bool dolars, typename ... Types> writer<M>& writer<M>::pushf(const std::string& filename, const Types&... params)
{
  last_terminated = true;
  return printf<dolars>(filename, params...);
}

template <class M>
template<bool dolars , typename ... Types, typename Type> writer<M>& writer<M>::push(const Type& format, const Types&... params)
{
  trim();
  last_terminated = true;
  return print<dolars>(format, params...);
}

template <class M>
template<bool dolars, typename ... Types> writer<M>& writer<M>::printf(const std::string& filename, const Types&... params)
{
  return print<dolars>(from_file(filename), params...);
}

template <class M>
template <bool dolars , typename Types> writer<M>& writer<M>::append(Types str)
{
  print<dolars>("$1", str);
  return *this;
}

template <class M>
std::string writer<M>::from_file(const std::string& filename)
{
  std::ifstream ifs(filename);
  std::string file( (std::istreambuf_iterator<char>(ifs) ), (std::istreambuf_iterator<char>() ) );
  return file;
}

template <class M>
writer<M> &writer<M>::list_concat(const std::string& delim)
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

template <class M>
void writer<M>::write(std::ostream& ss) 
{
  if(data.empty())
    return;
  trim();
  int indent = 0;
  int nobreak = 0;
  for(const auto& line : data)
  {
    if(line.find("for") == 0 && line.length() > 3 && (line[3] == ' ' || line[3] == '('))
      nobreak = 2;
    switch(line[line.length()-1])
    {
    case '}':
      --indent;
      write_indent(ss, line, indent, nobreak);
      break;
    case '{':
      write_indent(ss, line, indent, nobreak );
      ++indent;
      break;
    case ':':
      write_indent(ss, line, indent-1, nobreak );
      break;
    default:
      write_indent(ss, line, indent, nobreak );
      break;
    }
    nobreak = 0;
  }
}

template <class M>
void writer<M>::write_file(std::string filename)  
{
  std::ofstream f(filename);
  write(f);
  f.close();
}

template <class M>
std::string writer<M>::write_str()
{
  std::stringstream os;
  write(os);
  return os.str();
}

template <class M>
void writer<M>::write_std()  
{
  write(std::cout);
}

template <class M>
void writer<M>::clear()  
{
  data.clear();
  last_terminated = true;
}

template <class M>
bool writer<M>::operator==(const writer& w)
{
  return w.data == data;
}

template <class M>
bool writer<M>::operator!=(const writer& w)
{
  return w.data != data;
}

template <class M>
void writer<M>::self_test()
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
}

#endif

