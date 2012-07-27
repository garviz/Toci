#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  mems.pl
#
#        USAGE:  ./mems.pl  
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
#      CREATED:  07/25/12 14:26:37
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;

my @chunks=();

if ($#ARGV < 1) {
    print "usage: split1.mum split2.mum ...\n";
    exit;
}

sub chunk
{
    my $file = shift;
    my $offset;
    open FH, $file or die "Can not open $file\n";
    while (<FH>) {
        if (/^>/) {
            my $start = rindex($_,"_");
            my $stop = rindex($_,"-");
            $offset=substr($_,$start+1,$stop-$start-1)-1;
            next;
        }
        my @fields = split(" ");
        push @chunks, {R => $fields[0], Q => $fields[1]+$offset, MEM => $fields[2], Good => 1, Tentative => 1};
    }
    close FH;
}

for (my $i=0; $i<=$#ARGV; $i++) {
        &chunk($ARGV[$i]);
}

my @sorted = sort {$a->{Q} <=> $b->{Q}} @chunks;

my $i; 
my $j;
my $N = @sorted;

for ($i=0; $i < $N-1; $i++)
{
    my $i_diag; 
    my $i_end;
    next if (!$sorted[$i]{Good});
    $i_diag = $sorted[$i]{Q}-$sorted[$i]{R};
    $i_end = $sorted[$i]{Q}+$sorted[$i]{MEM};

    for ($j=$i+1; $j < $N && $sorted[$j]{Q} <= $i_end; $j++)
    {
        my $olap;
        my $j_diag;

        die "ERROR\n" if ($sorted[$i]{Q} <= $sorted[$j]{Q});

        next if (!$sorted[$j]{Good});

        $j_diag = $sorted[$j]{Q}-$sorted[$j]{R};
        if ($i_diag == $j_diag)
        {
            my $j_extent;
            $j_extent = $sorted[$j]{MEM}+$sorted[$j]{Q}-$sorted[$i]{Q};
            if ($j_extent > $sorted[$i]{MEM})
            {
                $sorted[$i]{MEM} = $j_extent;
                $i_end = $sorted[$i]{Q}+$j_extent;
            }
            $sorted[$j]{Good} = 0;
        }
        elsif ($sorted[$i]{R} == $sorted[$j]{R})
        {
            $olap = $sorted[$i]{Q}+$sorted[$i]{MEM}-$sorted[$i]{Q};
            if ($sorted[$i]{MEM}<$sorted[$j]{MEM})
            {
                if ($olap >= $sorted[$i]{MEM}/2)
                {
                    $sorted[$i]{Good} = 0;
                    last;
                }
            }
            elsif ($sorted[$j]{MEM} < $sorted[$i]{MEM})
            {
                if ($olap >= $sorted[$j]{MEM}/2)
                {
                    $sorted[$j]{Good} = 0;
                }
            }
            else
            {
                if ($olap >= $sorted[$i]{MEM}/2)
                {
                    $sorted[$j]{Tentative}=1;
                    if ($sorted[$i]{Tentative})
                    {
                        $sorted[$i]{Good} = 0;
                        last;
                    }
                }
            }
        }
        elsif ($sorted[$i]{Q} == $sorted[$j]{Q})
        {
            $olap = $sorted[$i]{R} + $sorted[$i]{MEM} - $sorted[$j]{R};
            if ($sorted[$i]{MEM} < $sorted[$j]{MEM})
            {
                if ($olap >= $sorted[$i]{MEM}/2)
                {
                    $sorted[$i]{Good} = 0;
                    last;
                }
            }
            elsif ($sorted[$j]{MEM} < $sorted[$i]{MEM})
            {
                if ($olap >= $sorted[$j]{MEM}/2)
                {
                    $sorted[$j]{Good} = 0;
                }
            }
            else
            {
                if ($olap >= $sorted[$i]{MEM}/2)
                {
                    $sorted[$j]{Tentative} = 1;
                    if ($sorted[$i]{Tentative})
                    {
                        $sorted[$i]{Good} = 0;
                        last;
                    }
                }
            }
        }
    }
}

for ($i = $j = 0; $i < $N; $i++)
{
    if ($sorted[$i]{Good})
    {
        if ($i != $j)
        {
            $sorted[$j] = $sorted[$i];
        }
        $j++;
    }
    $N = $j;
}

for ($i = 0; $i < $N; $i++)
{
    $sorted[$i]{Good} = 0;
}

for (my $i=0;$i<=$#chunks;$i++) {
    printf("%8lu  %8lu  %8lu\n" , $chunks[$i]{R}, $chunks[$i]{Q}, $chunks[$i]{MEM});
}

