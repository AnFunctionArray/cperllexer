#!/usr/bin/perl

use re 'eval';

$filename = "main.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $mainregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = "./metaregex/regex.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $metaregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = "maintest.c";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $subject = do { local $/; <$fh> };

close $fh;

my $mainregexfinal = "((*F)";

my $entryregex;

$mainregexfilecontent =~/$metaregexfilecontent/;

parseregexfile((substr $mainregexfilecontent, length $&), 1);

$mainregexfinal = $mainregexfinal . ")|" . $entryregex;

my $typedefidentifiers = "";

$mainregexfinal =~s/\s|\n//g;

startmatching($subject, $mainregexfinal);

#$subject =~/$mainregexfinal/xx;

#print $&;

sub addtypdefidentifier {
    $typedefidentifiers = $typedefidentifiers . $+{identifierraw} . "|" if(not $mute_callbacks and $+{typedefkeyword});
}

sub getypedefidentifiers {
    return substr $typedefidentifiers, 0, -1;
}

sub defaultcallback {
    print "$_[0]\n" if (not $mute_callbacks);
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

    my $regexfile = $regexfilecontent; 
    
    $regexfile =~s/#restrictoutsidefacet\b//g;

    $regexfile =~s/\(\?\?C(\d++)\)/(?C$1)/g;

    my $regexfilecontentcopy = $regexfilecontent;

    sub replacefacetgroups {
        $regexfilecontent =~s/\Q$_[0]\E(facet)?>/(/g;
    }

    replacefacetgroups($1, $_[1]) while($regexfilecontentcopy =~/(\(\?<\w+)facet>/g);



    $regexfilecontent =~s/\(\?C(\d++)\)//g;

    $regexfilecontent =~s/\(\?\?C(\d++)\)/(?C$1)/g;

    $regexfilecontent =~s/[(]\?&(\w+?)(facet)?[)]/(?&$1facet)/g;

    $regexfilecontent =~s/(\(\?<\w+)>/$1facet>/g;

    $regexfilecontent =~s/(\(\?<\w+)#restrictoutsidefacet>/$1facet>(*F)/g;

    $mainregexfinal = $mainregexfinal . $regexfile . $regexfilecontent;
}