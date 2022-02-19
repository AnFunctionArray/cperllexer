$isnested = 1;

use re 'eval';


BEGIN{push @INC, "./misc"};
BEGIN{push @INC, "./regexes/supplement"};

$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $subject = do { local $/; <$fh> };

close $fh;

require './regexes/parse.pl';

#use re => qw(Debug EXECUTE);

while ($subject =~ m{
    [^{}]*\b(?<fnname>\w++)(?=\s*+[(]).*?\n\}
}sxxng) { 
    #print $& . "\n";
    my $fndef = $&;
    my $fnname = $+{fnname};
    #print $fnname . "\n";
    open my $target, '>', "$fnname.c" or die "error opening $fnname: $!";

    print $target $fndef;

    close $target
}

exit;