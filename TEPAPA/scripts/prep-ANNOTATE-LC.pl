#!/usr/bin/perl

use strict;
use warnings;

my @lines = <STDIN>;

# print @lines;

# print "\n----- ANNOTATE-LC ---------\n";

my $tag_regex = '</?(?:\S+(?:: *\S+)?)/?>';

for (@lines) { 
	my @parts = split /($tag_regex)/, $_;
	my $s = '';
	for (@parts) {
		/$tag_regex/ and do { $s .= $_; next; };
		$s .= lc($_);
		}
	
	print $s;
	}
