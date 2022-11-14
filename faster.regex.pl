
sub dispatch_file_scope_stm {
    my $currpos = $_[0];
    $lastcurpos = $currpos;
    #CORE::print ( $currpos . "__" .$lastpos . "\n" );
    if ($nqueues++ == $minlen) {
        $q->enqueue([scalar($lastntypedfs), scalar($lastpos_dispatch), scalar($currpos)]);
        $lastpos_dispatch = $currpos;
        if ($typedefs_changed) {
        # eval {#use Data::Dumper;
            #CORE::print ("sending\n");
            #CORE::print Dumper(\$typedefidentifiersvector);
        # };
            $qtypdfs->enqueue({ %{$typedefidentifiersvector->[0]} });
            $lastntypedfs = $qtypdfs->pending();#scalar(@{keys %{$typedefidentifiersvector->[0]}});
            undef $typedefs_changed;
        }
        $nqueues = 0;
    }
}

sub register_taggable {
    push_decl(pos(), $+{tagtype} . " " . $+{identtag})
}

sub register_normal_decl {
    my $poslast = pos();
    my $ident = $_[0];
    $matches[-1]{'ident'} = $ident;
    #CORE::print ("registering " . $ident . " " . $matches[-1]{typedefkey} . "\n");
    register_decl($matches[-1]);
    push_decl($poslast, $ident)
}