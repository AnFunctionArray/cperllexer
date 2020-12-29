#!/usr/bin/perl

use re 'eval';

use experimental 'switch';

use File::Basename;

$filename = "main.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $mainregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = "./metaregex/regex.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $metaregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $subject = do { local $/; <$fh> };

close $fh;

my $mainregexfinal = "((*F)";

my $entryregex;

my $matchinperl = 0;

$mainregexfilecontent =~/$metaregexfilecontent/;

#(?{parseregexfile($+{filename})})
#(?{entryregexmain($+{entrygroup}, $+{prefix})})

entryregexmain($-{entrygroup}[0], $-{prefix}[0]);

parseregexfile((substr $mainregexfilecontent, length $&), 1);

$mainregexfinal = $mainregexfinal . ")|" . $entryregex;

my $typedefidentifiers = "";

$mainregexfinal =~s/\s|\n//g if(not $matchinperl);

startmatching($subject, $mainregexfinal, basename($ARGV[0], @suffixlist)) if(not $matchinperl);

exit if(not $matchinperl);

#print $mainregexfinal;

$subject =~/$mainregexfinal/xx;

print $&;

sub addtypdefidentifier {
    $typedefidentifiers = $typedefidentifiers . $+{identifierraw} . "|" if(not $mute_callbacks and $+{typedefkeyword});
}

sub getypedefidentifiers {
    return substr $typedefidentifiers, 0, -1;
}

sub printifdefined{
    print $_[0] if($_[1]);
}

sub defaultcallback {
    print "in";
    given ($_[0])
    {
        when (38) {return getypedefidentifiers() if $+{identifierraw}; return "";};

        when (39) {addtypdefidentifier() if $+{identifierraw};};

    	print("comma\n") when 35;
        print("ternary0\n") when 34;
        print("ternary1\n") when 33;
        print("ternary2\n") when 32;

        print("start func decl params") when 47;
        print("end func decl params") when 48;

        printifdefined("$+{assignopraw}\n", $+{assignopraw}) when 30;
        printifdefined("$+{orlogicopraw}\n", $+{orlogicopraw}) when 29;
        printifdefined("$+{andlogicopraw}\n", $+{andlogicopraw}) when 28;
        printifdefined("$+{oropraw}\n", $+{oropraw}) when 27;
        printifdefined("$+{xoropraw}\n", $+{xoropraw}) when 26;
        printifdefined("$+{andopraw}\n", $+{andopraw}) when 25;
        printifdefined("$+{eqopraw}\n", $+{eqopraw}) when 24;
        printifdefined("$+{relopraw}\n", $+{relopraw}) when 23;
        printifdefined("$+{shiftopraw}\n", $+{shiftopraw}) when 22;
        printifdefined("$+{addopraw}\n", $+{addopraw}) when 20;
        printifdefined("$+{mulopraw}\n", $+{mulopraw}) when 21;
        printifdefined("$+{unaryopraw}\n", $+{unaryopraw}) when 19;
        printifdefined("postfix arithmetic:\n$+{postfixarithraw}\n", $+{postfixarithraw}) when 9;
        printifdefined("prefix arithmetic:\n$+{prefixarithraw}\n", $+{prefixarithraw}) when 10;
        printifdefined("$+{escaperaw}\n", $+{escaperaw}) when 1;
        printifdefined("$+{numberliteralraw}\n", $+{numberliteralraw}) when 5;
        printifdefined("$+{textraw}\n", $+{textraw}) when 2;
        printifdefined("$+{identifierraw}\n", $+{identifierraw}) when 6;
        print("start string\n") when 4;
        print("start function call\n") when 7;
        print("end function call\n") when 13;
        printifdefined("member access operator:\n$+{arrowordotraw}\n", $+{arrowordotraw}) when 8;
        print("end sizeof\n") when 14;
        print("begin sizeof\n") when 15;
        printifdefined("$+{qualifiers}\n", $+{qualifiers}) when 11;

        #default {print "bad call $_[0]\n"; exit}
    }
}

sub entryregexmain {
    $entryregex = $_[1] . "(?&" . $_[0] . ")";
}

sub parseregexfile {

    if(not defined $_[1])
    {
        my $filename = $_[0];
        open my $fh, '<', $filename or die "error opening $filename: $!";

        $regexfilecontent = do { local $/; <$fh> };

        close $fh;

    } else {
        $regexfilecontent = $_[0];
    }

    $regexfilecontent =~s/\(\?\?C(\d++)\)/(??{return defaultcallback($1)})/g if($matchinperl);

    $regexfilecontent =~s/#restrictperlonly//g if($matchinperl);

    $regexfilecontent =~s/#restrictpcre2only//g if(not $matchinperl);

    $regexfilecontent =~s/(\(\?<\w+)#restrictpcre2only>/((*F)/g if($matchinperl);

    $regexfilecontent =~s/(\(\?<\w+)#restrictperlonly>/((*F)/g if(not $matchinperl);

    my $regexfile = $regexfilecontent; 
    
    $regexfile =~s/(?<!<)#restrictoutsidefacet\b//g;

    $regexfile =~s/\(\?<#restrictoutsidefacet>/(/g;

    $regexfile =~s/[(]\?&(\w+?)#nofacet[)]/(?&$1)/g;

    $regexfile =~s/\(\?\?C(\d++)\)/(?C$1)/g if(not $matchinperl);

    $regexfile =~s/\(\?C(\d++)\)/(?{defaultcallback($1)})/g if($matchinperl);

    my $regexfilecontentcopy = $regexfilecontent;

    sub replacefacetgroups {
        $regexfilecontent =~s/\Q$_[0]\E(facet)?>/(/g;
    }

    replacefacetgroups($1, $_[1]) while($regexfilecontentcopy =~/(\(\?<\w+)facet>/g);

    $regexfilecontent =~s/\(\?C(\d++)\)//g;

    $regexfilecontent =~s/\(\?\?C(\d++)\)/(?C$1)/g if(not $matchinperl);

    $regexfilecontent =~s/[(]\?&(\w+?)(facet)?[)]/(?&$1facet)/g;

    $regexfilecontent =~s/[(]\?&(\w+?)#nofacet[)]/(?&$1)/g;

    $regexfilecontent =~s/(\(\?<\w+)>/$1facet>/g;

    $regexfilecontent =~s/(\(\?<\w+)#restrictoutsidefacet>/$1facet>(*F)/g;

    $regexfilecontent =~s/\(\?<#restrictoutsidefacet>/((*F)/g;

    $mainregexfinal = $mainregexfinal . $regexfile . $regexfilecontent;
}