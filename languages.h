#ifndef LANGUAGEEMPTY_GUARD
#define LANGUAGEEMPTY_GUARD

namespace ctb
{
  class language_empty
  {
    public:
      static void shouldbreak(const int& pos, const std::string& format, bool& brbf, bool& braf)
      { 
        brbf=false;
        braf=false;
        switch(format[pos])
        {
          case '\n':
          case '\r':
            braf = true;
            break;
        }
      };
      static void shouldindent( const std::string&, int& outindent, int& indent, int& nobreak)
      {  
        outindent=0; 
        nobreak=0;
      };
  } ;

  class language_cpp
  {
    public:
      static void shouldbreak(const int& pos, const std::string& format, bool& brbf, bool& braf)
      { 
        brbf = false;
        braf = false;
        switch(format[pos])
        {
          case ':':
            if((pos > 0 && format[pos-1] == ':') || ( format.size() > pos && format[pos+1] == ':'))
              break;
          case ';':
          case '\n':
          case '\r':
          case '?':
            braf = true;
            break;
          case '}':
          case '{':
            braf = true;
            brbf = true;
            break;
        }
      };

      static void shouldindent( const std::string& line, int& outindent, int& indent, int& nobreak)
      {  
        outindent = indent;
        nobreak = 0;

        if(line.find("for") == 0 && line.length() > 3 && (line[3] == ' ' || line[3] == '('))
          nobreak = 2;
        switch(line[line.length()-1])
        {
          case '}':
            --indent;
            break;
          case '{':
            ++indent;
            break;
          case ':':
            outindent = indent-1;
            break;
          default:
            outindent = indent;
            break;
        }
      };
  } ;

}

#endif 
