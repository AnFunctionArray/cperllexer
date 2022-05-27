$isnested = 1;

use re 'eval';


BEGIN{push @INC, "./misc"};
BEGIN{push @INC, "./regexes/supplement"};

#require './regexes/parse.pl';

$filename = $ARGV[0];
open my $fh, '<', $filename or die "error opening $filename: $!";

my $subjectoutter = do { local $/; <$fh> };

close $fh;

#require './regexes/parse.pl';

#use re => qw(Debug EXECUTE);

$declsout;

while($subjectoutter =~ s{^#.*$}{}gm){}

print "fixing structs...\n";

$subjectoutter =~ s{\b__attribute__\s*+\((\(([^()]|(?1))*+\))\)
    |\b__declspec(\(([^()]|(?1))*+\))
    |\b(__fastcall|__hidden|__cdecl|__extension__|__inline__|__inline|__userpurge|__usercall
    |__stdcall|__unaligned|__thiscall)\b
    |\b__asm__\b[^;]*+;
    |\s:\s*+[^\d\W]++\b\s*+(?=\{)
    |\b__spoils<[^<>]*+>
    |//[^\n]*(?=\n)
    |\b__asm\b\s*+(?<body>\{((?&body)|[^{}])*+\})
    |@<\w++>}{}sxxg; 

print "and asm is fixed too\n";

print "now fixing []\n";

$subjectoutter =~ s{\[\](?=;\n|\s*+=)}{[1]}sg;

@matches = {};

print "fixing operators...\n";

$optoid = {
    "+" => "plus",
    "-" => "minus",
    "*" => "star",
    "/" => "division",
    "%" => "modulo",
    "^" => "xor",
    "&" => "and",
    "|" => "or",
    "~" => "bitwisenot",
    "!" => "logicalnot",
    "=" => "assignment",
    "<" => "lessthan",
    ">" => "greaterthan",
    "+=" => "compoundplus",
    "-=" => "compoundminus",
    "*=" => "compoundmul",
    "/=" => "compounddiv",
    "%=" => "compoundmod" ,
    "^=" => "compoundxor",
    "&=" => "compoundand",
    "|=" => "compoundor",
    "<<" => "shiftleft",
    ">>" => "shiftright",
    "=/" => "sadfaceass",
    ">>=" => "compoundshiftright",
    "<<=" => "compoundshiftleft",
    "==" => "equal",
    "!=" => "notequal",
    "<=" => "lessthanorequal",
    ">=" => "greaterthanorequal", 
    "&&" => "logicaland",
    "||" => "logicalor",
    "++" => "increment",
    "--" => "decremenet",
    "," => "comma",
    "->*" => "pointertomember", 
    "->" => "pointerto" ,
    "()" => "call",
    "[]" => "subscript",
    " new" => "new",
    " new[]" => "newsubscript",
    " delete" => "delete",
    " delete[]" => "deletesubscript",
};

my $basicsingleops = qr{\+|-|\*|/|%|\^|&|\||~|!|=|<|>|,};

my $basicopsdual = qr{=/|\+=|-=|\*=|/=|%=|^=|&=|\|=|<<|>>|>>=|<<=|==|!=|<=|>=|&&|\|\||\+\+|--|->\*|->|\(\)|\[\]};

$subjectoutter =~ s{
   \boperator\b((?(?=$basicopsdual)$basicopsdual|$basicsingleops)|\snew(\[\])?+|\sdelete(\[\])?+)
}{operator$optoid->{$1}}sxxg;

print "fixing pesky template arguments\n";


while($subjectoutter =~ s{
   (?<!['"][^'"\n]{0,20})(?<=\w)(<([^<>]*+|(?R))>)
}{($1 =~ s{(?![<>])($basicops)}{$optoid->{$1}}rg) =~ s{\W}{_}rg}sxxge){}

print "misc\n";

$subjectoutter =~ s{([@]|::)~}{_de}g; 

$subjectoutter =~ s{[@]|::|\$}{_}g; 

$subjectoutter =~ s{
    ^((struct|union|enum)\b\s*+(\w++)\s*+(?<body>\{((?&body)|[^{}])*+\})?+);
}{typedef $2 $3 $3;\n\ntypedef $1 $3;}sxxgm;

while($subjectoutter =~ s{((?<!['"][^'"\n]{0,20})`[^']*?'(?<body>\{((?&body)|[^{}])*+\})?+)}{$1=~s{\W}{_}gr}ge) {}
$subjectoutter =~ s{(?<=[A-Fa-f0-9])(u)?+i64}{$1ll}g;

#print "macos specifics\n";

#$subjectoutter =~ s{
#    Opaque\w++\b
#}{void}sxxgm;

$subjectoutter =~ s{PA\.deinit}{PA_deinit}g;


#use re 'debug';

$filename = $ARGV[1];
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