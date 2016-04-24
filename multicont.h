#ifndef imp_cont_GUARD
#define imp_cont_GUARD

#include "defines.h"
#include <vector>
#include <assert.h>
#include "errorhandling.h"

/* imp_cont is a class which mimics its template parameter, but provides an additional method which allows access to a vector of 'nondefault' instances of the template parameter. 
 *
 * E.g. this allows as to add multiple classes of edges between graph vertices without having to change anything except one container type.
 * */

template <class T>
class imp_cont : public T
{
  private:
    mutable vector<T> vec;
    void fillin(int i) const
    {
      while(vec.size() < i)
        vec.push_back(T());
    };
  public:
    using T::T;

    T& getlevel(int i = 0)
    { 
      fillin(i);
      return i == 0 ? *this : vec[i-1];
    };

    const T& getlevel(int i = 0) const 
    { 
      fillin(i);
      return ((i == 0) ? (*((T*)this)) : (vec[i-1]));
    };

    int getlevelcount()
    {
      return vec.size()+1;
    };

    static void self_test()
    {
      cout << "testing multi container" << endl;
      imp_cont<vector<int>> a;
      a.push_back(0);
      a.push_back(1);
      a.getlevel(1).push_back(2);
      assert(a[0] == 0);
      assert(a[1] == 1);
      assert(a.getlevel(1)[0] == 2);
    }
};

template <class T>
class imp_contA : public imp_cont<T>
{
  public:
    using imp_cont<T>::imp_cont;

    T& operator[](int i)
    {
      return this->getlevel(i);
    };
};

template <class T>
class imp_contB : public T
{
  private:
    mutable map<string, T> vec;
  public:
    using T::T;

    T& operator[](const string& k)
    {
      if(k == "default")
        return *this;
      else
        return vec[k];
    };

    T const & operator[](const string& k) const
    {
      if(k == "default")
        return *this;
      else
      {
        auto itr = vec.find(k);
        if(itr == vec.end())
          throw(string("key not found: ") + k);
        return
          *itr;
      }
    };
};

typedef imp_cont<vector<int>> imp_cont_default;

#endif
