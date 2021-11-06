#!/usr/bin/perl

my $inpar = qr{(?<inpar>\((?<inner>(([^][()\\]|\\.)++|(?&inpar)
                        |(?<insquare>\[\^?+(.|\\.)(([^]\\]|\\.)++
                        |(?&insquare))*\]))*)\))}xxs;

my $matchinperl = 1;

use re 'eval';

use experimental 'switch';


use Data::Dumper;

use File::Basename;

use List::Util qw(first);

$filename = "regexes/main.regex";
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

my $mainregexfinal = $matchinperl ? "(?(DEFINE)" : "((*F)";

#@typedefidentifiersvector = ("(*F)");

my $entryregex;

chdir "regexes";

$mainregexfilecontent =~/$metaregexfilecontent/;

chdir "..";

#(?{parseregexfile($+{filename})})
#(?{entryregexmain($+{entrygroup}, $+{prefix})})

entryregexmain($-{entrygroup}[0], $-{prefix}[0]);

parseregexfile((substr $mainregexfilecontent, length $&), 1);

$mainregexfinal = $mainregexfinal . ")" . ($matchinperl ? "" : "|")  . $entryregex;

%regexsubshash;

$mainregexfinal =~s/\((\?\??+)C&(\S+?)\s*(<(?<args>[^()<>]*?)>)?\)/
            my $prefix = "(" . $1 . "{debugcallout(" . "\"$2\"";
            $prefix . ($+{args} ? "," . ($+{args} =~ s {\b\S+\b}{\$\+\{$&\}}gr) : "") . ")})"
            /ges if($matchinperl);

    #$mainregexfinal =~s/\(\?\?C&(\S++)\)/(?C&$1)/g if(not $matchinperl);
$mainregexfinal =~s/\(\?C(\d++)\)//g;

$mainregexfinal =~ s{[(]\?&(?<doubleref>&)?+(\w+?)(?<isfacet>facet)?+[)]}
                    {(??{callsub("$2", "$+{isfacet}", $-[0], "$+{doubleref}")})}g;

