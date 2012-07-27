#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  mums.pl
#
#        USAGE:  ./mums.pl  
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
#      CREATED:  07/25/12 23:01:36
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;

open FH, $ARGV[0] or die "Can't open $ARGV[0]\n";

while (<FH>)
{
    if(/^>/)
    {
        print $_;
        next;
    }
    my @fields = split(" ");
    $seen
}
my @mems = {};
my %seen = ();
my @mums = grep { ! $seen{$_} ++ } @mems;

close FH;
