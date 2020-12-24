#!/usr/bin/perl

$text = "abcd+6";

$result = $text =~/(?<test>(abcd)(?{print %-{test}[1]})\+6)/xx;
						
						
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

