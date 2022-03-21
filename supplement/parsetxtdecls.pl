$isnested = 1;

use re 'eval';


BEGIN{push @INC, "./misc"};
BEGIN{push @INC, "./regexes/supplement"};

$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $proxy = do { local $/; <$fh> };

close $fh;

#require './regexes/parse.pl';

#use re qw(Debug EXECUTE);

my $decls;

while($proxy =~ m{
\n[^\S\n\r]{1}(?<varname>[^\d\s]\S+?):[^\n\S]([^\n]*+\n(\S++[^\n\r\S]){1,2}(?<type>(?!\d|\?|\S+?:\S++|\")\w++))?
}sxxng) {
    my $type = $+{type};
    my $varname = $+{varname} =~ s{\W}{_}rg;

    print $varname . "\n";

    #foreach my $type (@typearr) {
        if ($type and $type =~ m{^(?![0-9a-fA-F]+h?+$ )(?!ds$ )(?!\?\?$ )(?!.$ )}sxx) {
            $decls = $decls . "\nextern $type $varname;";
        } else {
            $decls = $decls . "\nextern unsigned char $varname [1];";
        }
    #}
}

$filename = "decls.h";
open my $fh, '>', $filename or die "error opening $filename: $!";

print $fh $decls;

close $fh;

exit;