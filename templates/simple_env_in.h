if ( pos_in_$1 == size_in_$1 )
{
  size_in_$1 = rand()%100; 
  if(data_in_$1 != NULL)
    delete [] data_in_$1;
  data_in_$1 = new $2[size_in_$1];
  pos_in_$1 = 0;
  for(int i = 0; i < size_in_$1; ++i)
  {
    data_in_$1[i] = processed_in_$1;
    processed_in_$1 += $1;
  }
}

