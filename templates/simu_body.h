
void test_body$10()
{
  std::cout << "processing width " << $10 << std::endl;
  reset();
  int cycles = 0;
  const int packsize = $10;
  for(int r = 0; r < 1; ++r)
  {
    $4

      for (int i = 0; i < 10 ;i++)
      {
        /*std::cout << "at cycle " << cycles << std::endl*/;
        $5

          std::size_t batch_size;
        batch_size = $6;

        std::size_t j = 0;
        for ( ; j + $10 <= batch_size; j += $10)
        {
          $7
            cycles += $10;
        }
        for (; j < batch_size; ++j)
        {
          $8
            cycles++;
        }

        $9
      }
  }
}

