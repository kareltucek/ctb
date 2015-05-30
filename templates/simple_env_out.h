if ( pos_out_$1 == size_out_$1 )
{
  for(int i = 0; i < size_out_$1; ++i)
  {
   //std::cout << $1 << " " << data_out_$1[i] << std::endl;
    /*assert(data_out_$1[i] == processed_out_$1);*/
    assert(data_out_$1[i] == 2*processed_out_$1*processed_out_$1/3/3+processed_out_$1);
    processed_out_$1 += bsum;
  }

  size_out_$1 = rand()%100; 
  if(data_out_$1 != NULL)
  delete [] data_out_$1;
  data_out_$1 = new $2[size_out_$1];
  pos_out_$1 = 0;
}
