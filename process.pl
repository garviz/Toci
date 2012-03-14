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

my %count = ();

while(<FH>) {
    chomp;
    my @fields = split;
    my $src=`grep D- "$ARGV[1]"|grep -w "$fields[1]"|cut -d" " -f1|tr -d "D-"`;
    chomp $src;
    my $dst=`grep D- "$ARGV[1]"|grep -w "$fields[3]"|cut -d" " -f1|tr -d "D-"`;
    chomp $dst;
    for(my $i=0; $i<$fields[0]; $i++) {
        print "$src-->$dst\n";
    }
#   $count{"$src-->$dst"}+=$fields[0];
}
close FH;
