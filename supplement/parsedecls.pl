$isnested = 1;

use re 'eval';


BEGIN{push @INC, "./misc"};
BEGIN{push @INC, "./regexes/supplement"};

$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $subjectoutter = do { local $/; <$fh> };

close $fh;

$filename = $ARGV[1];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $proxy = do { local $/; <$fh> };

close $fh;

#require './regexes/parse.pl';

#use re => qw(Debug EXECUTE);

$declsout;

while($subjectoutter =~ m{(?<target>DAT_0*+(?<addr>[0-9a-fA-F]++))}g){
    my $addr = $+{addr};
    my $target = $+{target};
    if($proxy =~ m{:0*+$addr\s*+(((?!:)\S)++):}i) {
        
        print $1 . "\n";    

        my $toreplace = $1;

       $subjectoutter =~ s{$target}{$toreplace}g;

    } else {
        print "warn " . $target . "\n";
        $subjectoutter =~ s{$target}{0x$addr}g;
    }
}

$filename = $ARGV[2];
open my $fh, '>', $filename or die "error opening $filename: $!";

print $fh $subjectoutter;

close $fh;

exit;

=begin
return;

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

    print $init;

    if($lastvalid) {

        $subject =~ s{;\n\s*\n}{;\n$init\n\n}sxxn;
        $subject =~ s/\{/\{$stackdecl/sxxn;
    }

    #print $subject . "\n";

    #close $target
    $subject
=cut