$mainregexfinal =~ s{(?<=[(]\?[(])<(\w+?)>}{?{iscaptexist($1)}}g;

$mainregexfinal =~ s{\\g\{(\w+?)\}}{(??{getcapt($1)})}g;

sub getcapt {
    return %lastmatches{$_[0]}
}

sub iscaptexist {
    return %lastmatches{$_[0]}
}

sub findsubs {

    while($_[0] =~ m{[(]\?<(\w+?)(?<isfacet>facet)?+>(?<inside>$inpar|(?&inner))[)]}gsxx) {
        my $subname = $1;
        my $subbody = $&;
        my $isfacet = $+{isfacet};
        my $lastpos = $-[0];
        my $inside = $+{inside};

        findsubs($inside);

        print "\n\n\n".  $subbody . "\n\n\n";

        #$regexsubshash{$subname} = [] unless defined $regexsubshash{$subname};

        #use re 'debug';

        #use re qw(Debug MATCH);

        while($subbody =~ s{[(]\?<(\w+?)(?<isfacet>facet)?+>(?<inside>$inpar|(?&inner))[)]}
        {((?{registernc("$1$+{isfacet}")})($+{inside})(?{popnc()})|(?{popnc()})(*F))}gsxx){}


        my $regextmpimpl = qr{($subbody(?{callsubcleanup()})|
                    (?{callsubcleanup()})(*F))}sxx;

        push @{$regexsubshash{$subname}}, ({impl => $regextmpimpl,
                                    initialpos => $lastpos,
                                    isfacet => $isfacet}) unless first { $lastpos eq $_{initialpos} } @{$regexsubshash{$subname}};
    }
}

findsubs($mainregexfinal);

@_ = sort {$a{initialpos} <=> $b{initialpos}} @_ for values %regexsubshash;

@facetstack = ();
#my @matchesstack = ({});
%lastmatches;
@infostack = ();

@namedcaptures;

@substack;

sub registernc {
    push @namedcaptures, $_[0];
}

sub popnc {
    my $capt;
    $capt = ${$#-} if($#-);
    my $nmcapt = pop @namedcaptures;
    print "pop: " . $nmcapt . " - $capt\n";
    $lastmatches{$nmcapt} = $capt;
}

sub callsub {
    #print Dumper, %+;
    my $subname = $_[0];
    my $isfacet = $_[1];
    my $initialpos = $_[2];
    my $doubleref = $_[3];
    #my %lastmatch = %+;
    my @currsubs = @{$regexsubshash{$subname}};

    my %lastsub;

    #print Dumper \%+;

    for my $el (reverse(@currsubs)) {
        $lastsub = { %$el } if(($el{isfacet} eq $isfacet) or $isfacet);
        last if($el{initialpos} < $initialpos)
    }

    #print Dumper \$lastsub;

    push @substack, { %lastmatches };

    push @facetstack, {} if($isfacet);
    print "gosub \n" . scalar @facetstack . "\n";
    #print Dumper \%$lastmatch;
    #push @matchesstack, { %$lastmatch };
    #@lastmatches{keys %lastmatch} = values %lastmatch;
    push @infostack, {isfacet => $isfacet,
                    doubleref => $doubleref};

    return $lastsub->{impl};
}

sub callsubcleanup {
    #print Dumper, %+;
    #my %lastmatch = %+;
    my $lastinfo = pop @infostack;
    #my $subname = $_[0];
    my $isfacet = $lastinfo->{isfacet};#$_[1];
    #my $initialpos = $_[2];
    my $doubleref = $lastinfo->{doubleref};
    
    pop @facetstack if($isfacet);

    #my $lastmatches = pop @matchesstack;
    #my $lastlastmatches = pop @matchesstack;

    #print Dumper, \$lastmatch;

    if($doubleref) {
        #@lastmatches{keys %lastmatch} = values %lastmatch;

        #print Dumper \$lastlastmatches;

        #push @matchesstack, %$merged;
    } else {
        %lastmatches = %{pop @substack};
        #push @matchesstack, %$lastmatches;
    }
    #return $result;
}

=for comment

$mainregexfinal =~s{[(]\?&(\w+?)facet[)]}{

    The strategy is - last on the left side determines
    or any of the right side - with priority taking a facet declaration

        my $whole = $&;
        my $lastmatch = @-;
        my $subname = $1;
        my $foundlastfacet;
        my $foundany;
        my $foundanyfacet;
        my $isafter;
        while($_ =~ m{[(]\?<${subname}(?<isfacet>facet)?+>(?<inside>(?<inpar>(?<!\\)\((?<inner>(([^][()]|(?<=\\).)++|(?&inpar)
                        |(?<insquare>(?<!\\)\[\^?+(.|\\.)(([^]]|(?<=\\).)++
                        |(?&insquare))*(?<!\\)\]))*)(?<!\\)\))|(?&inner))[)]}g) {
            last if(@- > $lastmatch and $foundany and not $isafter);
            $foundlastfacet = $+{isfacet};
            $foundany //= $&;
            $foundanyfacet //= $+{isfacet};
            $isafter = @- > $lastmatch;
        }
        if(not $isafter and not $foundlastfacet or not $foundanyfacet) {
            "((?<facetarg>)(?&$1)(?<facetarg>(*F))|)"
        } else {
            $whole
        }
    }ge;

=cut

my $typedefidentifiers = "";

$mainregexfinal =~s/\s|\n//g if(not $matchinperl);

#print $mainregexfinal;

startmatching($subject, $mainregexfinal, basename($ARGV[0], @suffixlist)) if(not $matchinperl);

exit if(not $matchinperl);

#system cls;

#print $mainregexfinal;

#exit;

#use Chj::ruse;

push @INC, ".";

eval {
    delete @{\%INC}{grep m{^src/}, keys %INC };
    #print Dumper \%INC;
    require src::typedefs;
    #$oldfh = select(STDERR);

    $filename = $ARGV[0];

    open my $fh, '<', $filename or die "error opening $filename: $!";

    my $subject = do { local $/; <$fh> };

    close $fh;

    startmodule(basename($ARGV[0], @suffixlist)) if(defined &startmodule);

    if($^X =~ m{perl.exe$}) {
        use re 'debug';

        $subject =~ callsub("cprogram", "", 0)
    } else {
        print "\n\n\n========================================================";
        print "final regex \n";
        print "========================================================\n\n\n";
        #print $mainregexfinal . "\n";
        print Dumper \%regexsubshash; 

        $subject =~ callsub("cprogram", "", 0)
    }

    #struc_or_union_body();

    endmodule() if(defined &endmodule);
    
    {
        1
    }
} or do {
    print $@;
}; #while({#system CLS,
 #system PAUSE, 1});

#print $&;

print Dumper(@compoundscope);

exit;

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
    #print Dumper, %+;
    #print Dumper(\%+);
    print "callout \n" . scalar @facetstack . "\n";
    return if(scalar @facetstack);
    
    my %captures = %lastmatches;

    #@captures{keys %+} = values %+;

    #@captures{ keys %$_ } = values %$_ for (@matchesstack, ({ %+ }));
    
    #require re;
    #re->import('debug') if($_[1] eq "Ptr64ToPtr");
    #select($oldfh) if($_[1] eq "Ptr64ToPtr");
    #my @arr = @_;
    my $subslice = substr $subject, pos() - 10, 20;

    $subslice =~ s{\R}{ }g;
    
    if($_[0] ne "identifier_typedef") {
        print  "capture: " . (substr $subslice, 10) . "\n";
        print  "capture: " . (substr $subslice, 0, 10) . "\n";
        print $_[0] . "\n";
        print Dumper(\%captures);
    }
    #foreach my $i (@arr) {
    #    print $i . "\n";
    #}
    my $res = $_[0]->(%captures) if(defined &{ $_[0] });
    callout($_[0], %captures) if(defined &callout);
    #return $res;
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

sub entryregexmain {
    $entryregex = $_[1] . "(?&" . $_[0] . ")";
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

    return $backup unless($regexcontent =~ m {\(\?<(?<params>[^()<>]*?)>\)\s*+(?=\(\?<$templatetoreplace(?<isfacet>$isfacet)?+\b)$inpar}gxxs);

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

    while(substitutetemplateinstancesdoregex($body, $regexcontent)) {
        print $body;
    }

    dofacetreplacements($body) if($isfacet); #{
    #    $body =~ s{[(]\?<\S+?>}{$&(?(<facet>)(?<lastfacet>)|((?<lastfacet>(*F))|)(?<facet>))};
    #    $body =~ s{[)]\s*+$}{(?(<lastfacet>)|((?<facet>(*F))|))$&};
    #}

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
        $regexfilecontent =~s {\(\?<(?<params>[^()<>]*?)>\)\s*+(?=\(\?<\Q$template\E\b)$inpar}{}gxxs;
    }

    #print $regexfilecontent;

    #exit;

    my $regexfile = $regexfilecontent; 
    
    #$regexfile =~s/(?<!<)#restrictoutsidefacet\b//g;

    #$regexfile =~s/(\(\?<\w+)#restrictinsidefacet>/$1>(*F)/g;

    #$regexfile =~s/\(\?<#restrictinsidefacet>/((*F)/g;

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

    dofacetreplacements($regexfilecontent);

=cut
    sub dofacetreplacements {
        print "\n\n\n========================================================";
        print "facet replacement before \n";
        print "========================================================\n\n\n"; 

        print $_[0];

        #$_[0] =~s/(?<!<)#restrictinsidefacet\b//g;

        #$_[0] =~s/\(\?<#restrictinsidefacet>/(/g;
        
        $_[0] =~s/\(\?C&(\S+?)\s*(<(?<args>[^()<>]*?)>)?\)//gs;

        #$regexfilecontent =~s/\(\?\?C(\d++)\)/(?C$1)/g if(not $matchinperl);

        $_[0] =~s/[(]\?&&?+(\w+?)(facet)?+[)]/(?&$1facet)/g;

        #$regexfilecontent =~s/([(]\?)?+[(]<(?>\w+?)(facet)?+>[)]/$1(<$2facet>)/g;

        #$regexfilecontent =~s/[(]\?&(?>\w+?)\#nofacet[)]/(?&$1)/g;

        #$regexfilecontent =~s/[(]\?[(]<(\w+?)#nofacet>[)]/(?(<$1>)/g;

        $_[0] =~s{([(]\?[(]?+)<(\w+?)(facet)?+>}{$1<$2facet>}g;

        $_[0] =~s/(\(\?<\w+)#restrictoutsidefacet>/$1facet>(*F)/g;

        $_[0] =~s/\(\?<#restrictoutsidefacet>/((*F)/g;

        $_[0] =~s/(\(\?<\w+)facet?+>/$1facet>/g;

        print "\n\n\n========================================================";
        print "facet replacement after \n";
        print "========================================================\n\n\n"; 

        print $_[0];
    }

     #((?<facetarg>)(?&$1)(?<facetarg>(*F))|)

    $regexfile =~s{(\(\?<\w*?)\#restrictoutsidefacet>(?<inside>$inpar|(?&inner))[)]}{$1>(?(?{scalar \@facetstack})(*F)|($+{inside})))}gsxx;

    $regexfile =~s{[(]\?\<\>}{(}gsxx;

    #$regexfile =~s/[(]\?&(\w+?)[)]/((?<facetarg>(*F))|(?&$1))/g;

    #$regexfile =~s/[(]\?<(\w+?)>/$&(?<facet>\g{facetarg})|/g;

    #$regexfile =~ s{(?=\(\?<\w+?facet\b)(?<inpar>(?<!\\)\((
                    #([^][()]|(?<=\\).)++|(?&inpar)|(?<insquare>(?<!\\)\[\^?+(.|\\.)(
                    #([^]]|(?<=\\).)++|(?&insquare))*(?<!\\)\]))*(?<!\\)\))}{dofacetreplacements($&)}gxxs;

    #$regexfile =~s/[(]\?<(\w+?)facet>/$&(?<facet>)/g;


    $mainregexfinal = $mainregexfinal . $regexfile; #$regexfile . $regexfilecontent;
}