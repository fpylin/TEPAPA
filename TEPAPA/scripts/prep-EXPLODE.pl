#!/usr/bin/perl

use strict;
use warnings;

use Lingua::Stem;

my @lines = <STDIN>;

my $sentence_sep = '([\.,:;])(\s+|$)';


sub deannotate_block_annotations {
	my @annotations ;
	my $sb = $_[0];
# 	print STDERR "\e[1;31m$sb\e[0m\n";
	if ( $sb =~ /^<(\S+?(?::.+?)?)>/ ) {
		my $tag_full = $1;
		my $after = $';
		my $tag = $tag_full; $tag =~ s/:.*//;
		
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
my $annot_regex = '<(?:\S+?|\S+?:.+?)/>';

sub annotate_token {
	my @annotations;
	my $x = $_[0];
	for ($x) {
		/^[A-Za-z]$/ and do { push @annotations, "<ALPHA/>"; };
		/^[A-Z]$/ and do { push @annotations, "<UPPER/>"; };
		/^[a-z]$/ and do { push @annotations, "<LOWER/>"; };
		/^[[:punct:]]$/ and do { push @annotations, "<PUNCT/>"; };
		/^\s$/ and do { push @annotations, "<SPACE/>"; };
		/^\d$/ and do { push @annotations, "<DIGIT/>"; };
		}
	return @annotations;
	};

for my $line (@lines) {
	my @blocks = split /($annot_regex)/, $line ;
	for my $b (@blocks) {
# 		print STDERR ">> $b <<\n";
		if ( $b =~ /$annot_regex/ ) {
			$output .= $b."\n";
			next;
			}
			
		# Now process block-style annotations:
		my @sub_blocks ;
		
		while ( $b =~ /<(\S+?)(?::.+?)?>/sg ) { # opening tag
			my $tag = $1;
			my $closing_tag = '';
			my $opening_tag = $&;
			my $before = $`; 
			my $after = $';
			my $token = '';
# 			die $tag;
			if ( $after =~ /<\/\Q$tag\E>/ ) { # closing tag
				$closing_tag = $&;
				$token = $`;
				$b = $';
				push @sub_blocks, $before;
				push @sub_blocks, $opening_tag.$token.$closing_tag;
# 				print STDERR "\e[1;33m$opening_tag$token$closing_tag\e[0m\n";
				}
			}
		push @sub_blocks, $b if length $b;
		
		for my $sb (@sub_blocks) {
			if ( $sb =~ /^<.+?>(.+)<\/.+?>$/ ) {
				my $token = $1;
				$output .= join("\t", deannotate_block_annotations($&) )."\n";
				next;
				}
			
			my @tokens = split //, $sb;
			for my $token (@tokens) {
				if ( $token =~ /[\s]/ ) {
					$output .= join("\t", annotate_token($token))."\n";
					}
				else {
					$output .= join("\t", $token, annotate_token($token))."\n";
					}
				}
			}
		}
	$output .= "<CRLF/>\n";
	}

$output =~ s/(<CRLF\/>\n)+/<CRLF\/>\n/sg;

print $output;
