#!/usr/bin/perl

use re 'eval';

use experimental 'switch';

use File::Basename;

$oldfh = select(STDERR);

my $inpar = qr{(?<inpar>\((?<inner>(([^][()\\]|\\.)++|(?&inpar)
                        |(?<insquare>\[\^?+(.|\\.)(([^]\\]|\\.)++
                        |(?&insquare))*\]))*)\))}xxs;

$filename = "regexes/main.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $mainregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = "regexes/identifier.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $identifierregexfilecontent = do { local $/; <$fh> };

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

my $defaultidentifiers = {};

@typedefidentifiersvector = ($defaultidentifiers);

my $entryregex;

my $matchinperl = 1;

chdir "regexes";

$mainregexfilecontent =~/$metaregexfilecontent/;

chdir "..";

#(?{parseregexfile($+{filename})})
#(?{entryregexmain($+{entrygroup}, $+{prefix})})

entryregexmain($-{entrygroup}[0], $-{prefix}[0]);

parseregexfile((substr $mainregexfilecontent, length $&), 1);

$mainregexdefs = $mainregexfinal . ")";

$mainregexdefs =~s/\s|\n//g if(not $matchinperl);

$mainregexdefs = "$mainregexdefs(?&&$entryregex)";

my %cached_instances = ();

while($mainregexdefs =~ m{
    [(][?]&&(\w+?)(facet)?+[)]
}sxxg) {
    $cached_instances{"$1"} = "(?&$1)";
    $cached_instances{"${1}facet"} = dofacetreplacements("(?&$1)")
}

sub instantiate_cached_instance {
    my $arg = $_[0];
    my $isfacet = $arg =~ m{facet$} or $+{facet} ? "facet" : "";
    $arg =~ s{facet$}{};
    return $cached_instances{$arg . $isfacet};
}

$mainregexdefs =~ s{
    [(][?]&&(\w+?(facet)?+)[)]
}{
    (??{instantiate_cached_instance($1)})
}sxxg;

$mainregexdefs =~ s{
    [(][?]&(\w+)facet[)]
}{
    dofacetreplacements("(?&$1)")
}sxxge;

while(my($k, $v) = each %cached_instances) { 
    use if $ARGV[1], re => qw(Debug EXECUTE);
    $cached_instances{$k} = qr{$mainregexdefs|$v}sxx
}

print $mainregexdefs;

startmatching($subject, $mainregexfinal, basename($ARGV[0], @suffixlist)) if(not $matchinperl);

exit if(not $matchinperl);

startmodule(basename($ARGV[0], @suffixlist)) if(defined &startmodule);

{
    require "extractfns.pm";

    use if $ARGV[1], re => qw(Debug EXECUTE); 

    my $entry = $cached_instances{$entryregex};

    $subject =~ m{^$entry$}sxx
}

#print $&;

exit;

