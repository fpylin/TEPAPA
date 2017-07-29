#!/usr/bin/perl

my @lines = <STDIN>;
my @newlines ;
my $prevlen = 0;

for (@lines) {
	s/^ *$//g;
# 	print "* $_";
	chomp;
	if ($prevlen == 0) {
		push @newlines, $_;
		}
	else {
		$newlines[$#newlines] .= " ". $_;
		}
	$prevlen = length $_;
	}
	
@lines = @newlines;

for (@lines) {
	s/  +/ /g;
	s/^ +//g;
	s/(\.) ([A-Z])/$1\n$2/g;
	}
	
my $data = join ('', map { "$_\n" } @lines);

print $data;
