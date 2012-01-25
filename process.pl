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

my $start=140085083372389-13469;

while(<FH>) {
    chomp;
    print $_-$start . "\n";
}

close FH;
