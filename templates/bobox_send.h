if ( pos_out_$1 )
{
  env_out_$1->set_size( pos_out_$1);
  send_envelope( outputs::output_list_$1(), env_out_$1);
}
send_poisoned( outputs::output_list_$1());

