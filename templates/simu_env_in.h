if ( pos_in_$1 == size_in_$1 )
{
  if(std::string("$3") == "bool")
  {
    size_in_$1 = (pull()%16)*32; 
  }
  else
  {
    size_in_$1 = pull()%100; 
  }

  if(data_in_$1 != NULL)
  {
    delete [] data_in_$1;
  }
  data_in_$1 = new $2[size_in_$1];
  pos_in_$1 = 0;
  if(std::string("$3") == "ssebool")
  {
    for(int i = 0; i < size_in_$1; ++i)
      data_in_$1[i] = (pull() % 2) * 0xFF;
  }
  else if(std::string("$3") == "bool")
  {
    for(int i = 0; i < size_in_$1/32; ++i)
      data_in_$1[i] = pull();
  }
  else
  {
    for(int i = 0; i < size_in_$1; ++i)
    {
      data_in_$1[i] = ($2)pull();
      if(data_in_$1[i] == 0) /*division by zero*/
      {
        data_in_$1[i]++; 
      }
    }
  }
}
