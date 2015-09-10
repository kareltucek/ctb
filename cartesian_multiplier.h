#ifndef CARTESIAN_GUARD
#define CARTESIAN_GUARD

#include <vector>

namespace ctb
{
  /*
   * This class serves for generating cartesian product of arbitrary number of sets of the same type. I am writting this explicitly since it is not the first time I need this feature.
   *
   * The cartesian multiplier is basically an iterator which dereferences to a list of iterators. Respectively it is an iterator which can be used directly to obtain iterators of iterators of the sets over which we are constructing the cartesian product.h
   * */

  template <typename T>
   class cartesian_multiplier : std::forward_iterator_tag
  {
    private:
      bool end_b;
      typedef std::vector<typename T::iterator> itr;
      itr begins;
      itr ends;
      itr middles;
    public:
      typedef cartesian_multiplier<const T> constant;
      typedef itr iterator;

      //lets assume that default constructors and operators will work

      cartesian_multiplier();
cartesian_multiplier& operator++();
      void add(T&);
      void reset();
      iterator& operator*();
      iterator* operator->();
      const iterator& operator*() const;
      const iterator* operator->() const;

      bool operator==(const cartesian_multiplier&)const ;
      bool operator!=(const cartesian_multiplier&)const ;

      cartesian_multiplier begin();
      cartesian_multiplier end();
      cartesian_multiplier::constant begin() const;
      cartesian_multiplier::constant end() const;

      static void self_test();
  };

  typedef cartesian_multiplier<std::vector<int> > cartesian_multiplier_default;

    template <typename T>
  typename cartesian_multiplier<T>::constant cartesian_multiplier<T>::begin() const
  {
    return cartesian_multiplier::constant(*this);
  }

      template <typename T>
    typename cartesian_multiplier<T>::constant cartesian_multiplier<T>::end() const
    {
      return cartesian_multiplier::constant();
    }

    template <typename T>
  cartesian_multiplier<T> cartesian_multiplier<T>::begin()
  {
    return cartesian_multiplier(*this);
  }

      template <typename T>
    cartesian_multiplier<T> cartesian_multiplier<T>::end()
    {
      return cartesian_multiplier();
    }

  template <typename T>
    cartesian_multiplier<T>::cartesian_multiplier() : end_b(true)
    {
    }

    template <typename T>
  void cartesian_multiplier<T>::reset()
  {
      for(int i = middles.size()-1; i >= 0; i--)
        middles[i] = begins[i];
      end_b = false;
  }

    template <typename T>
  void cartesian_multiplier<T>::add(T& t)
  {
    begins.push_back(t.begin()); 
    ends.push_back(t.end()); 
    middles.push_back(t.begin()); 
    end_b = false;
  }

    template <typename T>
    cartesian_multiplier<T>& cartesian_multiplier<T>::operator++()
    {
      for(int i = middles.size()-1; i >= 0; i--)
      {
        ++middles[i];
        if(middles[i] == ends[i])
          middles[i] = begins[i];
        else
          return *this;
      }
      end_b = true;
          return *this;
    }

    template <typename T>
    const typename cartesian_multiplier<T>::iterator* cartesian_multiplier<T>::operator->() const
    {
      return &middles;
    }

    template <typename T>
   const typename cartesian_multiplier<T>::iterator& cartesian_multiplier<T>::operator*() const
    {
      return middles;
    }

    template <typename T>
    typename cartesian_multiplier<T>::iterator* cartesian_multiplier<T>::operator->()
    {
      return &middles;
    }

    template <typename T>
   typename cartesian_multiplier<T>::iterator& cartesian_multiplier<T>::operator*()
    {
      return middles;
    }

      template <typename T>
    bool cartesian_multiplier<T>::operator!=(const cartesian_multiplier& c) const
    {
      return !(this->operator==(c)); 
    }

      template <typename T>
    bool cartesian_multiplier<T>::operator==(const cartesian_multiplier& c) const
    {
      return c.end_b == end_b || (middles == c.middles && c.begins == begins && c.ends == ends); 
    }

        template <typename T>
      void cartesian_multiplier<T>::self_test()
      {
        std::vector<int> a = {1,2,3};
        std::vector<int> b = {3,4,5};
        std::vector<int> c = {1,3,1,4,1,5,2,3,2,4,2,5,3,3,3,4,3,5};
        std::vector<int> d;
        cartesian_multiplier<std::vector<int> > itr;
        itr.add(a);
        itr.add(b);

        for( auto r : itr)
        {
          for( auto s : r)
          {
            d.push_back(*s);
          }
        }

        assert(c==d);

      }
  
};

#endif
