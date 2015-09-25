if(aligned)
{
  for ( std::size_t j = 0; j < output_offset && j < batch_size; ++ j)
  {
    $4
  }
  $5
  switch(align_offset + output_offset)
  {
    case 0:
      {$1};
      break;
    $2
  }
}
else
{
  $3
}

for ( std::size_t j = 0; j < batch_size; ++ j)
{
  $4
}
