#!/usr/bin/perl

use re 'eval';

use experimental 'switch';

use File::Basename;

$filename = "main.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $mainregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = "./utility/regex.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $metaregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $subject = do { local $/; <$fh> };

close $fh;

#$filename = "output.txt";
#open fhoutput, '>', $filename or die "error opening $filename: $!";

#select fhoutput;

my $mainregexfinal = "((*F)";

my $entryregex;

my $matchinperl = 1;

$mainregexfilecontent =~/$metaregexfilecontent/;

#(?{parseregexfile($+{filename})})
#(?{entryregexmain($+{entrygroup}, $+{prefix})})

entryregexmain($-{entrygroup}[0], $-{prefix}[0]);

parseregexfile((substr $mainregexfilecontent, length $&), 1);

$mainregexfinal = $mainregexfinal . ")|" . $entryregex;

my $typedefidentifiers = "";

$mainregexfinal =~s/\s|\n//g if(not $matchinperl);

#print $mainregexfinal;

startmatching($subject, $mainregexfinal, basename($ARGV[0], @suffixlist)) if(not $matchinperl);

exit if(not $matchinperl);

print $mainregexfinal;

startmodule(basename($ARGV[0], @suffixlist));

{
    #$oldfh = select(STDERR);

    #use re 'debug';

    $subject =~m{$mainregexfinal}sxx;
}

#print $&;

sub addtypdefidentifier {
    $typedefidentifiers = $typedefidentifiers . $+{identifierraw} . "|" if(not $mute_callbacks and $+{typedefkeyword});
}

sub getypedefidentifiers {
    return substr $typedefidentifiers, 0, -1;
}

sub printifdefined{
    print $_[0] if($_[1]);
}

sub debugcallout {
    #require re;
    #re->import('debug') if($_[1] eq "Ptr64ToPtr");
    #select($oldfh) if($_[1] eq "Ptr64ToPtr");
    my @arr = @_;
    my $subslice = substr $subject, pos(), 10;

    $subslice =~ s{\R}{ }g;
    
    print  "capture: " . $subslice . "\n";
    foreach my $i (@arr) {
        print $i . "\n";
    }
    callout(@arr);
}

=for comment

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

=cut

sub entryregexmain {
    $entryregex = $_[1] . "(?&" . $_[0] . ")";
}

sub getnext {
    $_[0] =~ s {\s*+(?<arg>\S+?)\s*+($|,)}{};

    return $+{arg};
}

