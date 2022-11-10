
sub dispatch_file_scope_stm {
    my $currpos = $_[0];
    $lastcurpos = $currpos;
    #CORE::print ( $currpos . "__" .$lastpos . "\n" );
    if ($nqueues++ == $minlen) {
        $q->enqueue([scalar($lastntypedfs), scalar($lastpos), scalar($currpos), scalar($nqueues) + 1]);
        $lastpos = $currpos;
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
    push_decl($lastcurpos, $+{tagtype} . " " . $+{identtag})
}

sub register_normal_decl {
    my $ident = $_[0];
    $matches[-1]{'ident'} = $ident;
    register_decl($matches[-1]);
    push_decl($lastcurpos, $ident)
}

1