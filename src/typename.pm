#package typename;

require src::cprogram;

sub identifier_decl {

    push @{$scope[-1]->{'identifiers'}}, {'identinfo' => $_[0]};

    print Dumper($scope[-1]->{'identifiers'});
    print Dumper($scope)
}

sub struc_or_union_body {

    my $lastidenthash = $scope[-1]->{'identifiers'}[-1];

    $lastidenthash->{'struc'} = {'members' => [{}]};

    print Dumper($lastidenthash);

    my $laststruchash = $lastidenthash->{'struc'};

    $laststruchash->{'taginfo'} = $_[0];

    print Dumper($laststruchash);
    
    #@{$laststruchash->{'members'}} = {};

    push @{$scope[-1]->{'lastscope'}}, $scope[-1]->{'identifiers'};

    $scope[-1]->{'identifiers'} = \@{$laststruchash->{'members'}};

    print Dumper(@scope);
}

sub endbuildingstructorunion {
    my $lastidentifs = pop @{$scope[-1]->{'lastscope'}};
    $scope[-1]->{'identifiers'} = $lastidentifs;
    print Dumper(@scope);
}

{
    1
}