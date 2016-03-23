#ifndef LANGUAGEEMPTY_GUARD
#define LANGUAGEEMPTY_GUARD

#include "defines.h"

namespace ctb
{
  /** 
   * General description
   * -------------------
   * Languages describe how should writer input and output be autoformatted. 
   *
   * This happens in two phases - first takes place while putting things into writer - that is breaking text into lines. Second phase is outputting phase (stream/file/string/whatever), when the lines are indented.
   *
   * Languages thus provide two callbacks:
   *
   * shouldbreak:
   *  - brbf = break before ('position' in 'format')
   *  - braf = break after  (-||-)
   *
   * shouldindent:
   *  - indent = logical indent - this one is persistent during parsing, so you can record brace structure using these
   *  - outindent = indent to be applied for the line provided - may differ from the logical indent e.g. typically for case which is one level to the left from the indent defined by braces.
   *  - nobreak = if set to nonzero value, the last 'nobreak' newlines will be ignored. May be useful for fixing abnormalities such as semicolons in for cycle. 
   *
   *  TODO: generalize type of persistent structure to be language-side defined
   *
   * */
  class language_empty
  {
    public:
      static void shouldbreak(const int& pos, const string& format, bool& brbf, bool& braf)
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
      static void shouldindent( const string&, int& outindent, int& indent, int& nobreak)
      {  
        outindent=0; 
        nobreak=0;
      };
  } ;

  class language_cpp
  {
    public:
      static void shouldbreak(const int& pos, const string& format, bool& brbf, bool& braf)
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
            braf = true;
            brbf = true;
            break;
          case '{':
            braf = true;
            if(pos == 0 || (pos>0 && format[pos-1] != '$'))
              brbf = true;
            break;
          case '$':
            if(format.size() > pos+1 && format[pos+1] == '{')
              brbf = true;
            break;
        }
      };

      static void shouldindent( const string& line, int& outindent, int& indent, int& nobreak)
      {  
        outindent = indent;
        nobreak = 0;

        if(line.find("for") == 0 && line.length() > 3 && (line[3] == ' ' || line[3] == '('))
          nobreak = 2;
        switch(line[line.length()-1])
        {
          case '}':
            --indent;
            outindent = indent;
            break;
          case '{':
            if(line.length() > 1 && line[line.length()-2] == '$')
              nobreak=3;
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
