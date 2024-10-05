#!/usr/bin/perl
use strict;
use warnings;
use File::Find;

die "Usage: $0 <directory>\n" unless @ARGV == 1;

my $directory = $ARGV[0];  # Directory to search

sub process
{
   # print "find: $_\n";
    open my $fh, '<', $_ or die "Cannot open file '$_': $!\n";

    my $file_contents = do { local $/; <$fh> };
    close $fh;

    # my $bytes = length($file_contents);
    # print "$_ has $bytes bytes\n";
    my @words = $file_contents =~ /[a-z][a-z][a-z0-9_-]+/g;
    foreach my $word (@words) 
    {
        print "INSERT INTO v(w) VALUES('$word') ON CONFLICT(w) DO UPDATE SET c=1;\n";
    }

}

sub wanted
{
    process if -f $_;
}


find(\&wanted, $directory);

# CREATE TABLE v(w TEXT PRIMARY KEY, c INT DEFAULT 1);
# INSERT INTO v(w) VALUES('jovial') ON CONFLICT(w) DO UPDATE SET c=1;

