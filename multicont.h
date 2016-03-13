#ifndef MULTICONT_GUARD
#define MULTICONT_GUARD

#include "defines.h"
#include <vector>
#include <assert.h>

/* Multicont is a class which mimics its template parameter, but provides an additional method which allows access to a vector of 'nondefault' instances of the template parameter. 
 *
 * E.g. this allows as to add multiple classes of edges between graph vertices without having to change anything except one container type.
 * */

template <class T>
class multicont : public T
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
      multicont<vector<int>> a;
      a.push_back(0);
      a.push_back(1);
      a.getlevel(1).push_back(2);
      assert(a[0] == 0);
      assert(a[1] == 1);
      assert(a.getlevel(1)[0] == 2);
    }
};

template <class T>
class multicontA : public multicont<T>
{
  public:
    using multicont<T>::multicont;

    T& operator[](int i)
    {
      return this->getlevel(i);
    };
};

typedef multicont<vector<int>> multicont_default;

#endif
