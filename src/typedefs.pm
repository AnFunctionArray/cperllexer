require src::cprogram;


sub identifier_typedef {
    my $ident = "(*F)";
    foreach my $currscope (@scope) {
        $ident = join '|', grep $_, $ident, $currscope->{'typedefidentifiers'}
    }
    print $ident . "\n";
    return $ident
}

sub identifier_decl {

    my $lastelem = $scope[-1]->{'typedefidentifiers'};
    $lastelem = join '|', grep $_, $lastelem, $_[0]->{'ident'} unless (not $_[0]->{'typedefkeyfacet'});
    $scope[-1]->{'typedefidentifiers'} = $lastelem;
}

{
    1
}