#!/usr/bin/perl

$text = "7*7*4*5*6*7+(4*4*4+3*7*4);7*7*4*5*6*7+(4*4*4+3*7*4)";

$result = $text =~/((*F)
    (?<addmulplusrest>(?>(?&muloprest))((?!\+\+|--)(?<addop>[\+\-])(?&addmulplusrest))|(?&muloprestfacet)(?{print "$+{addop}\n" if(debug(1));}))

    (?<muloprest>(?>(?&testxpr))((?<mulop>[\*\/\%])(?&muloprest))|(?&testxprfacet)(?{print "$+{mulop}\n" if(debug(1));}))
    (?<testxpr>(?<number>\d++)(??{print "$+{number}\n" if(debug(1)); return "(*F)";})|(?<inparenth>[(](?&addmulplusrest)[)]))

    (?<addmulplusrestfacet>(?>(?&muloprestfacet))((?!\+\+|--)([\+\-])(?&addmulplusrestfacet))|(?&muloprestfacet))

    (?<muloprestfacet>(?>(?&testxprfacet))([\*\/\%](?&muloprestfacet))|(?&testxprfacet))
    (?<testxprfacet>(\d++)|([(](?&addmulplusrestfacet)[)])))|(?&addmulplusrest);(?&addmulplusrest)/xx;
						
						
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

