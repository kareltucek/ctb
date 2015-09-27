/*actual code*/;
std::size_t j = 0;
if(aligned)
{
  for (; j < output_offset && j < batch_size; ++ j)
  {
    $4
  }
  $5
  switch((align_offset + output_offset) % $6)
  {
    case 0:
      {
        for ( ; j + $6 <= batch_size; j += $6)
        {
          $1;
        }
      }
      break;
    $2
  }
}
else
{
  for ( ; j + $6 <= batch_size; j += $6)
  {
    $3
  }
}

for ( ; j < batch_size; ++ j)
{
  $4
}
