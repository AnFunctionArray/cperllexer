#!/usr/bin/perl

use re 'eval';

BEGIN{push @INC, "./misc"};
BEGIN{push @INC, "./regexes/supplement"};

#use re qw(Debug ALL);

require "typename.regex.pl";

use List::Util qw(max);

use experimental 'switch';

use File::Basename;

use Data::Dumper;

#my sub Dumper {"\n"}

my sub print {1}
my sub print2 {CORE::print(@_)}

sub push2 {
    my @args = @_;
    if(@{$args[0]} ~~ @flags) {
        print "pushing to flags\n";
        print Dumper $args[1];
        print Dumper \@flags;
    }
    push @{$args[0]}, $args[1]
}

sub pop2 {
    my @args = @_;
    if(@{$args[0]} ~~ @flags) {
        print "popping from flags\n";
        print Dumper $flags[-1];
        print Dumper \@flags;
        print Dumper \$args[1];

        if(defined $args[1]) {
            scalar keys %{$flags[-1]} eq scalar keys %{$args[1]} or exit;

            #print "size equal\n";

            foreach my $key (keys %{$flags[-1]}) {
                exit unless (exists $args[1]->{$key});
            }
        }
    }
    pop @{$_[0]}
}

sub existsflag {
    my $flag = $_[0];
    my $exclusions = $_[1];
    foreach my $val (reverse @flags) {
        return 1 if(exists $val->{$flag});

        foreach my $key (keys %{$exclusions}) {
            return 0 if (exists $val->{$key});
        }
    }

    return 0
}

print Dumper @ARGV;

#open my $fhnull, '>', "/dev/null" or die "error opening $filename: $!";

$oldfh = select(STDERR);

my $inpar = qr{(?<inpar>\((?<inner>(([^][()\\]|\\.)++|(?&inpar)
                        |(?<insquare>\[\^?+(.|\\.)(([^]\\]|\\.)++
                        |(?&insquare))*\]))*)\))}xxs;

$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $subject = do { local $/; <$fh> };

close $fh;
=begin
$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $proxy = do { local $/; <$fh> };

close $fh;
=cut

$typedef_regex = qr{(*F)}sxxn;

sub inc2 {

    foreach my $name (@_) {
        "setting $name to -> " . ($$name = ++$$name) . "\n";
    }
}

sub dec2 {

    foreach my $name (@_) {

        "setting $name to -> " . ($$name = --$$name) . "\n";
    }
}

sub unset2 {

    foreach my $bulk (@_) {
        #print "popping $bulk\n";
        pop @$bulk;
        print "unsetting $bulk => $bulk->[-1]\n";
    }
}

sub set2 {
    #print Dumper(@_);
    foreach my $pair (@_) {
        my ($key, $value) = %$pair;
        #print Dumper($pair);
        if(exists $$pair{$key}) {
            push @$key, $value;
            print "pushing $key to $key->[-1]\n"
        }
    }
}

=begin
sub common {
    my @currvalsaltarr = ();
    my $origvars = join(',', @{map { $_ }, @_});

    foreach (@_) {
        if(exists $scopes{$_}) #if belong to a scope 
        {
            my @currstate = map { $_ } @{$scopes{$_}};

            push @currvalsaltarr, @currstate;
        } else {
            push @currvalsaltarr, [$_]
        }
    }
    my $currvalsalt = join(',', @{map { join(',', @{map { $$_ + 0 }, @_}) }, @currvalsaltarr});

    return $currvalsalt
}
=cut

sub evalval {
    $Data::Dumper::Deparse = 1;
    (Dumper $_[0]) =~ m{$(?!VAR\d)\S++\\b};
    print "reading from " . Dumper $_[0];
    my $result = eval {
        $_[0]()
    };
    #warn $@ if $@; 
    #print "eval is $result\n";

    print " with $result\n";

    return $result;
}

