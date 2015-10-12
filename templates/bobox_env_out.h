/*envelope out $1*/;
if ( pos_out_$1 == size_out_$1 )
{
  if ( pos_out_$1 )
  {
    env_out_$1->set_size( pos_out_$1);
    send_envelope( outputs::output_list_$1(), env_out_$1);
    wait();
  }

  env_out_$1 = allocate(outputs::output_list_$1());
  env_out_$1->cut_off((((std::size_t)env_out_$1->get_column(col_out_$1).get_data<$2>()) % 16 ) / sizeof($2) );
  size_out_$1 = env_out_$1->get_capacity();
  pos_out_$1 = 0;
  data_out_$1 = env_out_$1->get_column( col_out_$1).get_data< $2>();
}

