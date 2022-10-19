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
   $istypedef = 0;

    if ($+{identf} ne "typedef") {
        return qr{};
    }

    $istypedef = 1;
    
    return qr{(*F)};
}

=begin
sub register_decl_universal {
    my $res = $_[2];
    my $out = $_[3];

    if ($istypedef) {
        register_decl();
        $$res = 1;
        $$out = $lastdeclpos;
        CORE::print "$lastdeclpos";
    }
}
=cut

sub register_decl_fast {
    #CORE::print pos() . "\n";
}

1