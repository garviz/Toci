#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  process.pl
#
#        USAGE:  ./process.pl  
#
#  DESCRIPTION:  
#
#      OPTIONS:  ---
# REQUIREMENTS:  ---
#         BUGS:  ---
#        NOTES:  ---
#       AUTHOR:  YOUR NAME (), 
#      COMPANY:  
#      VERSION:  1.0
#      CREATED:  18/01/12 14:37:20
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;

open FH, $ARGV[0] or die "Can not open $ARGV[0]";

my @file=<FH>;

close FH;

for (my $i=1;$i<$#file;$i++) {
    next if (abs($file[$i-1]-$file[$i])==1);
    #next if (chomp($file[$i-1])==chomp($file[$i]));
    print $file[$i-1];
    print $file[$i];
}
