$isnested = 1;

use re 'eval';


BEGIN{push @INC, "./misc"};
BEGIN{push @INC, "./regexes/supplement"};

$filename = $ARGV[1];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $subject = do { local $/; <$fh> };

close $fh;

#require './regexes/parse.pl';

#use re => qw(Debug EXECUTE);

while ($subject =~ m{
    [^{}]*\s++\b(?<fnname>(?<identifierraw>\b(?>[_a-zA-Z](?<letter>[_a-zA-Z0-9])*+)\b(::(?&identifierraw))?+))(?=\s*+[(]).*?\n\}
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