sub substitutetemplateinstances {
    my $regexcontent = $_[0];

    my $template = $+{template};

    my $templatetoreplace = $template;

    $templatetoreplace =~ s {\#\S+$}{};

    my $args = $+{args};

    my $name = $+{name};

    my $doubleref = $+{doubleref};

    my $isfacet = $+{isfacet};

    #$isfacet = "(*F)" if(not $isfacet);

    $regexcontent =~ m {\(\?<(?<params>.*?)>\)\s*+(?=\(\?<$templatetoreplace(?<isfacet>$isfacet)?+\b)(?<inpar>(?<!\\)\((
                    ([^][()]|(?<=\\).)++|(?&inpar)|(?<insquare>(?<!\\)\[\^?+(.|\\.)(
                    ([^]]|(?<=\\).)++|(?&insquare))*(?<!\\)\]))*(?<!\\)\))}gxx;

    #$isfacet = not $+{isfacet};

    #push @arrayoftemplates, $& unless ($& ~~ @arrayoftemplates);

    my $params = $+{params};

    my $body = $+{inpar};

    push @arrayoftemplates, $templatetoreplace unless ($& ~~ @arrayoftemplates);

    while(1) {
        my $arg = getnext($args);

        $arg =~ m{((?<ident>[_a-zA-Z][_a-zA-Z0-9]*+)|(?<callout>\d++))(?<inargs><([^<>]++|(?&inargs))*+>)?+(?<qualifs>\S*+)};

        my $argident = $+{ident};

        my $argcallout = $+{callout};

        my $argqualifs = $+{qualifs};

        my $tmplargs = $+{inargs};

        my $param = getnext($params);

        last if(!$arg || !$param);

        if($tmplargs and not $argident ~~ @arrayoftemplate) { #if recursive template arguments append template body
            $arg = "(?&&" . $argident . $tmplargs . ")"; 
            substitutetemplateinstancesdoregex($arg, $regexcontent);
            $body = $arg . $body;
        }

        $body =~ s{(\(\?&{1,2})$param((facet)?+(<.*?>)?+)\)}{
            if(not $argcallout) { $1 . $argident .  $2 . ")" . $argqualifs; }
            else {
                "(" . $argqualifs . "?C" . $argcallout .  $2 . ")" ;
            }}eg
    }

    if($name) {
        $body =~ s{(?<=\(\?<)\Q$template\E\b}{$name}e;
    } elsif(not $doubleref) {
        $body =~ s{\(\?<\Q$template\E\b.*?>}{(};
    }

    dofacetreplacements($body) if($isfacet);

    return $body;

    #replacefacetgroups($1, $_[1]) while($regexcontent =~ m {(\(([^()]++|(?1))*\))}gxx);
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

    $regexfilecontent =~ s {(?=\(\?\#)(?<inpar>(?<!\\)\((([^][()]|(?<=\\).)++|(?&inpar)
                        |(?<insquare>(?<!\\)\[\^?+(.|\\.)(([^]]|(?<=\\).)++
                        |(?&insquare))*(?<!\\)\]))*(?<!\\)\))}{}gsxx;

    $regexfilecontent =~s/#restrictperlonly//g if($matchinperl);

    $regexfilecontent =~s/#restrictpcre2only//g if(not $matchinperl);

    $regexfilecontent =~ s {(?=(\(\?<\w+)\#restrictpcre2only>)(?<inpar>(?<!\\)\((([^][()]|(?<=\\).)++|(?&inpar)
                        |(?<insquare>(?<!\\)\[\^?+(.|\\.)(([^]]|(?<=\\).)++
                        |(?&insquare))*(?<!\\)\]))*(?<!\\)\))}{}gsxx if($matchinperl);

    $regexfilecontent =~ s {(?=(\(\?<\w+)\#restrictperlonly>)(?<inpar>(?<!\\)\((([^][()]|(?<=\\).)++|(?&inpar)
                        |(?<insquare>(?<!\\)\[\^?+(.|\\.)(([^]]|(?<=\\).)++
                        |(?&insquare))*(?<!\\)\]))*(?<!\\)\))}{}gsxx if(not $matchinperl);

    @arrayoftemplates = ();

    sub substitutetemplateinstancesdoregex {
        return $_[0] =~s {\(\?&(?<doubleref>&)?+(?<template>((?![)])\S)+?)(?<isfacet>facet)?+
                            (?<inargs><(?<args>[^<>]++|(?&inargs))*+>)
                            (=(?<name>((?![)])\S)+))?\)}
                            {substitutetemplateinstances($_[1])}gxxe;
    }

    while(substitutetemplateinstancesdoregex($regexfilecontent, $regexfilecontent)) {}

    #print @arrayoftemplates;

    foreach my $template (@arrayoftemplates) {
        #print "\n\n$template\n\n";
        $regexfilecontent =~s {\(\?<(?<params>.*?)>\)\s*+(?=\(\?<\Q$template\E\b)(?<inpar>(?<!\\)\((
                    ([^][()]|(?<=\\).)++|(?&inpar)|(?<insquare>(?<!\\)\[\^?+(.|\\.)(
                    ([^]]|(?<=\\).)++|(?&insquare))*(?<!\\)\]))*(?<!\\)\))}{}gxx ;
    }

    #print $regexfilecontent;

    #exit;

    my $regexfile = $regexfilecontent; 
    
    $regexfile =~s/(?<!<)#restrictoutsidefacet\b//g;

    $regexfile =~s/\(\?<#restrictoutsidefacet>/(/g;

    #$regexfile =~s/[(]\?&(?>\w+?)\#nofacet[)]/(?&$1)/g;

    #$regexfile =~s/[(]\?[(]<(\w+?)#nofacet>[)]/(?(<$1>)/g;

    #$regexfile =~s/\(\?\?C(\d++)\)/(?C$1)/g if(not $matchinperl);

    #$regexfile =~s/\(\?C(\d++)(<(?<args>.*?)>)?+\)/(?{callout($1 . ($+{args} ? "," . $+{args} : ""))})/g if($matchinperl);

    #$regexfile =~s/\(\?<(\w+)#nofacet>/(?<$1>/g;

    my $regexfilecontentcopy = $regexfilecontent;
=begin comment
    sub replacefacetgroups {
        $regexfilecontent =~s/\Q$_[0]\E(facet)?>/(/g;
    }

    replacefacetgroups($1, $_[1]) while($regexfilecontentcopy =~/(\(\?<\w+)facet>/g);

    sub replacenofacetgroups {
        my $name = $_[0];
        $_[1] =~s/[(]\?&\Q$name\E[)]/(?&$name#nofacet)/g;
        $_[1] =~s/\(\?<\Q$name\E\#nofacet>((?<inpar>(?<!\\)\((?<content>(([^][()]|(?<=\\).)++|(?&inpar)
                        |(?<insquare>(?<!\\)\[\^?+(.|\\.)(([^]]|(?<=\\).)++
                        |(?&insquare))*(?<!\\)\]))*)(?<!\\)\))|(?&content))\)/(?&$name#nofacet)/gsx;
    }

    replacenofacetgroups($1, $regexfilecontent) while($regexfilecontentcopy =~/\(\?<(\w+)#nofacet>/g); # remove references to nofacet groups
=cut
    sub dofacetreplacements {
        $_[0] =~s/\(\?C(\d++)(<(?<args>.*?)>)?+\)//g;

        #$regexfilecontent =~s/\(\?\?C(\d++)\)/(?C$1)/g if(not $matchinperl);

        $_[0] =~s/[(]\?&(\w+?)(facet)?+[)]/(?&$1facet)/g;

        #$regexfilecontent =~s/([(]\?)?+[(]<(?>\w+?)(facet)?+>[)]/$1(<$2facet>)/g;

        #$regexfilecontent =~s/[(]\?&(?>\w+?)\#nofacet[)]/(?&$1)/g;

        #$regexfilecontent =~s/[(]\?[(]<(\w+?)#nofacet>[)]/(?(<$1>)/g;

        $_[0] =~s{([(]\?[(]?+)<(\w+?)(facet)?+>}{$1<$2facet>}g;
        
        #$regexfilecontent =~s/(\(\?<\w+)>/$1facet>/g;

        $_[0] =~s/(\(\?<\w+)#restrictoutsidefacet>/$1facet>(*F)/g;

        $_[0] =~s/\(\?<#restrictoutsidefacet>/((*F)/g;
    }

    dofacetreplacements($regexfilecontent);

    $mainregexfinal = $mainregexfinal . $regexfile . $regexfilecontent;

    $mainregexfinal =~s/\((\?\??+)C(\d++)(<(?<args>.*?)>)?+\)/
            my $prefix = "(" . $1 . "{debugcallout(" . $2;
            $prefix . ($+{args} ? "," . ($+{args} =~ s {\b\S+\b}{\$\+\{$&\}}gr) : "") . ")})"
            /ges if($matchinperl);

    $mainregexfinal =~s/\(\?\?C(\d++)\)/(?C$1)/g if(not $matchinperl);
}