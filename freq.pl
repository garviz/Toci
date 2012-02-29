#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  freq.pl
#
#        USAGE:  ./freq.pl  
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
#      CREATED:  10/02/12 20:29:05
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;
use Bio::SeqIO;
use Math::Combinatorics;
use Algorithm::Combinatorics "variations_with_repetition";

my @n = qw(a c g t);
#my $seqio = Bio::SeqIO->new(-file => $ARGV[0], '-format' => 'Fasta');
#my $combinat = Math::Combinatorics->new(count => 8,data => \@n,);
my @permutaciones;

#open FH, ">out.dat" or die "Can't create output\n";
#while(my $seq = $seqio->next_seq) {
#    my $string = $seq->seq;
#    for (my $i=0; $i<length $string; $i++) {
#        print substr($string,$i,2)."\n";
#    }
#    while(my @combo = $combinat->next_permutation){
#        print join(' ',@combo)."\n";
#    }
    @permutaciones = variations_with_repetition(\@n,$ARGV[0]);
    for my $c (@permutaciones) {
        print @$c, "\n";
    }
#}
#foreach my $left (@permutaciones) {
#    foreach my $middle(@permutaciones) {
#        foreach my $right(@permutaciones) {
#            my $value="@$left@$middle@$right";
#            $value =~ s/ //g;
#           print FH "$value\n";
#        }
#    }
#}
#close FH;
