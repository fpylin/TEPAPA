#!/usr/bin/perl

use strict;
use warnings;

use Lingua::Stem;

my @lines = <STDIN>;

my $sentence_sep = '([\.,:;])(\s+|$)';

sub stem_word {
    my $p = shift;
    my @words = ($p);

    my $stemmed_words = Lingua::Stem::En::stem({ -words => \@words, -locale => 'en'});

    for my $w (@{ $stemmed_words }) {
	if ( (length($w)==0) ) {
	    return $p;
	}
	else {
	    return "<STEM:$w>";
	}
    }
}

sub process_part {
	my $x = shift;
	
	# inserting a space around punctuations
	$x =~ s/(\.)\s+([A-Z])/ $1 <SEP\/> $2/g;
	$x =~ s/([\(\[:;\]\)])/ $1 /g;
	$x =~ s/([\d]+)-([\d]+)/$1 <RANGE:TO\/> $2/g;
	$x =~ s/–/ -- /g;
	
	# inserting a space between units, dimension, and range
	$x =~ s/([\+\-]?[\d]+(?:\.[\d]+)?)([kmcu]?[mlg]|%)\b/$1 $2/g;
	$x =~ s/([\d]+)x([\d]+)x([\d]+)\b/$1 x $2 x $3/g;
	$x =~ s/([\d]+)x([\d]+)\b/$1 x $2/g;
	$x =~ s/\b(>|<|>=|<=)([0-9])\b/$1 $2/g;
	$x =~ s/([0-9])%/$1 %/g;
	
	$x =~ s/([\("])(\S+)/$1 $2/g;
	$x =~ s/(\S+)([\)"])/$1 $2/g;
	# $x =~ s/([\.?,])\^+/$1/g;
	$x =~ s/\^+(\s+|$)/$1/g;
	$x =~ s/(\S+),/$1 ,/g;
	
	$x =~ s/$sentence_sep/ $1 $2 /g;
	
	# remove comma within numbers
	$x =~ s[\$?[\+\-]\d+(,\d+)+] {
		my $a = $&;
		$a =~ s/,//g;
		qq[$a];
		}egs;
	return $x ;
	}

my %numeric_words = (
	'zero' => 0,
	'one' => 1, 'two' => 2, 'three' => 3, 'four' => 4, 'five' => 5,
	'six' => 6, 'seven' => 7, 'eight' => 8, 'nine' => 9, 'ten' => 10,
	);
	
my $month_regex = '(?i:Jan(?:uary)?|Feb(?:ruary)?|Mar(?:ch)|Apr(?:il)|May|June?|July?|Aug(?:ust)|Sep(?:tember)?|Oct(?:ober)?|Nov(?:ember)?|Dec(?:ember)?)';

sub annotate_token($) {
	my $token = $_[0];
	my @annotations;

	for ($token) {
		if ( /^(.*)_([A-Z]+)$/ ) { # pos tagging 
			my ($token1, $pos) = ($1, $2);
			push @annotations, $token1;
			push @annotations, "<POS:$pos>"; 
			$token = $token1;
			}
		else {
			push @annotations, $token;
			}
		
		/^[[:punct:]]+$/ 
			and do { push @annotations, "<PUNCT/>"; };
		/^[\+\-]?[0-9]+(?:\.[0-9]+)?$/ 
			and do { push @annotations, "<NUMBER/>"; };
		/^[0-9]+(?:st|nd|rd|th)$/ 
			and do { push @annotations, "<ORDINAL/>"; };
		/^$month_regex$/ 
			and do { push @annotations, "<MONTH/>"; };
		/^[0-9]{4}$/ and ($token >= 1700) and ( $token <= 2050 ) 
			and do { push @annotations, "<YEAR/>"; };
		exists( $numeric_words{$token} ) 
			and do do { push @annotations, $numeric_words{$token}, "<NUMBER/>"; };
		/^[\+\-]?[0-9]+(?:\.[0-9]+)?%$/ 
			and do { push @annotations, "<PERCENTAGE/>"};
# 		/^[A-Za-z]+$/ and do { push @annotations, "<WORD>"; };
		/^[A-Za-z]+$/ and do { push @annotations, stem_word($token); };
		}

	return @annotations;
	}

sub deannotate_block_annotations {
	my @annotations ;
	my $sb = $_[0];
# 	print STDERR "\e[1;31m$sb\e[0m\n";
	if ( $sb =~ /^<([A-Za-z0-9_]+(?::.+?)?)>/ ) {
		my $tag_full = $1;
		my $after = $';
		my $tag = $tag_full;#  $tag =~ s/:.*//;
		
# 		print STDERR "\e[1;32m$tag\t$after\e[0m\n";
		if ( $after =~ /<\/$tag>$/ ) {
			my $text = $`;
# 			print STDERR "\e[1;33m$tag\t$text\e[0m\n";
			my @subannotations = deannotate_block_annotations($text); 
			if ( @subannotations ) {
				@annotations = (@subannotations, "<$tag_full/>");
				}
			else {
				@annotations = ($text, "<$tag_full/>");
				}
			}
		else {
			warn "Annotation \"$sb\" not well-formed\n";
			}
		}
	return @annotations ;
	}
my $output = '';
my $token_sep_regex = '(?:\s+)';
my $annot_regex = '<(?:[A-Za-z0-9_]+(?::[^<>]+?)?)/>';

for my $line (@lines) {
	my @blocks = split /($annot_regex)/, $line ;
	for my $b (@blocks) {
# 		print STDERR ">> $b <<\n";
		if ( $b =~ /$annot_regex/ ) {
# 			$output .= "\e[1;36m$b\e[0m\n";
			$output .= "$b\n";
			next;
			}
			
		# Now process block-style annotations:
		my @sub_blocks ;
		
		while ( $b =~ /<([A-Za-z0-9_]+(?::.+?)?)>/sg ) { # opening tag
			my $tag = $1;
			my $closing_tag = '';
			my $opening_tag = $&;
			my $before = $`; 
			my $after = $';
			my $token = '';
# 			die $opening_tag;
		
			if ( $after =~ /<\/\Q$tag\E>/ ) { # closing tag
				$closing_tag = $&;
				$token = $`;
				$b = $';
				push @sub_blocks, $before;
				push @sub_blocks, $opening_tag.$token.$closing_tag;
# 				print STDERR "\e[1;33m$opening_tag$token$closing_tag\e[0m\n";
				}
			else {
# 				die "closing tag </$tag> not found\n";
				}
			}
		push @sub_blocks, $b if length $b;
		
		for my $sb (@sub_blocks) {
# 			print "\e[1;33m$sb\e[0m\n\e[34m---\e[0m\n";
			if ( $sb =~ /^<(?:[A-Za-z0-9_]+(?::.+?)?)>(.+)<\/(?:[A-Za-z0-9_]+(?::.+?)?)>$/ ) {
# 				my $token = $1;
				my $sx = join("\t", deannotate_block_annotations($&) )."\n";
# 				print "\e[1;35m!!!\e[0m\t$sx\n";
				$output .= $sx ;
				next;
				}
			
			my $transformed_str = process_part($sb);
# 			print STDERR "\t>> TRANSFORMED: $transformed_str <<\n";

			my @tokens = split /($token_sep_regex)/, $transformed_str;
			for my $token (@tokens) {
# 				print STDERR "\t>> TOKEN: $token <<\n";
				next if $token =~ /^\s*$/;
# 				$output .= join("\t", $token, annotate_token($token))."\n";
				$output .= join("\t", annotate_token($token))."\n";
# 				$output .= join("\t", $token)."\n";
				}
			}
		}
	$output .= "<CRLF/>\n";
	}

$output =~ s/(<CRLF\/>\n)+/<CRLF\/>\n/sg;

print $output;
