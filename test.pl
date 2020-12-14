#!/usr/bin/perl

$text = "(char(*restrict(*restrict(*const)[1][2])[3])[4])";

$result = $text =~/(?<typename>[(]
						(?<typesandqualifiers>\bint\b|\bchar\b|\bshort\b|\blong\b|\bsigned\b|\bunsigned\b|\bfloat\b|\bdouble\b|(?<qualifiers>\bconst\b|\brestrict\b|\bvolatile\b))*
						(?<abstdecl>((?<abstrptrrev>(?<abstrptr>[*](?<ptrqualifs>(?&qualifiers))*+)(?!(?&abstrptrrev)(?{print "$+{ptrqualifs}\n" if(debug(1));}))
						(?<abstrrest>(?<abstrmostoutter>[(](?&abstdecl)[)])?+(?<abstrsubs>\[(?<abstrsubsinner>\d++)\]
						(?{print "$+{abstrsubsinner}\n" if(debug(2));}))*+(?{print "$+{ptrqualifs}\n" if(debug(1));}))|(?&abstrptr))|(?&abstrrest))(?<abstdeclinside>(?&abstrptr)*+
                        ([(](?&abstdeclinside)[)])?+(\[(\d++)\])*+))[)])/xx;
						
						
print $&;

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
    
   #return 0 if (not $+{abstrrestoutter});
   #else {return 1 if(not $+{abstrptrrev});}
   return 1;
}

