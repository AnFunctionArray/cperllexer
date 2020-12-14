

$text = "!~*(char*volatile*restrict(*const)[4])a";

$result = $text =~/(?{$end = 1})(?<unaryoprev>(?<unaryop>([&*+-~!]))(?!(?&unaryoprev)|(?<typenamerev>(?<typename>(?<typenamebegin>\s*[(]\s*
						(\bint\b|\bchar\b|\bshort\b|\blong\b|\bsigned\b|\bunsigned\b|\bfloat\b|\bdouble\b|(?<qualifiers>\bconst\b|\brestrict\b|\bvolatile\b))*\s*
						(?<abstdecl>(?<abstdeclinside>(?<abstrptrrev>(?<abstrptr>\s*[*]\s*(?<qualifptr>(?&qualifiers))*+\s*)
						
						(?!(?&abstrptrrev))
						
						(?<abstrmostoutter>\s*[(](?&abstdecl)[)]\s*)?+
						(?{ print "$+{qualifptr}\n" if(debug(1)); })
						(?<abstrsubs>\s*\[(?<abstrsubsinner>\d++)\]\s*
						(?{print "$+{abstrsubsinner}\n" if(debug(2));}))*+
						|(?&abstrptr)(?{print "$+{qualifptr}\n" if(debug(3)); }))?+(?{ print "$+{qualifptr}\n" if(debug(4)); })(?&abstrptr)*+
						(?&abstrmostoutter)?+(?&abstrsubs)*+))[)])
						(?<typenameend>))(?!(?&typenamerev)|(?&identifier))|(?&typename)))|(?&unaryop))(?{$end = 1})(?&unaryop)*+
						(?&typename)*+(?<identifier>(?!sizeof)([_a-zA-Z][_a-zA-Z0-9]*+))/xx;
						
						
print $result;

sub debug {

my ($index) = @_;

if(0)
{
print "\n\n";
foreach my $bufname (sort keys %-) {
        my $ary = $-{$bufname};
        foreach my $idx (0..$#$ary) {
            print "\$-{$bufname}[$idx]  ",
                  (defined($ary->[$idx])
                      ? "$ary->[$idx]"
                      : ""),
                  "\n";
        }
    };
    print "\n\n";
    }
    
   if($index eq 4) {return 1 if(1);}
   #else {return 1 if(not $+{abstrptrrev});}
   return 0;
}

