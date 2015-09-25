if ( pos_out_$1 == size_out_$1 )
{
  /*would be nices as a macro*/
  cycles -= size_out_$1; 
  if(std::string("$3") == "bool")
  {
    for(int i = 0; i < size_out_$1; ++i)
    {
      if($4 == 1)
      {
        data_check_$1.push_back(load_bool(data_out_$1,i));
        processed_out_$1++;
      }
      else
      {
        if(load_bool(data_out_$1,i) != data_check_$1[processed_out_$1])
        {
          DEBUG_STD((int)load_bool(data_out_$1,i), "$5 OUTPUT_$1");
          DEBUG_STD((int)data_check_$1[processed_out_$1], "$5 OUTPUT_CHECK_$1");
          std::cout << "failed in cycle " << cycles << std::endl;
          throw (int)cycles;
        };
        processed_out_$1++;
      }
      cycles++;
    }
  }
  else
  {
  for(int i = 0; i < size_out_$1; ++i)
    {
      if($4 == 1)
      {
        data_check_$1.push_back(data_out_$1[i]);
        processed_out_$1++;
      }
      else
      {
        if(data_out_$1[i] != data_check_$1[processed_out_$1])
        {
          if(std::string("$3") == "ssebool")
          {
            DEBUG_STD((int)data_out_$1[i], "$5 OUTPUT_$1");
            DEBUG_STD((int)data_check_$1[processed_out_$1], "$5 OUTPUT_CHECK_$1");
          }
          else
          {
            DEBUG_STD(data_out_$1[i], "$5 OUTPUT_$1");
            DEBUG_STD(data_check_$1[processed_out_$1], "$5 OUTPUT_CHECK_$1");
          }
          std::cout << "failed in cycle " << cycles << std::endl;
          throw (int)cycles;
        };
        processed_out_$1++;
      }
      cycles++;
    }
  }

  if(std::string("$3") == "bool")
  {
    size_out_$1 = (10+pull()%16)*32; 
  }
  else
  {
    size_out_$1 = 50+pull()%100; 
  }

  if(data_out_$1 != NULL)
  {
    delete [] data_out_$1;
  }
  data_out_$1 = new $2[size_out_$1];
  pos_out_$1 = 0;
}
