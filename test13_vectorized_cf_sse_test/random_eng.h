/* Purpose of this class is providing an object which generates pseudorandom data which are reproducible.
 *
 * There are doubts about these properties in standard pseudorandom generators, 
 */

class random_engine
{
  private:
    uint32_t seed;
  public:
    void reset(int s = 0) {seed = 5323; for(int i = 0; i < s; i++) get();};
    uint32_t get()
    {
      //math taken from http://stackoverflow.com/questions/15500621/c-c-algorithm-to-produce-same-pseudo-random-number-sequences-from-same-seed-on
      seed = (8253729 * seed + 2396403); 
      return seed;
    };
};

