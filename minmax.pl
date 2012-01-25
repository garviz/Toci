#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  minmax.pl
#
#        USAGE:  ./minmax.pl  
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
#      CREATED:  20/01/12 15:41:10
#     REVISION:  ---
#===============================================================================
use strict;
use warnings;

open FH, $ARGV[0] or die "Can't open file $ARGV[0]";

my @file=<FH>;
close FH;
my $max=$file[0];
my $min=$file[0];

for (my $i=1;$i<$#file;$i++) {
    if ($file[$i]>$max) {
        $max=$file[$i];
    }
    if ($file[$i]<$min) {
        $min=$file[$i];
    }
    chomp $max;
    chomp $min;
    print "$min $max\n";
}
