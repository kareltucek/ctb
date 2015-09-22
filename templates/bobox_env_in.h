if ( pos_in_$1 == size_in_$1 )
{
  env_in_$1.reset();
  env_in_$1 = pop_envelope( inputs::input_list_$1());
  if ( env_in_$1->is_poisoned() )
    break;
  size_in_$1 = env_in_$1->get_size();
  pos_in_$1 = 0;
  data_out_$1 = env_in_$1->get_column( col_in_$1).get_data< int>();
  continue; /* to skip empty envelopes*/;
}

