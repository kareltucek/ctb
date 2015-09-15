void f(...)
{
  if(A)
  {
    o = a+b;
  }
  else
  {
    o = b+c;
  }
}

void r(...)
{
  {
base_pull:
base:
    if(left_full)
      goto left;
    if(right_full)
      goto right;
    if(no_more_data)
      goto single;
    ra = LD(a);
    rb = LD(b);
    rc = LD(c);
    SPLIT_ST_TRUE(rla, ra, f);
    SPLIT_ST_TRUE(rlb, rb, f);
    SPLIT_ST_FALSE(rrb, rb, f);
    SPLIT_ST_FALSE(rrc, rc, f);
    TMP_ST(f);
    goto base;
  }
  {
left_pull:
    if(jo_full)
      goto join;
    if(!left_full)
      goto base_pull;
    ...
    process 1 by 1;
    ...
    goto left_pull;
left:
    if(jo_full)
      goto join;
    if(!left_full)
      goto base;
    sa = SPLIT_LD(rla);
    sb = SPLIT_LD(rlb);
    so = ADD(sa,sb);
    JOIN_ST(jlo, so);
    goto left;
  }
  {
right_pull:
    ...
      the same as for left pull;
    ...
right:
    ...
      the same as for left;
    ...
  }
  {
join:
    if(jlo_overfilled)
      goto right_pull;
    if(jro_overfilled)
      goto left_pull;
    if(!jlo_full)
      goto left;
    if(!jro_full)
      goto right;
    tf = TMP_LD(f);
    to = JOIN_LD(jo);
    ST(to);
  }
  {
single:
    ...
      code that examines all the registers and pushes them through 1 by 1;
    ...
  }


}

implementation:

SPLIT_ST_TRUE(to, from, control)
{
  global __m128 B1_to;
  global __m128 B2_to;
  global __m128 B3_to;
  global int fill_mask;
  local int8 mask = _mm_movemask(control);
shift_to_0_at_0:
  if(mask & 0x1)
  {
    mask == mask << c;
    if(0x80 & _mm_movemask(B1_to)) //need to decide before shifting
    {
      B1_to = _mm_or(_mm_s_li(B1_to), _mm_shuffle(_mm_setzero(),from)); /*shuffle also implicitly shifts*/
      fillmask |= full; /*at every *to_0*/
      fillmask |= OVERFILL; /*at every *to_1*/
      goto shift_to_1_at_1;
    }
    {
      B1_to = _mm_or(_mm_s_li(B1_to), _mm_shuffle(_mm_setzero(),from)); /*shuffle also implicitly shifts*/
    }
    goto end; /*at every *at_3 */
  }
shift_to_0_at_1:SIMILAR;
shift_to_0_at_2:SIMILAR;
shift_to_0_at_3:SIMILAR;
shift_to_1_at_0:SIMILAR;
shift_to_1_at_1:SIMILAR;
shift_to_1_at_2:SIMILAR;
shift_to_1_at_3:SIMILAR;
shift_to_2_at_0:SIMILAR;
shift_to_2_at_1:SIMILAR;
shift_to_2_at_2:SIMILAR;
shift_to_2_at_3:SIMILAR;
end:;
}

SPLIT_LD(to, from) //!!!n^2 ->branching with round robin?
{
  to = B1_from;
  B1_from = B2_from;
  B2_from = B3_from;
  B3_from = _mm_insert(_mm_setzero(),0xFF,C);
  fill_mask = fill_mask << c;
}

JOIN_ST(to, from)
{
  /* round robin store */
}

JOIN_LD(from)
{
  /* overfill checks */
  /* round robin load */
}
