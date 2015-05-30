#!/usr/bin/perl
our @files = @ARGV;
while ( @files > 0 )
{
  $file = pop (@files);
  open FILE, $file;
  open FILEW, '>', default;
  while(<FILE>)
  {
    if(m=//#file ([^ \n\t\r]*)=)
    {
      close FILEW;
      open FILEW, '>', $1;
    }
  else
    {
      s=//#==;
      print FILEW $_;
    }
  }
  close FILEW;
  close FILE;
}

