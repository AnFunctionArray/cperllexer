my sub Dumper  {use Data::Dumper; Dumper(@_) if( not $silent )}

my %tagkeys;
@tagkeys{
    "struct",
    "union",
    "enum",
} = ();

sub checkfastident {
    $istaggable = exists $tagkeys{$+{identf}};
   # CORE::print "$+{identf}\n";
    return qr{};
}

1