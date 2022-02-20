#$isnested = 1;

use re 'eval';

use File::Basename;


BEGIN{push @INC, "./misc"};
BEGIN{push @INC, "./regexes/supplement"};

$filename = $ARGV[1];
open my $fh, '<', $filename or die "error opening $filename: $!";

$subject = do { local $/; <$fh> };

close $fh;

$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

$proxy = do { local $/; <$fh> };

close $fh;

#require './regexes/parse.pl';

use Data::Dumper;

=begin

my $original_identifier_decl = \&identifier_decl;
my $original_beginscope = \&beginscope;

sub identifier_decl {
    $original_identifier_decl->();
    
}

my $scopehasbegan;

my $fnname;

sub beginscope {
    $original_beginscope->();

    $scopehasbegan = 1;
}

exit;

sub identifier_decl {
    $original_identifier_decl->();

    $fnname = $_[0]{"ident"} if(not $scopehasbegan and not $fnname);

    return if(not $scopehasbegan);

    $proxy =~ m{FUNCTION\s++\*\s++;\*{62}\s++[^\s]++}
}

=cut

my $fnname = basename($ARGV[0], ".c");

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

$subject =~ s{;\n\n}{;\n$init\n\n}sxxn;
$subject =~ s{\{}{\{$stackdecl}sxxn;

if($lastvalid) {

    #$filename = $ARGV[0];
    #open my $out, '>', $filename or die "error opening $filename: $!";

    #print $out $subject;
    #print $subject;

    #$subject =~ qr{(?(DEFINE)$mainregexdefs)(?&$entryregex)}sxxn

    #require './regexes/parse.pl'
}

print $subject . "\n";

#$subject =~ qr{(?(DEFINE)$mainregexdefs)(?&$entryregex)}sxxn

require './regexes/parse.pl'