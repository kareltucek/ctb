
#ifndef PARSER_GUARD
#define PARSER_GUARD

#include "defines.h"
#include <string>
#include <map>
#include "errorhandling.h"

namespace ctb
{
  class parser
  {
    public:
      enum ttype{tWhite,tMod,tNum,tMul,tDiv,tAdd,tSub,tPB,tPE,tEof};
    private:
      typedef pair<ttype, int> token;
      vector<token> stack;
      ttype type(const token&);
      int val(const token&);
      token tokenize(const char *&);
      const token& get(int i);
      int process(const char*&);
      void squash_mul_div();
      void squash_add_sub();
      static void diagnosis(const string& s, const string& e, int pos);
    public:
      static void self_test();
      static int calculate(const string&);
  };

  void parser::self_test()
  {
    cout << "testing math parser" << endl;
    assert(parser::calculate("1+1") == 2);
    assert(parser::calculate("2+3*4+2") == 16);
    assert(parser::calculate("(2+3)*(4-2)") == 10);
    assert(parser::calculate("22-(2+3)*(4-2)") == 12);
    assert(parser::calculate("(1 + (0/4))% (12/4)") == 1);
  }

  void parser::squash_mul_div()
  {
    while(stack.size() >= 3 && (type(get(1)) == tMul || type(get(1)) == tDiv || type(get(1)) == tMod))
    {
      if(type(get(0)) != tNum || type(get(2)) != tNum)
        error("something wrong found where number expected");
      int a = val(get(0));
      int b = val(get(2));
      ttype t = type(get(1));
      stack.pop_back();
      stack.pop_back();
      stack.pop_back();
      switch(t)
      {
        case tDiv:
          stack.push_back(token(tNum,b/a));
          break;
        case tMul:
          stack.push_back(token(tNum,b*a));
          break;
        case tMod:
          stack.push_back(token(tNum,b%a));
          break;
        default:
          error("this should have never happened");
      }
    }
  }

  void parser::squash_add_sub()
  {
    while(stack.size() >= 3 && (type(get(1)) == tAdd || type(get(1)) == tSub))
    {
      if(type(get(0)) != tNum || type(get(2)) != tNum)
        error("something wrong found where number expected (note that we dont support unary minus!)");
      int a = val(get(0));
      int b = val(get(2));
      ttype t = type(get(1));
      stack.pop_back();
      stack.pop_back();
      stack.pop_back();
      switch(t)
      {
        case tSub:
          stack.push_back(token(tNum,b-a));
          break;
        case tAdd:
          stack.push_back(token(tNum,a+b));
          break;
        default:
          error("this should have never happened");
      }
    }
  }

  void parser::diagnosis(const string& s, const string& e, int pos)
  {
    error(string("at position ").append(to_string(pos)).append(" in expression ").append(s).append("\n    ").append(e), false);
  }

  int parser::calculate(const string& s)
  {
    parser p;
    const char* exp = s.c_str();
    const char* start = s.c_str();
    int r = 0;
    try
    {
      r = p.process(exp);
    }
    catch(const error_struct& err)
    {
      diagnosis(s, err.first, exp-start);
    }
    catch(exception& err)
    {
      diagnosis(s, err.what(), exp-start);
    }
    return r;
  }

  const parser::token& parser::get(int i)
  {
    return stack[stack.size()-1-i];
  }

  int parser::val(const parser::token& t)
  {
    return t.second;
  }

  parser::ttype parser::type(const token& t)
  {
    return t.first;
  }

  parser::token parser::tokenize(const char *& ptr)
  {
    static map<char, ttype> hash = {
      {'(',tPB},
      {')',tPE},
      {'*',tMul},
      {'/',tDiv},
      {'+',tAdd},
      {'-',tSub},
      {'%',tMod},
      {'0',tNum},
      {'1',tNum},
      {'2',tNum},
      {'3',tNum},
      {'4',tNum},
      {'5',tNum},
      {'6',tNum},
      {'7',tNum},
      {'8',tNum},
      {'9',tNum},
      {'\0',tEof},
      {' ',tWhite},
    };
    if(hash.find(*ptr) == hash.end())
      error("unexpected character, cannot tokenize");
    ttype t = hash.find(*ptr)->second;
    if(t == tNum)
    {
      int i = 0;
      while(isdigit(*ptr))
        i = i*10+((*(ptr++))-48);
      return token(t,i);
    }
    else
    {
      ptr++;
      return token(t,0);
    }
  }



  int parser::process(const char*& exp)
  {
    for(token t = tokenize(exp); true; t = tokenize(exp))
    {
      switch(type(t))
      {
        case tWhite:
          break;
        case tNum:
          {
            stack.push_back(t);  
            squash_mul_div();
          }
          break;
        case tPE:
          {
            squash_add_sub();
            if(stack.size() < 2)
              error("misplaced parenthesis");
            if(type(get(1)) != tPB)
              error("misplaced parentheses");
            token num = get(0);
            stack.pop_back();
            stack.pop_back();
            stack.push_back(num);
          }
          break;
        case tPB:
          stack.push_back(t);
          break;
        case tMul:
        case tDiv:
        case tMod:
          stack.push_back(t);
          break;
        case tAdd:
        case tSub:
          squash_add_sub();
          stack.push_back(t);
          break;
        case tEof:
          squash_mul_div();
          squash_add_sub();
          if(stack.size() != 1)
            error(string(to_string(stack.size())).append(" tokens remained on stack"));
          if(type(get(0)) != tNum)
            error(string("wrong token type remained on stack "));
          return val(get(0));
      }
    }
    error("this place should never have been reached!");
    return 0;
  }
};

#endif
