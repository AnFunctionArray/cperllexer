$isnested = 1;

use re 'eval';


BEGIN{push @INC, "./misc"};
BEGIN{push @INC, "./regexes/supplement"};

$filename = $ARGV[1];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $subjectoutter = do { local $/; <$fh> };

close $fh;

$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $proxy = do { local $/; <$fh> };

close $fh;

#require './regexes/parse.pl';

#use re => qw(Debug EXECUTE);

$subjectoutter =~ s{
    [^{}]*\s++\b(?<fnname>(?<identifierraw>\b(?>[_a-zA-Z](?<letter>[_a-zA-Z0-9])*+)\b(::(?&identifierraw))?+))(?=\s*+[(]).*?\n\}
}{
    
    #print $& . "\n";
    my $subject = $&;
    my $fnname = $+{fnname};
    #print $fnname . "\n";
    #open my $target, '>', "$fnname.c" or die "error opening $fnname: $!";

    #print $target $fndef;

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

    print $init;x

    if($lastvalid) {

        $subject =~ s{;\n\s*\n}{;\n$init\n\n}sxxn;
        $subject =~ s/\{/\{$stackdecl/sxxn;
    }

    #print $subject . "\n";

    #close $target
    $subject
}sxxnge;

$filename = $ARGV[1];
open my $fh, '>', $filename or die "error opening $filename: $!";

print $fh $subjectoutter;

close $fh;

exit;