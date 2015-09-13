/* Purpose of this class is providing an object which generates pseudorandom data which are reproducible.
 *
 * Unfortunately neither the old C++ nor c++11 provides such implementation.*/

class random_engine
{
  private:
    uint32_t seed;
  public:
    void reset() {seed = 5323;};
    uint32_t get()
    {
      //taken from http://stackoverflow.com/questions/15500621/c-c-algorithm-to-produce-same-pseudo-random-number-sequences-from-same-seed-on
      seed = (8253729 * seed + 2396403); 
      return seed;
    };
};

