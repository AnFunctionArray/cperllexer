#!/usr/bin/perl

use re 'eval';
use threads;
use threads::shared;
#use Clone 'clone';

my @threads;
my $donework :shared;
#my $nthreads :shared = 0;
our (@matches);

BEGIN{push @INC, "."};

use Thread::Queue;

our @typedfsmap : shared;

$q = Thread::Queue->new();

$qtypdfs = Thread::Queue->new();

my @sharedarr;
my @sharedarrcommon : shared;

my $shouldend : shared;

my $qprim : shared;

#my @{sharedarr} : shared;

#$q->limit = 2147483;

#my $condinput : shared;
my $condexit : shared;
my $scopevarsdone : shared;
$identstoidmap = shared_clone({});
my $identsdone : shared = shared_clone({});
my $identwaiters : shared = shared_clone({});

$nfilescopesrequested = 0;

#use re qw(Debug EXECUTE);

do "typename.regex.pl";
do "primexpr.regex.pl";
do "faster.regex.pl";

use List::Util qw(max);

use experimental 'switch';

use File::Basename;

$genml = $ENV{'GENMLJSON'};

@flags = ();

$maxtimoeut = $ENV{'TIMEOUTWAIT'} // 2;
$minlen = $ENV{'MINQUEUECOUNT'};
$debug = $ENV{'DEBUG'};
$silent = $ENV{'SILENT'};
$lineonly = $ENV{'LINEONLY'};
$maxthreads = $ENV{'MAXTHREADS'};

#my sub Dumper {"\n"}
#use Data::Dumper;
my sub print {CORE::print(@_) if( 1 )}
my sub print2 {CORE::print(@_) if(1)}
sub print3 {CORE::print(@_) if( 1)}
sub Dumper2  {use Data::Dumper; CORE::print(Dumper(@_)) }
my sub Dumper  {use Data::Dumper; Dumper(@_) if( 1 )}
my sub strftime  {use POSIX; strftime(@_) if( 1 )}

sub push2 {
    my @args = @_;
    if ($debug) {
        if(@{$args[0]} ~~ @flags) {
            print "pushing to flags\n";
            print Dumper $args[1];
            print Dumper \@flags;
        }
    }
    push @{$args[0]}, $args[1];
}

sub pop2 {
    my @args = @_;
    if ($debug) {
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
        } else {
            print "popping\n";
            print Dumper \$args[0]->[-1];
        }
    }
    pop @{$_[0]};
}

