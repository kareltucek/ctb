if ( pos_out_$1 == size_out_$1 )
{
  int mod = 1;
  if(std::string("$3") == "bool")
  {
    mod = 32;
  }

  for(int i = 0; i < size_out_$1/mod; ++i)
  {
    if($4 == 1)
    {
      data_check_$1.push_back(data_out_$1[i]);
      processed_out_$1++;
    }
    else
    {
      assert(data_out_$1[i] == data_check_$1[processed_out_$1]);
      processed_out_$1++;
    }
  }

  if(std::string("$3") == "bool")
  {
    size_out_$1 = (pull()%16)*32; 
  }
  else
  {
    size_out_$1 = pull()%100; 
  }

  if(data_out_$1 != NULL)
  {
    delete [] data_out_$1;
  }
  data_out_$1 = new $2[size_out_$1];
  pos_out_$1 = 0;
}