sub inc {
    my $vars = join(',', @_);
    return "((?{inc2 $vars})|(?{dec2 $vars})(*F))"
}

sub set {
    $Data::Dumper::Terse = 1;
    my $vars = join(',', map {Dumper($_)} @_);
    my $unset = join(',', map {(keys %$_)[0]} @_);

    #print $vars . "\n";
    $Data::Dumper::Terse = 0;
    return "((?{set2 $vars})|(?{unset2 $unset})(*F))"
}

sub unset {
    $Data::Dumper::Terse = 1;

    my $args = join(',', @_);

    print "un " . $args . "\n";
    $Data::Dumper::Terse = 0;

    #print Dumper @matches;

    return "(*COMMIT)(?{unset2 $args})"
}

sub setmatch {
    return "((?{eval {\$matches[-1]->{$_[0]}=\'$^N\'}})|(?{eval {delete \$matches[-1]->{$_[0]}}})(*F))"
}

sub dec {
    my $vars = join(',', @_);
    return "((?{dec2 $vars})|(?{inc2 $vars})(*F))"
}

#$filename = "output.txt";
#open fhoutput, '>', $filename or die "error opening $filename: $!";

#select fhoutput;

#sub loadregex {

$filename = "regexes/primexpr.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $mainregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = "regexes/regexmeta.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $metaregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = "./utility/regex.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $utilregexfilecontent = do { local $/; <$fh> };

close $fh;

my $mainregexfinal = "";

my $defaultidentifiers = {};

@typedefidentifiersvector = ($defaultidentifiers);

@typedefidentifierschanged = (0);

my $entryregex;

my $matchinperl = 0;

chdir "regexes";

$mainregexfilecontent =~/$utilregexfilecontent/;

chdir "..";

#(?{parseregexfile($+{filename})})
#(?{entryregexmain($+{entrygroup}, $+{prefix})})

entryregexmain($+{entrygroup}, $+{prefix});

parseregexfile((substr $mainregexfilecontent, length $&), 1);

$mainregexdefs = "$mainregexfinal";

$mainregexfinal =~s/\s|\n//g if(not $matchinperl);

#$mainregexdefs = "$mainregexdefs|(?&&$entryregex)";

=begin
my %cached_instances = ();