sub debugcallout {
    #print Dumper(\%+);
    my $captures = { %+ };
    #print "facet -> $facet\n";
    my $cond = ($entryregex =~ m{facet$} or not $facet);
    #return unless($entryregex =~ m{facet$} or not $facet);
    #require re;
    #re->import('debug') if($_[1] eq "Ptr64ToPtr");
    #select($oldfh) if($_[1] eq "Ptr64ToPtr");
    #my @arr = @_;
    my $subslice = substr $subject, pos(), 10;

    $subslice =~ s{\R}{ }g;

    if ($_[0] ne "identifier_typedef" and $cond) {
        use Data::Dumper;
        use POSIX;
    
        print strftime ("%F %T", localtime time) . " capture: " . $subslice . "\n";
        print $_[0] . "\n";
        print Dumper(\$captures)
    }
    
    #foreach my $i (@arr) {
    #    print $i . "\n";
    #}
    my $res;
    if(defined &{ $_[0] } and $cond or $_[0] eq "identifier_typedef") {
        $res = $_[0]->($captures) 
    }

    callout($_[0], $captures) if(defined &callout and $cond);
    return $res;
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

sub parsing {
    given ($_[0])
    {
        when (38) { return $typedefidentifiersvector[-1]; }
        when (39) 
        { 
            my $lastelem = pop @typedefidentifiersvector;
            $lastelem = $lastelem . "|" . $_[1] unless (not $_[3]);
            push @typedefidentifiersvector, $lastelem;
        }

        when (44) {
            push @typedefidentifiersvector, "(*F)";
        }

        when (45) {
            pop @typedefidentifiersvector;
        }
    }
}

=cut

sub identifier_typedef {
    my $ident = "(*F)";
    my %disallowed;
    foreach my $typedefidentifier (reverse @typedefidentifiersvector) {
        foreach my $k (keys %$typedefidentifier) {
            $ident = "$ident|\Q$k\E(?!(?&templateargsdecomp)|(?&identifierrawrest))" if($typedefidentifier->{$k} and not exists $disallowed{$k});
            $disallowed{$k} = 1;
        }
    }
    use if $ARGV[1] eq 2, re => qw(Debug EXECUTE);
    #$ident = qr{(?>(?<typedef>(?<ident>(*F)))|$ident)(?(<ident>)(*F))}sxx;
    #$ident = "(?>$ident)";
    no warnings qw(experimental::vlb);
    #print $ident . "\n";
    return qr{(?(DEFINE)$identifierregexfilecontent)(?=(?&identifierrawinner))$ident}sxx;
}

sub identifier_decl_object {
    my $identifier = $_[0]{'ident'};
    return if not $identifier;  
    ${$typedefidentifiersvector[-1]}{$identifier} = $_[0]{'typedefkey'};

}

sub beginscope {
    push @typedefidentifiersvector, {};
}

sub endscope {
    pop @typedefidentifiersvector;
}

sub entryregexmain {
    $entryregex = $_[0];
}

sub getnext {
    $_[0] =~ s {^\s*+(?<arg>\S+?)\s*+($|,)}{};

    return $+{arg};
}

sub substitutetemplateinstances {
    my $backup = $&;
    my $regexcontent = $_[0];

    my $template = $+{template};

    my $templatetoreplace = $template;

    $templatetoreplace =~ s {\#\S+$}{};

    my $args = $+{args};

    my $name = $+{name};

    my $doubleref = $+{doubleref};

    my $isfacet = $+{isfacet};

    #$isfacet = "(*F)" if(not $isfacet);

    return $backup unless($regexcontent =~ m {\(\?<(?<params>[^<>()]*?)>\)\s*+(?=\(\?<$templatetoreplace(?<isfacet>$isfacet)?+\b)$inpar}gxxs);

    #$isfacet = not $+{isfacet};

    #push @arrayoftemplates, $& unless ($& ~~ @arrayoftemplates);

    my $params = $+{params};

    my $body = $+{inpar};

    #push @arrayoftemplates, $templatetoreplace unless ($templatetoreplace ~~ @arrayoftemplates);

    while(1) {
        #my $arg = getnext($args);

        $args =~ s{^(\s*+$inpar\s*+|\s*+(?<arg>\S*?)\s*+($|,))}{}sxx;

        #print $+{inpar} . "\n\n";

        my $inpar = $+{inpar};

        my $arg = $+{arg};

        #print $arg . "\n";

        my $argident;

        my $argcallout;

        my $argqualifs;

        my $tmplargs;

        if(not $inpar) {

            $arg =~ m{((?<ident>[_a-zA-Z][_a-zA-Z0-9]*+)|(?<callout>\d++))(?<qualifs>\S*+)}x;

            $argident = $+{ident};

            $argcallout = $+{callout};

            $argqualifs = $+{qualifs};

            $tmplargs = $+{inargs};
            
        }

        my $param = getnext($params);

        last if(!$param);

        #print $inpar;

        $arg = $argident = substitutetemplateinstancesdoregex($inpar, $regexcontent) if($inpar);
        $body = $inpar . $body;

        print $param . "->" . $argident . "\n";

        $body =~ s{(\(\?&{1,2})$param((facet)?+(?<inargs><(?<args>[^<>]++|(?&inargs))*+>)?+)\)}{
            if($arg) {if(not $argcallout) { $1 . $argident .  $2 . ")" . $argqualifs; }
            else {
                "(" . $argqualifs . "?C" . $argcallout .  $2 . ")" ;
            } }else {
                "()"
            }}eg;

        $body =~ s{\b$param\b}{$argident}g;

        #print $str;
    }

    if($name) {
        $body =~ s{(?<=\(\?<)\Q$template\E\b}{$name}e;
    } elsif(not $doubleref) {
        $body =~ s{\(\?<\Q$template\E\b.*?>}{(};
    }
    if($isfacet) {
        $body = dofacetreplacements($body);
    } else {
        #my $bodyoriginal = $body;
        #dofacetreplacements($body);
        #$body = "(?(DEFINE)" . $body . ")" . $bodyoriginal;
    }

    return $body;

    #replacefacetgroups($1, $_[1]) while($regexcontent =~ m {(\(([^()]++|(?1))*\))}gxx);
}

sub parseregexfile {
    my $filename;
    if(not defined $_[1])
    {
        $filename = $_[0];
        open my $fh, '<', $filename or die "error opening $filename: $!";

        $regexfilecontent = do { local $/; <$fh> };

        close $fh;

    } else {
        $regexfilecontent = $_[0];
    }

    $regexfilecontent =~ s {(?=\(\?\#)$inpar}{}gsxx;

    $regexfilecontent =~s/#restrictperlonly//g if($matchinperl);

    $regexfilecontent =~s/#restrictpcre2only//g if(not $matchinperl);

    $regexfilecontent =~ s {(?=(\(\?<\w+)\#restrictpcre2only>)$inpar}{}gsxx if($matchinperl);

    $regexfilecontent =~ s {(?=(\(\?<\w+)\#restrictperlonly>)$inpar}{}gsxx if(not $matchinperl);

    #@arrayoftemplates = ();

    my $alltemplates = "";

    $alltemplates = $alltemplates . "\n\n" . $& while($regexfilecontent =~s {\(\?<(?<params>[^()<>]*?)>\)\s*+
                    $inpar}{}sxx);

    
    #print $alltemplates if($filename eq "inner.regex");

    #exit if($filename eq "inner.regex");

    sub substitutetemplateinstancesdoregex {
        my $templatename;
        $_[0] =~s {\(\?&(?<doubleref>&)?+(?<template>((?![)])\S)+?)(?<isfacet>facet)?+
                            (?<inargs><(?<args>[^<>]++|(?&inargs))*+>)
                            (=(?<name>((?![)])\S)+))?\)}
                            {
                                $templatename = $+{template};
                                print "\n\n\n========================================================";
                                print "substituting $templatename \n";
                                print "========================================================\n\n\n";
                                substitutetemplateinstances($_[1])
                            }gxxe;

        return $templatename;
    }

    while(substitutetemplateinstancesdoregex($regexfilecontent, $alltemplates)) {
        print $regexfilecontent;
    }

    #print $regexfilecontent;
    #exit;

    #print @arrayoftemplates;

    foreach my $template (@arrayoftemplates) {
        #print "\n\n$template\n\n";
        $regexfilecontent =~s {\(\?<(?<params>.*?)>\)\s*+(?=\(\?<\Q$template\E\b)$inpar}{}gxx ;
    }

    #print $regexfilecontent;

    #exit;

    my $regexfile = $regexfilecontent; 
=begin
    sub addfacetdefines {
        $_[0] =~s{(?(DEFINE)$inpar)(?=(?<parnnm>[(]\?<\w+?>(?<!facet>)))(?<body>(?&inpar))}
        {
            my $bodyorig = $+{body}, $bodyfacet = $&;
            my $parnnm = $+{parnnm};
            $bodyorig =~ s{^[(]\?<\w+?>}{}sxx;
            $bodyorig =~ s{[)]$}{}sxx;
            print "paramnm -> " . $bodyorig . "\n";
            "((*F)" . dofacetreplacements($bodyfacet) . "|" . $parnnm . addfacetdefines($bodyorig) . "))"
            }sxxge;

        return $_[0]
    }

    addfacetdefines($regexfile);
=cut    
    $regexfile =~s/(?<!<)#restrictoutsidefacet\b>/>/g;

    $regexfile =~s/\(\?<#restrictoutsidefacet>/(/g;

    #$regexfile =~s/[(]\?&(?>\w+?)\#nofacet[)]/(?&$1)/g;

    #$regexfile =~s/[(]\?[(]<(\w+?)#nofacet>[)]/(?(<$1>)/g;

    #$regexfile =~s/\(\?\?C(\d++)\)/(?C$1)/g if(not $matchinperl);

    #$regexfile =~s/\(\?C(\d++)(<(?<args>.*?)>)?+\)/(?{callout($1 . ($+{args} ? "," . $+{args} : ""))})/g if($matchinperl);

    #$regexfile =~s/\(\?<(\w+)#nofacet>/(?<$1>/g;

    my $regexfilecontentcopy = $regexfilecontent;
=begin
    sub replacefacetgroups {
        $regexfilecontent =~s/\Q$_[0]\E(facet)?>/(/g;
    }

    replacefacetgroups($1, $_[1]) while($regexfilecontentcopy =~/(\(\?<\w+)facet>/g);

    sub replacenofacetgroups {
        my $name = $_[0];
        $_[1] =~s/[(]\?&\Q$name\E[)]/(?&$name#nofacet)/g;
        $_[1] =~s/\(\?<\Q$name\E\#nofacet>($inpar|(?&inner))\)/(?&$name#nofacet)/gsx;
    }

    replacenofacetgroups($1, $regexfilecontent) while($regexfilecontentcopy =~/\(\?<(\w+)#nofacet>/g); # remove references to nofacet groups

    sub dofacetreplacements {
        $_[0] =~s/\(\?C&(?!\S+?facet)(\S+?)\s*(<(?<args>[^()<>]*?)>)?\)/(?C&$1facet$2)/gs;

        #$regexfilecontent =~s/\(\?\?C(\d++)\)/(?C$1)/g if(not $matchinperl);

        $_[0] =~s/[(]\?([(]R)?+&(&?+\w+?)(facet)?+\b/(?$1&$2facet/g;

        #$regexfilecontent =~s/([(]\?)?+[(]<(?>\w+?)(facet)?+>[)]/$1(<$2facet>)/g;

        #$regexfilecontent =~s/[(]\?&(?>\w+?)\#nofacet[)]/(?&$1)/g;

        #$regexfilecontent =~s/[(]\?[(]<(\w+?)#nofacet>[)]/(?(<$1>)/g;

        $_[0] =~s{([(]\?[(]?+)<(\w+?)(facet)?+>}{$1<$2facet>}g;
        
        #$regexfilecontent =~s/(\(\?<\w+)>/$1facet>/g;

        $_[0] =~s/(\(\?<\w+)#restrictoutsidefacet>/$1facet>(*F)/g;

        $_[0] =~s/\(\?<#restrictoutsidefacet>/((*F)/g;
        
        return $_[0]
    }
=cut
    sub dofacetreplacements {
        my $actual = $_[0];

        #return "(?(DEFINE)(?<facetsub>(?<facet>)$actual))(?&facetsub)";
        return "(?(?{\$facet})$actual|((?{\$facet=1})$actual(?{\$facet=0})|(?{\$facet=0})(*F)))";
    }
    #dofacetreplacements($regexfilecontent);

    $mainregexfinal = $mainregexfinal . $regexfile;

    $mainregexfinal =~s/\((\?\??+)C&(\S+?)\s*(<(?<args>[^()<>]*?)>)?\)/
            my $prefix = "(" . $1 . "{debugcallout(" . "\"$2\"";
            $prefix . ($+{args} ? "," . ($+{args} =~ s {\b\S+\b}{\$\+\{$&\}}gr) : "") . ")})"
            /ges if($matchinperl);

    #$mainregexfinal =~s/\(\?\?C&(\S++)\)/(?C&$1)/g if(not $matchinperl);
    $mainregexfinal =~s/\(\?C(\d++)\)//g;
}