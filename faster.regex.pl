
sub dispatch_file_scope_stm {
    my $currpos = pos();
    CORE::print ( $currpos . "__" .$lastpos . "\n" );
    $q->enqueue([scalar($lastntypedfs), scalar($lastpos), scalar($currpos)]);
    $lastpos = $currpos;
    if ($typedefs_changed) {
        $qtypdfs->enqueue(\{ %{$typedefidentifiersvector->[0]} });
        $lastntypedfs = $qtypdfs->pending();#scalar(@{keys %{$typedefidentifiersvector->[0]}});
        undef $typedefs_changed;
    }
}

sub register_taggable {
    push_decl($lastpos, $+{identtag})
}

sub register_normal_decl {
    my $ident = $+{identnormal} // $+{identinside};
    $matches[-1]{'ident'} = $ident;
    register_decl($matches[-1]);
    push_decl($lastpos, $ident)
}

1