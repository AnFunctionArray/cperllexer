$isnested = 1;

use re 'eval';

use Data::Dumper;


BEGIN{push @INC, "./misc"};
BEGIN{push @INC, "./regexes/supplement"};

$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $proxy = do { local $/; <$fh> };

close $fh;

$filename = $ARGV[1];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $subject = do { local $/; <$fh> };

close $fh;

$filename = $ARGV[2];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $header = do { local $/; <$fh> };

close $fh;


#require './regexes/parse.pl';

#use re qw(Debug EXECUTE);

my $decls;

my @errors;

$subject =~ s{(.*)\b(\w+)\._(?<off>\d+)_(?<sz>\d+)_(.*;)}{

}xxng;

if(0) {

while($proxy =~ m{\.c:(?<line>\d++):(?<column>\d++):\serror:
    (?=.*?(?<anon>'struct\s\(anonymous\sstruct))
    (?=.*?(and|to)\s'(?<target>[^']++)'
    |.*?arithmetic\sor\spointer\stype\sis\srequired
    |expression\sof\sinteger\stype)[^\n\r]*+
    \n(?<firstline>[^\n\r]*+)
    \n(?<secondline>[^\n\r]*+)\n}xxng) {
    my $line = $+{line} - 1;
    my $column = $+{column} - 1;

    my $firstline = $+{firstline};
    my $secondline = $+{secondline};

    push @errors, [$line, $column, $firstline, $secondline];
}

print Dumper @errors . "\n";

foreach my $elem (reverse @errors) {
    my $line = $$elem[0];
    my $column = $$elem[1];
    my $firstline = $$elem[2];
    my $secondline = $$elem[3];


    $secondline =~ m{(?<=\s)~{4,}(?=\s)};

    my $ocsubstr = $firstline, $-[0], length $&;

    print "$line , $column $ocsubstr\n";

    my $secondline = int(int($line) / 8) + $line % 8;

    $line = int(int($line) / 8);

    my $lastpos;

        $subject =~ m{(([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$secondline}
                    ([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$line})(?{$lastpos = pos()})}xxn;
    
    {

    }
}

}

        #(\._\d++_\d++_)
if(0) {
    $subject =~ m{(([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$secondline}
                    ([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$line}
                    ([^\n\r]*+\n){$line}
    .{$column})(?{$lastpos = pos()})
    (?<fieldname>\w++)\b}xxn;
    
    {
        print $+{fieldname} . "\n";
        my $replacement - $+{begin};
        my $fieldname = $+{fieldname};
        my $regexfieldname = $fieldname =~ s{_}{\\S+?}r;

        if($fieldname =~ m{^field_0x}){

            print $regexfieldname . "\n";

            $header =~ m{\b$regexfieldname\b};

            my $finalident = $&;

            print "$fieldname => $finalident at $lastpos\n";

            substr $subject, $lastpos, length $fieldname, $finalident
        }
    }
}

$filename = $ARGV[1] . "out.cc";
open my $fh, '>', $filename or die "error opening $filename: $!";

print $fh $subject;

close $fh;

exit;