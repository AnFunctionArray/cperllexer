#!/usr/bin/perl

$text = "(char*const(*restrict[2])[4][5])";

$result = $text =~/[(](\bint\b|\bchar\b|\bshort\b|\blong\b|\bsigned\b|\bunsigned\b|\bfloat\b|\bdouble\b|
			(?<qualifiers>\bconst\b|\brestrict\b|\bvolatile\b))
			(?<abstdecl>(?&abstrptr)*(?<abstrptr1>(?<abstrptr>[*](?<ptrqualifs>(?&qualifiers))))
			(?<abstrest>(?<abstrmostoutter>[(](?&abstdecl)[)])?+
			(?<abstrsubs>\[(?<abstrsubsinner>\d++)\]
			(?{print "$+{abstrsubsinner}\n" if(debug(2));}))*+)?+(?{print "$+{ptrqualifs}\n" if(debug(1));})
			(?!)|
			(?&abstrptr)*?(?&abstrptr)+(?<abstrrestoutter>([(](?&abstrptr)*?(?&abstrptr)+(?&abstrrestoutter)?+[)])?+
			(\[(\d++)\])*+)?+|(?&abstrest))[)]/xx;
						
						
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