while($mainregexdefs =~ m{
    (?=[(][?]<(\w+?)>)($inpar)(?=.*?[(][?]&&\1(facet)?+[)])
}sxxg) {
    $cached_instances{$1} = $+{inpar};
    print "\n\n\n========================================================";
                                print "registered $cached_instances{$1} \n";
                                print "========================================================\n\n\n";
}

while($mainregexdefs =~ m{
    [(][?]&&(?<ident>\w+?)(facet)?+[)]
}sxxg) {
    if(not exists $cached_instances{$1}) {
        $cached_instances{$1} = $+{ident};
        print "\n\n\n========================================================";
                                    print "registered $cached_instances{$1} \n";
                                    print "========================================================\n\n\n";
    }
}


while($mainregexdefs =~ m{
    [(][?]&&(\w+?)(facet)?+[)]
}sxxg) {
    $cached_instances{"$1"} = "(?&$1)";
    $cached_instances{"${1}facet"} = dofacetsubreplacements($1)
}


sub instantiate_cached_instance {
    my $arg = $_[0];
    my $isfacet = $arg =~ m{facet$} or exists $+{facet} ? "facet" : "";
    $arg =~ s{facet$}{};
=begin
    print "\n\n\n========================================================";
    print "\n========================================================";
    print "\n========================================================\n";
                                print "executing $cached_instances{$arg . $isfacet} \n";
                                 print "\n========================================================";
                                 print "\n========================================================\n";
                                print "========================================================\n\n\n";
= cut
    return $cached_instances{$arg . $isfacet};
}

$mainregexdefs =~ s{
    [(][?]&&(?<nm>\w+?(facet)?+)[)]
}{
    (??{instantiate_cached_instance("$+{nm}")})
}sxxg;

$mainregexdefs =~ s{
    [(][?](&{1,3}\+?+)(\w+?)[)]
}{
    (?&$2)
}sxxg;



$mainregexdefs =~ s{
    [(][?](&{1,3})(\w+?)(facet|(?<facet>facet)?+<(?<name>\w+)>)[)]
}{
    if(not $+{name}) {
        dofacetsubreplacements($1, $2)
    } else {
        dotemplatesubreplacements($1, $2, $+{name}, $+{facet})
    }
}sxxge;

=cut

=begin

$mainregexdefs =~ s{
    [(][?]{call(.*?)}[)]
}{
    (?(R&facet)(?{callfacet$1})|(?{call$1}))
}sxxg;



while(my($k, $v) = each %cached_instances) { 
    use if $ARGV[1], re => qw(Debug EXECUTE);

    my $body = $mainregexdefs;

    my $prefix = $entryregex ne $k ? "" : "^";

    my $postfix = "";

    $body = $v =~ s{^[(][?]<(\w+?)>}{(?<${k}entry>}r;

    print "$body\n==========\n";

    $postfix = "entry" if(defined $1);

    $body = $mainregexdefs =~ s{\Q$v\E}{$body}rs;

    $cached_instances{$k} = qr{(*F)($body)|$prefix(?&$k$postfix)}sxxn;

    my $facet = dofacetsubreplacements("$k$postfix");

    $cached_instances{$k . "facet"} = qr{(*F)($body)|$prefix$facet}sxxn;
    
    #$cached_instances{$k . "facet"} = qr{(*F)($body)(?<facet>(?&${k}$postfix))|$prefix(?&facet)}sxxn
}

print $cached_instances{$entryregex};

=cut

#print "$mainregexdefs\n";

#$mainregexfinal = "((*F)$mainregexfinal)^(?&$entryregex)\$";

#if(not $matchinperl) {
#    startmatching($subject, $mainregexfinal, basename($ARGV[-1]));
#    exit;
#}

#exit if(not $matchinperl);

startmodule(basename($ARGV[-1])) if(defined &startmodule and not $nested);

#my $matchprototype = qr{(?(DEFINE)$mainregexdefs)^((??{set {"decls" => "extrnl"}})(?&abstdeclorallqualifs)(??{unset {"decls" => "extrnl"}}))}sxxn;
#my $matchtype = qr{(?(DEFINE)$mainregexdefs)(?&abstdeclorallqualifs)}sxxn;

=begin
sub obtainvalbyidentifier {
    my $fnnamr = $_[0]{"ident"};
    #my $fnnm = $fnnamr =~ s{::}{//}gr;
    #print $fnnm . "\n";
    if(open my $fh, '<', "$fnnamr.c") {

        my $subject = do { local $/; <$fh> };

        close $fh;

        $subject =~ $matchprototype;
    }
    #warn $!; 
    #use re qw(Debug EXECUTE); 

    my $origid = $fnnamr;

    $fnnamr =~ s{_}{[_:.]}g;

    $proxy =~ qr{
        \s$fnnamr:\s.*?\n[^\n\r]+?\s(?<type>\w++)\s
    }sxxn;

    print "$+{type}\n";

    if($+{type}) {
        my $declextrnl = "extern $+{type} $origid;";
        print $declextrnl . "\n";
        $declextrnl =~ $matchtype
    }

}
=cut

#use Term::ANSIColor qw(:constants);

#$isnested = 1;

if(not $isnested)
{
    #my $i = 2;
    use if $ENV{'DEBUG'}, re => qw(Debug EXECUTE); 
    #while(1) {
    #require "extractfns.pm";
    if($ENV{'REPLAY'}) {
        @typedefidentifiersvector = eval { require $ENV{'REPLAY'} . ".txt"}
    }
    while(1) {
        $regexfinal = qr{(?(DEFINE)$mainregexdefs)^(*COMMIT)(?&$entryregex)*+$}sxx;
        eval {$subject =~ m{$regexfinal}sxx};
        if($@) {
            warn $@;
            undef $facet
        } else {
            last
        }
    }

    if($ENV{'RECORD'}) {
        open my $out, '>', "$ENV{RECORD}.txt" or die "error opening $filename: $!";
        $Data::Dumper::Terse = 1;
        CORE::print $out Dumper @typedefidentifiersvector;
        close $out;
    }

=begin
        my $fnname = basename($ARGV[$i], ".c");

        $filename = $fnname . ".c";

        open my $fh, '<', $filename or die "error opening $i: $!";

        $subject = do { local $/; <$fh> };

        close $fh;

        $currentprint = $originalprint;

        #$oldfh = select(STDERR);

        print $fnname . "\n";

        #use if $ARGV[1], re => qw(Debug EXECUTE); 

        my $init;

        my $stackdecl;

        $proxy =~ m{\b$fnname\b[^\n]++
                    (\n\s++;(?<name>\S++)\s++(\S++)\s++(?<at>\S++)[^\n]++(?{
                        $at = $+{at};
                        my $name = $+{name};

                        if($at =~ m{^[\-\+]?+([0-9a-fA-F]++)$}) {
                            $at = $1;
                            print "$name => $at\n";
                            if($subject =~ s{\b$name\b}{(*$&)}sxxng) {
                                $init = $init . "\n  $name = 1[&stack] - 0x$at;";
                                $lastvalid = $at;
                            }
                        }
                    }))*+}sxxn;

        $stackdecl = "\n  unsigned char stack[0x$lastvalid];";

        if($lastvalid) {

            $subject =~ s{;\n\n}{;\n$init\n\n}sxxn;
            $subject =~ s{\{}{\{$stackdecl}sxxn;
        }

        print $subject . "\n";

        ++$i
=cut
   # }
}

exit;

#}

=for comment

sub recovery_mode() {
    my $failedat = pos();
    my $subslice = substr $subject, pos(), 30;

    $subslice =~ s{\R}{ }g;

    print "Failed to match around: $subslice.\n";

    print {$fh} $failedat;

    {local $,='\n';print @typedefidentifiersvector}

    open my $fh, '>', $filename or die "error opening $filename: $!";
    print {$fh} $file;
    close $fh;
}

=cut

#loadregex();

#print $&;

sub isfacet {
    use Data::Dumper;
    print "facet \tchecking -> " . $facet . "\n";
    return $facet#ref $-{facet} ne ARRAY
}

sub replay {
    my @trace = @{$_[0]};
    if(not @trace) {
        return;
    }
    foreach my $event (@trace) {
        my $evntnm = (keys %$event)[0];
        call($evntnm, $$event{$evntnm});
    }
}

sub call {
    #print Dumper(\%+);
    my $funcnm = shift;
    my $captures = {%+};

    eval {@$captures{keys %{$matches[-1]}} = values %{$matches[-1]}} if (scalar @matches);

    my $subslice = substr $subject, pos(), 10;

    $subslice =~ s{\R}{ }g;

    use Data::Dumper;
    use POSIX;

    my $printer = $recording ? \&print : \&print2;

    $printer->(strftime ("%F %T", localtime time) . " capture: " . $subslice . "\n");
    $printer->($recording . " " . $funcnm . "\n");
    $printer->(Dumper(\$captures));
    
    if($recording) {
        #eval {
            print "$recording pushing to " . scalar @savedcallouts . "\n";
            push @{$savedcallouts[-1]}, {$funcnm => {%$captures}};
            print "success\n";
        #};
        return
    }
    return callcommon($funcnm, { %$captures }, $recording)
}

sub call2 {
    return callcommon(shift, {%{$matches[-1]}}, 0)
}

sub callcommon {
    #print Dumper(\%+);
    my $funcnm = shift;
    my $captures = shift;
    my $facet = shift;

    #print "instancing " . $funcnm . "\n";

    #my $argsin = shift;

    #print Dumper $argsin;

    #@$captures{keys %$argsin} = values %$argsin;

    #print "facet -> $facet\n";
    
    #my $cond = ($entryregex =~ m{facet$} or not $facet);
    #return unless($entryregex =~ m{facet$} or not $facet);
    #require re;
    #re->import('debug') if($_[1] eq "Ptr64ToPtr");
    #select($oldfh) if($_[1] eq "Ptr64ToPtr");
    #my @arr = @_;
    
    #foreach my $i (@arr) {
    #    print $i . "\n";
    #}
    my $res;
    eval {
        if(not $facet) {
            $res = $funcnm->($captures) 
        }
    };

    callout($funcnm, $captures) if(defined &callout and not $facet);
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

    my $closure = $+{closure};

    #push @arrayoftemplates, $templatetoreplace unless ($templatetoreplace ~~ @arrayoftemplates);

    while(1) {
        #my $arg = getnext($args);

        $args =~ s{^(\s*+$inpar\s*+|\s*+(?<arg>\S*?)(\s*+\bas\b\s*+(?<alias>\w*))?+\s*+($|,))}{}sxx;

        #print $+{inpar} . "\n\n";

        my $inpar = $+{inpar};

        my $arg = $+{arg};

        my $alias = $+{alias};

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

        print $body . "\n";

        die "incorrect invocation for $param" if(not $argident);

        die "expr as arguments not supported currently" if($inpar);

        print $param . "->" . $argident . " as $alias\n";

        $body =~ s{(\(\?&{1,2})$param(facet)?+((?<inargs><(?<args>[^<>]++|(?&inargs))*+>)?+)\)}{
            if($arg) {if(not $argcallout) {
                if($+{inargs} and $alias) {
                    $1 . $argident . ($isfacet // $2) . $3 . "=$alias)" . $argqualifs ;
                } else {
                    $1 . ($alias ? $alias : $argident) . ($isfacet // $2) . $3 . ")" . $argqualifs ;
                }
            } else {
                "(" . $argqualifs . "?C" . $argcallout . ")" ;
            } }else {
                "()"
            }}eg;

        #$body =~ s{\\g\{\b$param(facet)?+\b\}}{\\g{$argident}}g;}

        $body =~ s{@\b$param(facet)?+\b}{($alias ? $alias : $argident)}ge;

        print "matched at\n";

        $body =~ s{\b$param(facet)?+\b\s*+\bas\s*+\w*}{"$argident$1$argqualifs as" . ($alias ? " $alias" : "")}ge;

        print $& . "\n";

        #print $str;
    }

    if($name) {
        $body =~ s{(?<=\(\?<)\Q$template\E\b}{$name}e;
    } elsif(not $doubleref) {
        $body =~ s{\(\?<\Q$template\E\b.*?>}{(};
    }
    if($isfacet) {
        $body = dofacetreplacements($body, $template);
    } else {
        #my $bodyoriginal = $body;
        #dofacetreplacements($body);
        #$body = "(?(DEFINE)" . $body . ")" . $bodyoriginal;
    }

    print "done:\n";

    print $body . "\n";

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

    $mainregexfinal = $mainregexfinal . $regexfilecontent;

    return;

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
                                print "substituting $& \n";
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
            print "$parnnm -> " . $bodyorig . "\n";
            "((*F)" . dofacetreplacements($bodyfacet) . "|" . $parnnm . addfacetdefines($bodyorig) . "))"
            }sxxge;

        return $_[0]
    }

    addfacetdefines($regexfile);
=cut   
    #$regexfile =~s/(?<!<)#restrictoutsidefacet\b>/>(?(?{$facet})/g;

    #$regexfile =~s/\(\?<#restrictoutsidefacet>/((*F)/g;

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
=cut
    sub dofacetreplacements {
        my $ret = $_[0];

        $ret =~s/\Q(?(<facet>)\E/(?(?{1})/g;

        #$_[0] =~s/(\(\?C&{1,2})(?!\S+?facet)(\S+?)\s*(<(?<args>[^()<>]*?)>)?\)/$1$2facet$3)/gs;

        #$regexfilecontent =~s/\(\?\?C(\d++)\)/(?C$1)/g if(not $matchinperl);

        $ret =~s/([(]\?([(]C)?+&(&?+\w+?))(facet)?+\b/$1facet/g;

        #$regexfilecontent =~s/([(]\?)?+[(]<(?>\w+?)(facet)?+>[)]/$1(<$2facet>)/g;

        #$regexfilecontent =~s/[(]\?&(?>\w+?)\#nofacet[)]/(?&$1)/g;

        #$regexfilecontent =~s/[(]\?[(]<(\w+?)#nofacet>[)]/(?(<$1>)/g;

        #$ret =~s{([(]\?[(]?+)<(\w+?)(facet)?+>}{$1<$2facet>}g;
        
        #$regexfilecontent =~s/(\(\?<\w+)>/$1facet>/g;
        
        return $ret
    }
=begin
    sub dofacetsubreplacements {
        my $identifier = $_[1];
        my $prefix = $_[0];

        #return "(?(DEFINE)(?<facetsub>(?<facet>)$actual))(?&facetsub)";
        return "(((?{++\$facet})|(?{--\$facet}))
                (?$prefix$identifier)
                ((?{--\$facet})|(?{++\$facet})))";
    }
=cut

    sub dotemplatesubreplacements {
        my $identifier = $_[1];
        my $prefix = $_[0];
        my $name = $_[2];
        my $isalsofacet = $_[3];

        #return "(?(DEFINE)(?<facetsub>(?<facet>)$actual))(?&facetsub)";

        return "(((?{++\$$name})|(?{--\$$name}))
                (?$prefix$identifier)
                ((?{--\$$name})|(?{++\$$name})))";
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

sub argsappend {
    #my %matches = %+;

    (values %{$savedcallouts[-1][-1]})[0]->{'argument' . $argcounter++} = $^N
}

sub argsreset {
    $argcounter = 0;
}

sub regendinner {

    my @reginner = @{$savedcallouts[-1]};

    #print Dumper @savedcallouts;

    pop @savedcallouts;

    #push @{$savedcallouts[-1]}, {"regaddquantif" => {%+}} 
    (values %{$reginner[0]})[0]->{"regquantif"} = $+{quantifiers} if($+{quantifiers});

    @{$savedcallouts[-1]} = (@{$savedcallouts[-1]}, @reginner);
}

=begin

sub regchecklookaround {
    my @reginner = @{$savedcallouts[-1]};

    #print Dumper @savedcallouts;

    pop @savedcallouts;

    (values %{$reginner[0]})[0]->{"conditional"} = "1";

    #push @{$savedcallouts[-1]}, {"regcondlookaround" => {%+}};

    @{$savedcallouts[-1]} = (@{$savedcallouts[-1]}, @reginner);
}
=cut

sub regbeginsubcall {
    push @{$savedcallouts[-1]}, {regbeginsubcall => {
        "name" => $+{callee}
    }};
}

sub regbranch {

    my @reglast = @{$savedcallouts[-1]};

    my @reglastlast = @{$savedcallouts[-2]};

    #splice @savedcallouts, -2, 1;

    pop @savedcallouts;
    print scalar @savedcallouts . "\n";
    #exit;
    #pop @savedcallouts;

    #print Dumper \@reglast;
    #print Dumper \@reglastlast;

    #push @reglastlast, {"regbranch" => \$reglast[0]};

    #@{$savedcallouts[-1]} = (@reglastlast, @reglastlast);

    @{$savedcallouts[-1]} = ({"regbranch" => \$reglast[0]}, @reglastlast, @reglast)
    #\$reglast[0]}], @reglastlast, @reglast)
}

sub regend {
    my @reglast = @{$savedcallouts[-1]};

    pop @savedcallouts;

    #print Dumper \@reglast;

    @{$savedcallouts[-1]} = (@{$savedcallouts[-1]}, @reglast);
}

sub startrecord {
    return qr{((?{set2 {'savedcallouts' => []}})|(?{unset2 'savedcallouts'})(*F))}
}

sub stoprecord {
    return qr{((?{unset2 'savedcallouts'})|(?{set2 {'savedcallouts' => []}})(*F))}
}

$subject = $mainregexfinal;

print  $mainregexfinal;

use if $ENV{'DEBUG'}, re => qw(Debug EXECUTE);
#call "startrecording";
push @savedcallouts, [];
++$facet;
if(eval {$mainregexfinal =~ m{(?(DEFINE)$metaregexfilecontent)
    ^(?&regex)$
}sxx}){
    print "success\n"
}
if($@) {
    warn $@;
    undef $facet
}
undef $facet;
my @regexbindings = @{$savedcallouts[-1]};
pop @savedcallouts;

print Dumper \@savedcallouts;

my $entryindex;

sub findgroup {
    my $nametofind = $_[0];
    while (my ($index) = each @regexbindings) {
        #print $index . "\n";
        my %elem = %{$regexbindings[$index]};
        my $elemname = (keys %elem)[0];
        my %elembody = %{$elem->{$elemname}};
        #print Dumper $elembody{"name"};
        if($elemname eq "regbeginsub" and $elembody{"name"} eq $nametofind) {
            last
        }
    }
    return $index;
}

$entryindex = findgroup $entryregex;

print $entryindex . "\n";

#call "stoprecording";
=begin
#opnumber => stringpos

my @stackoperations = [];

my $currindex = $entryindex;

my $currposatsubject;

#\stackoperations

my @groupsstack = [];

#matches
#callop

my @callstack = [];

my $entitlements;

sub fillforindex {
    my %elem = %{$regexbindings[$_[0]]};
    my $elemname = (keys %elem)[0];
    my %elembody = %{$elem->{$elemname}};

    return ({%elem}, $elemname, {%elembody})
}

for(;1;++$currindex) {
    my (%elem, $elemname, %elembody) = fillforindex $currindex;
    push @stackoperations, {$currindex => $currposatsubject};
    given($elemname){
        when(["regbeginsub", "regbegingroup"]) {
            push @groupsstack, \$stackoperations[-1];
        }
        when("regcall") {
            push @callstack, {\$stackoperations[-1] => {%matches}};

            $currindex = findgroup($elembody{"callee"});

            @matches{@entitlements} = @matches{@entitlements} if($elembody{"square"});

            (%elem, $elemname, %elembody) = fillforindex $currindex;

            my @newentitlements = map {$_->{entitlement}} @{$elembody{undef}};

            @entitlements = (@entitlements, @newentitlements);
        }
        
    }

    #check quantifs

    
}

=cut

startmetaregex($entryregex, \@regexbindings, $subject) if(defined &startmetaregex and not $nested);
#startmatching($subject, $mainregexfinal, basename($ARGV[-1]), $entryregex);
#exit;


sub replayrecord {
    if(not $recording) {
        foreach my $hash (@{$savedcallouts[-1]})  {
            
            my $funcnm = (keys %$hash)[0];
            print2 "$funcnm replaying \n";
            print2 Dumper $hash;
            callcommon((keys %$hash)[0], (values %$hash)[0], 0)
            
        }
    } else {
        #print "replay merging\n";
        #print Dumper $hash;
        @{$savedcallouts[-2]} = (@{$savedcallouts[-2]}, @{$savedcallouts[-1]})
    }
}

1