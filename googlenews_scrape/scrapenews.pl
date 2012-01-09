#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use Switch;

# to do: readme, check for mexico in results, print var title in csv, program args

# declaring option arguments with default values
my $numresults = 100;
my $language = "any";
my $startdate = '';
my $enddate = '';

# get options: no need for coded die message because that is default behavior if options are used incorrectly
GetOptions( 'results=i' => \$numresults, 'language=s' => \$language, 'startdate=s' => \$startdate, 'enddate=s' => \$enddate );

if($#ARGV != 1)
{
	usage();
	exit(1);
}

# source must be specified as the first argument, and the output filename (with CSV extension) as the second
my $newssrc = $ARGV[0];
my $csv_outfile = $ARGV[1];

# Global vars
my $srcwords; 
my $htmlsrc = "tempResults.html";

print "Source: $newssrc, Output file: $csv_outfile\n";
print "number of results: $numresults, language: $language, startdate: $startdate, enddate: $enddate\n";

open(TITLE, '>', $csv_outfile);
print TITLE "headline, source, date, excerpt\n";
close(TITLE);

# Usage
sub usage
{
	# source is specified as program argument rather than option, because it is not optional
	print "scrapenews.pl < search_strings_file [ options ] source csv_filename\n";
}


# query_google($searchstring);
# Query google news archives subroutine (function) definition
sub query_google
{
	my ($str) = (@_);
	my ($strfield, $numfield, $startfield, $endfield, $langfield);
	my $srcfield;

	$str =~ s/ /+/g;

	switch ($newssrc)
	{
		case /eu/  { $srcfield = qq{+source:"El"+source:"Universal"}; $srcwords = 2; }
		case /lat/ { $srcfield = qq{+source:"Los"+source:"Angeles"+source:"Times"}; $srcwords = 3; }
		case /nyt/ { $srcfield = qq{+source:"New"+source:"York"+source:"Times"}; $srcwords = 3; }
		else { die "source incorrect. please use eu, lat, or nyt to specify desired source: $!\n"; }
	}

	switch ($language)
	{
		case /^any/     { $langfield = ''; }
		case /^english/ { $langfield = "&lr=lang_en"; }
		case /^spanish/ { $langfield = "&lr=lang_es"; }
		else { die "language incorrect. please use any, english, or spanish to specify desired language: $!\n"; }
	}

	$numfield = "&num=".$numresults;
	$startfield = "&as_user_ldate=".$startdate;
	$endfield = "&as_user_hdate=".$enddate;

	my $address = "http://news.google.com/archivesearch?q=".$str.$srcfield.$numfield."&as_price=p0".$startfield.$endfield.$langfield."&hl=en&sa=N&start=0";
	
	print "Grabbing data from source: $newssrc\n";
	`wget -O $htmlsrc "$address"`;
	return $address;
}


# Parse arguments - subroutine (function) definition
sub parse_result
{
	my ($address, $sub_index) = (@_);
	open(IN, '<', $htmlsrc);
	open(OUT, '>>', $csv_outfile);

	my $count = 0;
	my $page = $sub_index/$numresults + 1;
	print "Parsing and printing results from page $page\n";

	while (<IN>)
	{
		$count++;

		if (/^<\/head><script>function showAdvanced\(\)/)
		{
			print "processing page body...\n";

			while (/<table cellpadding=0 cellspacing=0 border=0 style="margin-left: 1em;"><tr><td><a href=.*?>((\w*?\W*?)+?)<\/a><br><div class=j><font size=-1 color=666666 class=l>.*?((<b>\w+<\/b>\s*){$srcwords}).*?<font size=-1 color=black> - (\w{3} [0-9]{1,2}, [0-9]{4})<\/font><br><\/font><font size=-1>(.*?)<\/font><\/div><table cellpadding=0 cellspacing=0 border=0>/g)
			{
				my ($head, $sourc, $dat, $xrpt) = ($1, $3, $5, $6);
				$head =~ s/&#39;/'/g;
				$head =~ s/<\/*b>//g;
				$head =~ s/&quot;/""/g;
				$sourc =~ s/<\/*b>//g;
				$sourc =~ s/\s$//;
				$dat =~ s/,//;
				$xrpt =~ s/^\s//;
				$xrpt =~ s/&#39;/'/g;
				$xrpt =~ s/<\/*b>//g;
				$xrpt =~ s/&quot;/""/g;
				#print "matches\nheadline: $head\nsource: $sourc\ndate: $dat\nexcerpt: $xrpt\n";
    				write_to_csv($head, $sourc, $dat, $xrpt);
			}

			close(OUT);
			
			if (/<br>Next<\/a><\/table><\/div>/)
                	{
                        	$sub_index += $numresults;
                        	$address =~ s/(&hl=en&sa=N&start=)[0-9]*/$1$sub_index/;
				close(IN);
				sleep 1;
                        	`wget -O $htmlsrc "$address"`;
				parse_result($address, $sub_index); 
                	}
                	else
                	{
                        	print "Last page has been parsed.\n";
				close(IN);
				return;
                	}
		}
		elsif ($count <= 7)
		{
		}
		else
		{
			die "Search turned up page without results: $!\n";
		}
	}
}

# Write the data points to a csv file
sub write_to_csv
{
	my ($headline, $source, $date, $excerpt) = (@_);
	print "Writing to $csv_outfile: $headline, $source, $date, excerpt\n";

	if ($headline =~ /,/ && $excerpt =~ /,/)
	{
		print OUT qq{"$headline", $source, $date, "$excerpt"\n};
	}
	elsif ($headline =~ /,/)
	{
		print OUT qq{"$headline", $source, $date, $excerpt\n};
	}
	elsif ($excerpt =~ /,/)
	{
		print OUT qq{$headline, $source, $date, "$excerpt"\n};
	}
	else 
	{
		print OUT qq{$headline, $source, $date, $excerpt\n};
	}
}

while (<STDIN>)
{	
	my $addr = query_google($_);
	parse_result($addr, 0);
	print "On to next query\n";
	sleep 5;
}
	
print qq{Complete. Last "on to next query" was not valid.\n};


