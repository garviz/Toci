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
    my $src=`grep -w "$fields[0]" "$ARGV[1]"|cut -d" " -f1|tr -d "D-"`;
    chomp $src;
    my $dst=`grep -w "$fields[1]" "$ARGV[1]"|cut -d" " -f1|tr -d "D-"`;
    chomp $dst;
    print "$src-->$dst\n";
#   $count{"$src-->$dst"}+=$fields[0];
}
close FH;