sub existsflag {
    my $flag = $_[0];
    my $exclusions = $_[1];
    my @flags = @{$_[2] // \@flags};
    print "exists flag $_[0]\n";
    foreach my $val (reverse @flags) {
        return 1 if(exists $val->{$flag});

        foreach my $key (keys %{$exclusions}) {
            return 0 if (exists $val->{$key});
        }
    }

    return 0
}

print Dumper @ARGV;

sub endfndef {

}

sub startpotfndef {
    #print2 "nthr $nthreads\n"; 
    #preparethread() if(defined &preparethread);
    my $slice = substr $subject, pos();
    print2 "pos is " . pos() . "\n";
    #my $waitthread = threads->create(\&waitforthread) if(defined &waitforthread);
    if($maxthreads > 0) {
        while(1) {
            sleep(0);
            for my $i (0 .. $maxthreads - 1) {
                if(eval { !$threads[$i]->is_running } or $@) {
                    $threads[$i] = threads->create(\&execmain, 1, $slice, scalar(pos()));
                    goto ends;
                }
            }
        }
    }
    else {
        for my $i (0 .. $#threads) {
            if(eval { !$threads[$i]->is_running } or $@) {
                $threads[$i]->join();
                $threads[$i] = threads->create(\&execmain, 1, $slice, scalar(pos()));
                goto ends;
            }
        }

        push @threads, threads->create(\&execmain, 1, $slice, scalar(pos()));
    }
ends:
    #lock($nthreads);
    #++$nthreads;
    #waitforthread() if(defined &waitforthread);
    #$waitthread->join() if(defined &waitforthread);
    lock($donework); 
    cond_wait($donework) until $donework == 1;
    $donework = 0;
}

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

$filename = "primexpr.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $mainregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = "faster.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $fasterregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = "fasterlight.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $fasterlightregexfilecontent = do { local $/; <$fh> };

close $fh;

$filename = "regex.regex";
open my $fh, '<', $filename or die "error opening $filename: $!";

my $utilregexfilecontent = do { local $/; <$fh> };

close $fh;

my $mainregexfinal = "";

my $defaultidentifiers = {};

$typedefidentifiersvector = [$defaultidentifiers];

my @typedefidentifiersvectorcopies = ();

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

$mainregexdefs = $mainregexfinal;

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

#startmodule(basename($ARGV[-1]) . "_base") if(defined &startmodule and not $nested);

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

sub findnearest {
    my $currpos = $_[0];
    my @idposes = @_[1];
    my $closest = $_[2];

    my $shortdst = 2147483647;
    my $closest = -1;
    my @returnarr = ();

    #Dumper2(\@idposes);

    foreach my $idpos (@idposes) {
        #CORE::print ("idpos " . $idpos . "\n");
        #return $idpos;
        if ($closest && abs($currpos - $idpos) < $shortdst) {
            $shortdst = abs($currpos - $idpos);
            push @returnarr, $idpos;
        }
        elsif($idpos <= $currpos) {
            push @returnarr, $idpos;
        }
    }

   # CORE::print ("idpos " . $closest . "\n");

    return @returnarr
}

sub getidtostor {
    my $ident = $_[0];
    my $currpos = pos();
    my $lastid = -1;

    {
        #return -1 if (not exists $identstoidmap->{$ident});
        lock @{$identstoidmap->{$ident}};

        #CORE::print ("$ident " . Dumper2(\@{$identstoidmap->{$ident}}) . "\n");

        foreach my $id (@{$identstoidmap->{$ident}}) {
            #CORE::print ($id->[0] . " against " .  $currpos . "\n");
            last if ($id->[0] > $currpos);
            $lastid++;
        }
    }
    #CORE::print ("lastid $lastid\n");
    return $lastid;
}

sub broadcastid {
    my $ident = $_[0];
     my $idtosignal = $_[1];
     my $locker;
     #CORE::print("dump " . $ident ."\n");
    {
        #return -1 if (not exists $identstoidmap->{$ident});
        lock @{$identstoidmap->{$ident}};

        #CORE::print ("signalling over " . $idtosignal . "\n");

        lock @{$identstoidmap->{$ident}->[$idtosignal]};

        $identstoidmap->{$ident}->[$idtosignal]->[1] = 1;

        cond_broadcast(@{$identstoidmap->{$ident}})
    }

    #CORE::print ("signalling " . $idtosignal . "\n");

    return $idtosignal
}

sub waitforid {
     my $currpos = pos();
    my $ident = $_[0];
    my @identscur;
    my @idtosignal;
    my @locker;
    
    while(1)
    {
        my $areallset = 1;
        my $nset = -1;

        {
            #lock $identstoidmap->{$_[0]}->{$idtosignal};
            {

                lock $identstoidmap;
                return -1 if (not exists $identstoidmap->{$ident});
            }

            lock @{$identstoidmap->{$ident}};


                #CORE::print("$currpos waitin : $ident, " . Dumper2(\@{$identstoidmap->{$ident}}));

                foreach my $ind (@{$identstoidmap->{$ident}}) {
                    lock @{$ind};
                    #CORE::print ("check map $ind \n");
                    last if(not ($ind->[0] <= $currpos));
                    $areallset = $areallset && $ind->[1];
                    last if (not $areallset);
                    ++$nset;
                }

            return $nset if($areallset);

            #CORE::print ("waitin on " . $ident . "\n");

            if(!cond_timedwait(@{$identstoidmap->{$ident}}, time() + $maxtimoeut)) {

                CORE::print("$currpos waitin timed out on : $ident, \n");
                Dumper2(\@{$identstoidmap->{$ident}});
                sleep(10000)
            }
        }
    }

    #return $idtosignal;
}

my $tryingagain: shared = 0;

#if(not $isnested)
#{

    #my $i = 2;
    #use re qw(Debug EXECUTE); 
    #while(1) {
    #require "extractfns.pm";
    if($ENV{'REPLAY'}) {
        @typedefidentifiersvector = eval { require $ENV{'REPLAY'} . ".txt"};
    }
    my $flind = 1;
    my $execmainregshared = $mainregexdefs; #eval { #use re qw(Debug EXECUTE);
         #"(?(DEFINE)$mainregexdefs)\\G(?&cprogram)" };
    sub execmain {
        #use re qw(Debug EXECUTE);
        #CORE::print("here1\n");
        $regexstr = $_[0];
        $regex = qr{(?(DEFINE)$regexstr)\G(?&cprogram)}sxx;
        $subject = $_[1];
        $fasterregex = $_[2];
        $threadid = $_[3];
        #CORE::print("working - " . Dumper(\@{$$tmp}). "\n");

        #$delegatetaggables = 0;

        #inittypenameconstants();

        startmodule(basename($ARGV[-1]) . "_" . $threadid) if(defined &startmodule);

        #eval {registerthread(scalar($threadid))};

        #CORE::print("working - " . Dumper(\@{$$tmp}). "\n");

        my $tmp;
         my @item;

         my $tryingagainlocal;

         #CORE::print("here\n");

        while (1) {
            if (not $tryingagainlocal)
            {
                #lock ($qprim);
                #lock (@sharedarrcommon);
                #cond_wait ($condinput);

                # $silent = 0;
                

                #CORE::print (Dumper(\$tmp) . "or not\n");

                #$silent = 1;

                #cond_wait (@sharedarr) until scalar(@sharedarr);

                #$silent = 0;

                while (!defined($tmp = $q->dequeue())) {
                   CORE::print(" ending\n");
                    endmodule() if(defined &endmodule);
                    return;
                }

                #CORE::print("working - " . Dumper(\@{$$tmp}). "\n");

                #$silent = 1;

                #$silent = 1;

                @item = @{$tmp};

                #cond_signal ($q)
            }
            #CORE::print("here\n");

             #$silent = 0;

                #CORE::print("working - " . Dumper(\@item). "\n");

                #$silent = 1;
=begin
            use Devel::Peek;
            my $index = scalar($item[0]);
            my %typdefshash = {};
            foreach my $i (0..$index) {
                #CORE::print("j - $index\n");
                my @currelem = @{$qtypdfs->peek($i)};
                #CORE::print("$start - working - " . Dumper(\@currelem). "\n");
                $typdefshash{$currelem[1]} = $currelem[0]
            }

            $typedefidentifiersvector = [{%typdefshash}];
=cut
            my $start = $item[1];
            $silent = 0;
            #CORE::print("$index - working - " . Dumper(\%typdefshash). "\n");
            $silent = 1;
            $nextpos = scalar($item[2]);
            my $ntyps = scalar($item[0]);
            #my $ntomatch = scalar($item[3]);
            #lock $typedefidentifiersvector;
            $typedefidentifiersvector = [$ntyps ? {%{$qtypdfs->peek($ntyps - 1)}} : {}]; #[{}];
            #CORE::print (Dumper(\@{$typedefidentifiersvector}));
            #CORE::print ("$start\n");
=begin
            for my $i (0..$ntyps) {
                my $elem = $qtypdfs->peek($i);

                $typedefidentifiersvector{$elem->[0]} = $elem->[1]
            }
=cut
            #$identstoidmap = $item[4];

            #print2("dumping typedefs - $nfilescopesrequested\n");
            
            #$silent = 0;
            #CORE::print(Dumper(\@item));
            #$silent = 1;
            #consumefilescopes(scalar($isblockntypstosyncc - 1)) if ($isblockntypstosyncc && defined &consumefilescopes);

            #CORE::print (Dumper(\@item) . "above\n");

            #Copy registered typedefs so far
            @flags = ();
            @matches = ();
            @savedcallouts = ();
            $recording = 0;

            $unmask = 1;
            $hasdelegates = 0;

            #initthread(scalar($start)) if(defined &initthread and $nthread);

            #eval { updateavailidents($identstoidmap) };


            $currposlast = $start;

            #CORE::print "$subject\n";

            #exit; 

            if(0) {
                use Time::HiRes qw( time );
                push2 \@matches, {};
                push2 \@savedcallouts, [];
                push2 \@flags, {"outter"=>undef,"opt"=>undef, "nonbitfl"=>undef};
                ++$recording;
                #CORE::print ("matching struct " . $unmask . "s\n");
                #use re qw(Debug EXECUTE);
                my $begin_time = time();
                if (!($subject =~ m{(?(DEFINE)$regex)\G(?&taggableonly)}sxx)) {
                    CORE::print ("3 failed near " . pos($subject) . "\n");
                    exit
                }
                my $end_time = time();
                pop2 \@savedcallouts;
                --$recording;
                #CORE::print("outcawst $&\n");
                #CORE::printf("%.2f\n", $end_time - $begin_time);
                #all('check_stray_struc') if (not eval {exists $matches[-1]{enum}}); 
                eval {broadcast(scalar(1), scalar($nfilescopesrequested))};

                {
                    lock $identsdone;
                    $identsdone->{scalar($nfilescopesrequested)} = undef
                }

                {
                    lock $identwaiters;

                    my @waiters = @{$identwaiters->{scalar($nfilescopesrequested)}};

                    foreach (@waiters) {
                        lock $_;
                        cond_signal $_
                    }
                }
                
                pop2 \@matches;
                pop2 \@flags;
            }
            else {
                #$silent =0;
                #CORE::print ("begin " . Dumper(\$typedefidentifiersvector));
                #$silent =1;
=begin
                my $currposlast = $start;
            my $i = 0;
            while(1) {
                CORE::print ("pos is to after " . $+[0] . "\n");
            if ($i++ >= $ntimes) {
                last unless ($currposlast < $lastpos);
                CORE::print ("pos is last $lastpos\n to pos " . $currposlast . "\n");
                goto tryagain_begin
            }
=cut
            pos($subject) = $start;
            my $lastvalid = $start;
            my $tryingagainlocal;
            my $lasrtypedefobj = {%{$typedefidentifiersvector->[0]}};
            my $i = 0;
            my $entertryagain = 0;
            while(1){
                #if ($i++ >= $ntomatch) {
                last if($lastvalid >= $nextpos);
                #}
tryagain:    #[{}];
                #pos($subject) = $currposlast;
                #@flags = ();
                #@matches = ();
                #@savedcallouts = ();
                #$recording = 0;
                #CORE::print ("matchin...");
                if(!($subject =~ m{$regex}gxxs) || pos($subject) == $lastvalid) {
                    #CORE::print ( "fail" . $+[0] . "__" .$nextpos . "\n" );
                    eval{reset_state()};
                    @flags = ();
            @matches = ();
            @savedcallouts = ();
            $recording = 0;
                    pos($subject) = $lastvalid;
                    $typedefidentifiersvector = [{%{$lasrtypedefobj}}];
                    if ($tryingagainlocal) {
                    lock ($tryingagain);
                    if($tryingagain) {
                        sleep (100000);
                    }
                    CORE::print ("4 failed near " . pos($subject) . " - start - $start\n");
                    
                    $tryingagainlocal = $tryingagain = 1;
                    #$silent = 0;
                    #CORE::print (Dump(\@{$typedefidentifiersvector}));
                    #CORE::print (Dumper2(\@{$typedefidentifiersvector}));
                    #$silent = 1;
                    CORE::print ("retrying...");
                    
                    $regex = eval { use re qw(Debug EXECUTE);  qr{(?(DEFINE)$regexstr)\G(?&cprogram)}sxx; };
                    }
                    $tryingagainlocal = 1;
                    $entertryagain = 0;
                    #CORE::print("currposlast" . $currposlast . "\n");
                    goto tryagain
                }
                #else {
                    $lastvalid = pos($subject);
                    $tryingagainlocal = 0;
                    $lasrtypedefobj = {%{$typedefidentifiersvector->[0]}};
                #}
                CORE::print("$threadid -- $i ==========succes=============\n $&\n==========endsuccess=============\n");
                #pos($subject) = $+[0]
                #CORE::print ("pos is to pos " . $+[0] . "\n");
                #$currposlast = $+[0];
            }
            #}
                #$currposlast = $+[0]
                #CORE::print (Dumper(\@{$typedefidentifiersvector}));
            }

            #CORE::print("==========succes=============\n $&\n==========endsuccess=============\n");

            #exit;

            #CORE::print ("on end - $nfilescopesrequested\n");

            #if (!$isblock) {
            #eval {broadcast(scalar($threadid))};
            #}
        }
    }

    sub parse_filescope_var {
        CORE::print("test\n");
        my $identtosearch = $_[0];
        my $flags = $_[1];
        my $currpos = $_[2];
        my $continue = $_[3];
        my $lastposend = 0;

        if ($flags eq 1) {
            $identtosearch = qr{\bstruct\s++$identtosearch\s++\{}sxx;
        }

        if ($flags eq 2) {
            $identtosearch = qr{\bunion\s++$identtosearch\s++\{}sxx;
        }

        if ($flags eq 3) {
            $identtosearch = qr{\benum\s++$identtosearch\s++\{}sxx;
        }

        if ($flags eq 0) {
            $identtosearch = qr{\b$identtosearch\b}sxx;
        }

        CORE::print("test - $currpos\n");

        pos($subject) = $continue;

        CORE::print("$identtosearch\n");

        while ($subject =~ m{$fasterregex((?&parens)(\s*+(?<block>(?&brackets)))?+|(?<identen>;)|(?&brackets)|$identtosearch)}sxxgsc) {
            CORE::print("test\n");
            my $lastpos = $+[0];

            my $shouldstorelast = $+{identen} || exists $+{block};

            if ($shouldstorelast) {
                #CORE::print (($lastpos) . "\n");
                $lastposend = $lastpos
            }
            else {
                if ($lastposend > $currpos) {
                    CORE::print("end0\n");
                    return scalar(0);
                }

                my $lastpos = pos($subject);

                pos($subject) = $lastposend;
                $subject =~ m{(?(DEFINE)(?<compoundstatement>[\{]))$regex}sxxo;
                return scalar($lastpos);
            }
        }

        CORE::print("testend\n");

        pos($subject) = $currpos;
        CORE::print("end0\n");
        return scalar(0);
    }

    my $initseqlight = $fasterlightregexfilecontent;

    sub push_decl {
        my $lastposend = $_[0];
        my $possibleidentlocal = $_[1];
        lock $identstoidmap;
        if (not exists $identstoidmap->{$possibleidentlocal}) {
            $identstoidmap->{$possibleidentlocal} = shared_clone([[$lastposend, 0]]) 
        }
        else {
            lock (@{$identstoidmap->{$possibleidentlocal}});
            push @{$identstoidmap->{$possibleidentlocal}}, shared_clone([$lastposend, 0]);
        }
    }

    my $initseqlight = qr{(?(DEFINE)$fasterregexfilecontent$fasterlightregexfilecontent)}sxx;

    for (1..$maxthreads) {
        push @threads, threads->create(\&execmain, $execmainregshared, $subject, $initseq, $_);
    }

    push2 \@matches, {};
    push2 \@flags, {"outter"=>undef,"opt"=>undef, "nonbitfl"=>undef};

    my $tryagain = 2;
    my $lasrtypedefobj = {%{$typedefidentifiersvector->[0]}};
    my $lastposcurrlast = 0;
    my $compreg = qr{$initseqlight\G\s*+(?&fasterdecls)}sxx;

    #while(1) {
        {
tryagain_main:
        while($subject =~ m{$compreg}gxxs) {
            #CORE::print($lastposcurrlast . "\n");
            $lasrtypedefobj = {%{$typedefidentifiersvector->[0]}};
            $lastposcurrlast = $+[0];
        }

        if(!($subject =~ m{$}sxxg)) {
            @flags = ();
            @matches = ();
            @savedcallouts = ();
            $recording = 0;
            push2 \@matches, {};
            push2 \@flags, {"outter"=>undef,"opt"=>undef, "nonbitfl"=>undef};
            $typedefidentifiersvector = [{%{$lasrtypedefobj}}];
            if(!$tryagain) {

                $tryagain = 1;
                pos($subject) = $lastposcurrlast;
                goto tryagain_main
            }
            elsif($tryagain++ == 1) {
                use re qw(Debug EXECUTE); 
                $compreg = qr{(?(DEFINE)$finitseqlight)\G\s*+(?&fasterdecls)}sxx;
                #CORE::print("trying again with debug\n" . $initseqlight );
                pos($subject) = $lastposcurrlast;
                goto tryagain_main
            }
            CORE::print ( "test failed" . $lastposcurrlast . "__" .length $subject . "\n" );
            exit -1;
        }
        }
    #}

    if ($nqueues) {
        $q->enqueue(([scalar($lastntypedfs), scalar($lastpos_dispatch), scalar(length $subject) - 1]));
    }

    CORE::print ("joinning for real $lastqueuepoint\n");

    $q->end();

    $_->join for @threads;
    exit

    pop2 \@matches;
    pop2 \@flags;

    $delegatetaggables = 1;
   # sub execmain {
        #while(1) {
=begin
        while($subject =~ m{$initseq((?<ident>;)|(?&parens)\s*+(?<block>(?&brackets))|(?&brackets)|(?<typedef>\btypedef\b))}gxxnsoc) {
            if ($+{ident} or $+{block}) {
                #CORE::print $+[0] . "\n";
                $begin = $+[0]
            }
            if ($+{typedef}) {
                while ($subject =~ m{$initseq
                    (((?=(?<identparens>(?&inparnes)))(?&parens)
                    |(?<identn>(?&identifierpure)))(?&parens)*+\s*+(?<identen>[,;])|(?&brackets))}sxxgsoc) {
                    ${$typedefidentifiersvector[-1]}{$+{identparens} // $+{identn}} = 1;
                    last if ($+{identen} eq ';');
                }
            }
        }
= cut
        #push2 \@savedcallouts, [];
        #++$recording;
        #push2 \@flags, {"skiptaggedbodies"};
        $lastposend = 0;
        #$dirty = 0;
        #$nstackdirty = 1;
        #my $typedefstosync = 1;
        #$nfilescopesrequested = 0;
        $threadid = 0;
        #eval {registerthread(scalar(0))};
        my $lastypedef;
        sub delegatetaggables {
            #CORE::print "delegatetaggables - $delegatetaggables - $nfilescopesrequested\n";
            return 0 if (!$delegatetaggables);

            #lock $q;
            my @arrcp = map { [@$_] } \@typedefidentifiersvector;
            $q->enqueue([@arrcp, scalar(pos()), scalar($nfilescopesrequested), scalar(1)]);
            #CORE::print (pos() . " unmask = 0 for $nfilescopesrequested\n");
            $unmask = 0;
            #cond_signal ($q);
            return 1;
        }
        sub brodc {
            #CORE::print ("broadcasting\n");
            CORE::print("broadcasting - $unmask , $nfilescopesrequested\n");
            eval {broadcast(scalar($unmask), scalar($nfilescopesrequested))};
            {
                lock $identsdone;
                #CORE::print ("broadcasting\n");
                $identsdone->{scalar($nfilescopesrequested)} = undef if ($_[0]);
                #CORE::print ("broadcasting\n");
            }

            {
                my @waiters;
                {
                    lock $identwaiters;

                    #CORE::print ("broadcasting\n");

                    return if (not exists $identwaiters->{scalar($nfilescopesrequested)});

                    @waiters = @{$identwaiters->{scalar($nfilescopesrequested)}};

                    foreach (@waiters) {
                        lock $_;
                        cond_signal $_
                    }

                    delete $identwaiters->{scalar($nfilescopesrequested)};
                }

                #CORE::print ("brosignaling\n");
            }
        }

        sub probequeue {
            #lock $q;
            $q->pending()
        }


        my $typeorqualifsreg = flattentypeorqualif();

        $typeorqualifsreg = qr{(?(DEFINE)(?<typeorqualif>\s*+$typeorqualifsreg\s*+))}sxxn;

        $unmask = 1;
        $delegatetaggables = 0;

        for (1..$maxthreads) {
            push @threads, threads->create(\&execmain, $execmainregshared, $subject, $initseq, $_);
        }
        

         my $begin_time = time();
        my $end_time = time();
        my $nfilescopesrequested;
        my $ndeclsorbodies = 0;
        my $lastntypedfs = 0;
        my $lastqueuepoint = 0;
        while (1) {
            if (!($subject =~ m{$typeorqualifsreg$initseqlight
            (?&parens)\s*+(?<block>(?&brackets))?+|(?<identen>[;])|(?&brackets)|(?&strunus)}sxxgs)) {
end:
                CORE::print ("joinning for real $lastqueuepoint\n");
                #sleep (10000);
                $q->enqueue([scalar($qtypdfs->pending()), scalar($lastqueuepoint), scalar($nfilescopesrequested), scalar($lastpos), scalar($ndeclsorbodies - 1)]);
                $q->end();

                $_->join for @threads;
                exit
            }
            my $lastpos = $+[0];

            my $shouldstorelast = (($+{identen} eq ';') or exists $+{block});

            my $wastypedef = exists $+{typedef};

            #CORE::print ("was typ : $lastpos -$lastypedef \n");
            my %taggables = ("enum" => undef, "struct" => undef, "union" => undef);
                if($shouldstorelast) {
                    #lock $qprim;
                    #lock (@sharedarrcommon);
                    #my @arrcp = map { [@$_] } \@typedefidentifiersvector;
                    #$q->enqueue([@typedefidentifiersvector, scalar($lastposend), scalar($nfilescopesrequested)]);
                    #CORE::print ("$lastposend enqueuing " . Dumper($typedefidentifiersvector) . "\n");
                    #$q->enqueue([$qtypdfs->pending(), scalar($lastposend), scalar($nfilescopesrequested), scalar($lastpos)]);
                    if (!($lastpos - $lastqueuepoint < $minlen)) {
                        #$silent = 0;
                        #CORE::print ("$lastposend enqueuing " . Dumper(\$typedefidentifiersvector) . "\n");
                        #$sileng = 1;
                        $q->enqueue([scalar($qtypdfs->pending()), scalar($lastqueuepoint), scalar($nfilescopesrequested), scalar($lastpos), scalar($ndeclsorbodies - 1)]);
                        #$lastntypedfs = $qtypdfs->pending();
                        if ($typedefs_changed) {
                            $qtypdfs->enqueue(\{ %{$typedefidentifiersvector->[0]} });
                            $lastntypedfs = scalar(@{keys %{$typedefidentifiersvector->[0]}});
                            undef $typedefs_changed;
                        }
                        #push @typedefidentifiersvectorcopies, { %{$typedefidentifiersvector->[0]} };
                        ++$nfilescopesrequested;
                        #CORE::print (("lastpos is : " . $lastpos) . "\n");
                        $lastqueuepoint = $lastpos;
                        $ndeclsorbodies = 0;
                    }

                    #cond_signal(@sharedarrcommon);
                    #goto end
                    use Time::HiRes qw( time );
                    $unmask = 1;
                    $hasdelegates = 0;
                   #CORE::print ("on start -$lastposend\n");
                    my $lastlastpos = $lastpos;
                    pos($subject) = $lastposend;
                    #pos($subject) = $+[0];
                    my $typedeffound;
                    my $taggablefound;
                    #while(1) {
                        my sub push_decl {
                            my $lastposend = $_[0];
                            my $possibleidentlocal = $_[1];
                            lock $identstoidmap;
                            if (not exists $identstoidmap->{$possibleidentlocal}) {
                                $identstoidmap->{$possibleidentlocal} = shared_clone([[$lastposend, 0]]) 
                            }
                            else {
                                lock (@{$identstoidmap->{$possibleidentlocal}});
                                push @{$identstoidmap->{$possibleidentlocal}}, shared_clone([$lastposend, 0]);
                            }
                        }
                        my $posident;
                        my $possibleident;
                        my $wasinside = 0;
                        push2 \@matches, {};
                        push2 \@flags, {"outter"=>undef,"opt"=>undef, "nonbitfl"=>undef};
                        while ($subject =~ m{\G\s*+([_a-zA-Z][_a-zA-Z0-9]*+)\s*+}sxxg) {
                            my $possibleidentlocal = $^N;
                            $posident = $-[0];

                            #CORE::print ("posi ".$possibleidentlocal . "\n");

                            if (exists $taggables{$possibleidentlocal}) {
                                #my $posident = $-[0];
                                if ($subject =~ m{\G\s*+([_a-zA-Z][_a-zA-Z0-9]*+)\s*+}sxxg) {
                                    $possibleidentlocal = $possibleidentlocal . " " . $^N;
                                    #CORE::print("tag setting $possibleidentlocal\n");
                                    if ($subject =~ m{$typeorqualifsreg$initseqlight\G(?&brackets)\s*+}sxxg) {
                                        pos($subject) = $+[0];
                                        push_decl($lastposend, $possibleidentlocal)
                                        #lock %{$identstoidmap->{$possibleidentlocal}};
                                        #lock $identstoidmap->{$possibleident};
                                        #CORE::print("tag setting $possibleidentlocal @ $lastposend\n");
                                        #$identstoidmap->{$possibleidentlocal}->{(scalar($lastposend))} = 0;
                                    }
                                    $matches[-1]{strc} = 1
                                }
                                $taggablefound = 1;
                            }
                            else {
                                
                                if (not checktypeorqualifortypdf(1)) {
                                    $possibleident = $possibleidentlocal;
                                    last
                                }
                            }
                        }

                        my sub process_ident_shallow {
                            if (not exists $+{identnormal}) {
                                $& =~ m{\G[_a-zA-Z][_a-zA-Z0-9]*+}sxx;
                                #$posident = $-[0];
                                $possibleident = $&;
                            }
                            else {
                                #$posident = $-[0];
                                $possibleident =  $+{identnormal};
                            }
                            $posident = $-[0];
                        }

                        if (!$possibleident) {
                            if ($subject =~ m{$typeorqualifsreg$initseqlight\G\s*+(?&ptr)*+((?&rndbrcksdecl)|\K(?<identnormal>(?&identifierpure)))\s*+}sxxg) {
                                process_ident_shallow
                            }
                        }

                        my $istypedef = $matches[-1]{typedefkey};

                        if (!$possibleident and not $taggablefound) {
                            CORE::print ("2 failed near ??" . pos($subject) . "\n");
                            exit
                        }
                        elsif($possibleident) {
                            push_decl($lastposend, $possibleident);
                            #lock %{$identstoidmap->{$possibleident}};
                            #lock $identstoidmap->{$possibleident};
                            #CORE::print("setting $possibleident - $lastposend\n");
                            #$identstoidmap->{$possibleident}->{(scalar($lastposend))} = 0;
                            register_decl({'ident'=>$possibleident, 'typedefkey'=>($istypedef)}, 1);
                            #$qtypdfs->enqueue([scalar($possibleident),scalar(${$typedefidentifiersvector->[-1]}{$possibleident})]);
                        }

                        pop2 \@matches;
                        pop2 \@flags;

                        $subject =~ m{$typeorqualifsreg$initseqlight\G(\s*+(?&parens)\s*+)*+}sxxg;

                        while ($subject =~ m{$typeorqualifsreg$initseqlight\G\s*+,\s*+(?&ptr)*+((?&rndbrcksdecl)|\K(?<identnormal>(?&identifierpure)))\s*+}sxxg) {
                            my $possibleident;
                            process_ident_shallow;
                            push_decl($lastposend, $possibleident);
                            register_decl({'ident'=>$possibleident, 'typedefkey'=>($istypedef)}, 1);
                            #$qtypdfs->enqueue([scalar($possibleident),scalar(${$typedefidentifiersvector->[-1]}{$possibleident})]);
                            $subject =~ m{$typeorqualifsreg$initseqlight\G(\s*+(?&parens)\s*+)*+}sxxg;
                        }

                        #pos($subject) = $+[0];

                        #undef $possibleident;
                        #undef $typedeffound;
                        #undef $taggablefound;
                    #}
                }

            if ($shouldstorelast) {
                #pos($subject) = $lastpos;
                $lastposend = $lastpos;
                ++$ndeclsorbodies;
            }
        }
=cut
        #pop2 \@savedcallouts, [];
        #pop2 \@flags;
        #--$recording;
        #CORE::print $fastersubject . "\n";
        #CORE::print Dumper(\@typedefidentifiersvector) . "\n";
        #exit;
        CORE::print ("joinning\n");

       
        #for my $elem (reverse @sharedarr)
            
        $_->join()  for @threads;
        print2 "joined\n";
        exit;
            if(!$nthread) {
                register_decl_fast();
                $regexfinal //= qr{(?(DEFINE)$mainregexdefs)^(*COMMIT)(?<entry>(?&identifiercompositefast)*+((?&identifiercompositefast)(?&entry))?+)$}sxxo;
                $currregex = $regexfinal;
            } else {
                $regexfinalthread //= qr{(?(DEFINE)$mainregexdefs)^(*COMMIT)(?&compoundstatement)}sxxo;
                $currregex = $regexfinalthread;
            }
            if(eval {$subject =~ m{$currregex}sxx}) {
                CORE::print "succes\n";
            }
            #print2 "subject is $subject";
=begin
            if($@) {
                warn $@;
                undef $facet;
                exit
            } else {
                $filename = $ARGV[$flind++];
                open my $fh, '<', $filename or last;

                $subject = do { local $/; <$fh> };
            }
=cut
        #}
        #print2 "failed at @ " . pos() . "\n";
        if(!$nthread) {
            #print2 "full is $&";
            waitforthreads();
        } else {
            if($@) {
                endmoduleabrupt() if(defined &endmoduleabrupt);
            }
            else {
                dumpmodule() if(defined &dumpmodule);
                endmodule() if(defined &endmodule);
            }
            #decnthreads($nthread);
        }
    #}

    #sub decnthreads {
    #    lock($nthreads);
    #    $nthreads--;
    #}

    #execmain(0, $subject);

    if($ENV{'RECORD'}) {
        $silent = 0;
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
   exit;
#}

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
    my $currpos = scalar(pos());
    #print Dumper(\%+);
    my $funcnm = shift;
    my $captures = {%+};

    @$captures{keys %{$matches[-1]}} = values %{$matches[-1]} if (scalar @matches);

    my $subslice;

    if(not $silent or $lineonly)
    {

        $subslice = substr $subject, $currpos, ($lineonly ? 100 : 25);

        $subslice =~ s{\R}{ }g;
    }

    if($lineonly) {
        CORE::print $subslice . "\n"
    }

    #use Data::Dumper;
    #use POSIX;

    my $printer = $recording ? sub {1} : \&print2;

    $printer->(strftime ("%F %T", localtime time) . " capture@" . pos() . ": " . $subslice . "\n");
    $printer->($recording . " " . $funcnm . "\n");
    $printer->(Dumper(\$captures));
    
    if($recording) {
        #eval {
            print  "$recording " .$funcnm ." pushing to " . scalar @savedcallouts . "\n";
            push @{$savedcallouts[-1]}, {
            $funcnm => {
                matches => {%$captures},
                flags => map {@$_} eval { @flags},
                pos => $currpos
            }};
            print "success\n";
            print Dumper \@{$savedcallouts[-1]};
        #};
        #return
    }
    return callcommon($funcnm, {%$captures}, $recording, \@flags, $currpos)
}

sub call2 {
    return callcommon(shift, {%{$matches[-1]}}, 0, undef, scalar(pos()))
}

sub callcommon {
    #print Dumper(\%+);
    my $funcnm = shift;
    my $captures = shift;
    my $facet = shift;
    my $flags = shift;
    my $currpos = shift;

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
    my $res=not $facet;
    my $out=-1;
    
    if($res) {
        eval {$funcnm->($captures, $flags, \$res, \$out) };
    }
    if($@ or $facet) {
        
        eval {($funcnm . "_universal")->($captures, $flags, \$res, \$out)};
    }

    print2 "not triggered\n" if(not $res);
    eval {callout($funcnm, $captures, scalar($currpos))} if(defined &callout and not $facet and $res);
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

=begin

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
= begin
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

startmetaregex($entryregex, \@regexbindings, $subject) if(defined &startmetaregex and not $nested);
#startmatching($subject, $mainregexfinal, basename($ARGV[-1]), $entryregex);
#exit;
=cut

sub replayrecord {
    if(not $recording) {
        foreach my $hash (@{defined $_[0] ? $_[0][-1] : $savedcallouts[-1]})  {
            
            my $funcnm = (keys %$hash)[0];
            my $record = (values %$hash)[0];
            print2 "$funcnm replaying \n";
            print2 Dumper $hash;
            callcommon((keys %$hash)[0], $record->{matches}, 0, \@{$record->{flags}}, scalar($record->{pos}))
            
        }
    } else {
        #print "replay merging\n";
        #print Dumper $hash;
        if(defined $_[0]) {
            @{$savedcallouts[-1]} = (@{$savedcallouts[-1]}, @{$_[0][-1]})
        } else {
            @{$savedcallouts[-2]} = (@{$savedcallouts[-2]}, @{$savedcallouts[-1]})
        }
    }
}

1