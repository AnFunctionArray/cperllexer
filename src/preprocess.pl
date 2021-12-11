$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";
my $file = do { local $/; <$fh> };
close $fh;

$file =~ s{`(\S+?)'}{
    "_" . ($1 =~ s{\W}{_}rg) . "_"
}sxxge;

$file =~ s{::|(?<=::)~|@|\?(?=\w)}{
    "_"
}sxxge;

$file =~ s{(?<templateargsdecomp>(?<pn>[<]([^<>;\s]++|(?&pn))*+[>]))}{
    ($& =~ s{\W}{_}rg)
}sxxge;

open my $fh, '>', $filename or die "error opening $filename: $!";
print {$fh} $file;
close $fh;