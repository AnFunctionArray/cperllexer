

sub regenerate_typedef_regex {
    my $ident = "(*F)";
    my %disallowed;
    foreach my $typedefidentifier (reverse @typedefidentifiersvector) {
        foreach my $k (keys %$typedefidentifier) {
            $ident = qr{$ident|\Q$k\E} if($typedefidentifier->{$k} and not exists $disallowed{$k});
            $disallowed{$k} = 1;
        }
    }
    use if $ARGV[1] eq 2, re => qw(Debug EXECUTE);
    #$ident = qr{(?>(?<typedef>(?<ident>(*F)))|$ident)(?(<ident>)(*F))}sxx;
    #$ident = "(?>$ident)";
    print $ident . "\n";
    $typedef_regex = qr{$ident}sxxn;
    $needregen = 0;
}

sub identifier_typedef {
    #regenerate_typedef_regex() if($needregen);
    #print "$typedef_regex\n";
    return $typedef_regex;
}

sub endfulldecl {
    if($needregen) {
        regenerate_typedef_regex();
    }
}

sub identifier_decl {
    my $identifier = $_[0]{'ident'};
    return if not $identifier;
    #$last_object_identifier = $identifier;
    my $priorstate = exists ${$typedefidentifiersvector[-1]}{$identifier} ? ${$typedefidentifiersvector[-1]}{$identifier} : -1;
    my $currentstate = $_[0]{'typedefkey'};
    ${$typedefidentifiersvector[-1]}{$identifier} = $currentstate;
    #print "$priorstate -> $currentstate\n";
    #$regenerate_needed = 1 
    if($priorstate ne $currentstate) { #and $currentstate ? 1 : $priorstate ne -1) {
        #regenerate_typedef_regex();
        $needregen = $typedefidentifierschanged[-1] = 1;
        #$regenerate_needed = 0;
    }
}

sub beginscope {
    push @typedefidentifiersvector, {};
    push @typedefidentifierschanged, 0;
}

sub endscope {
    $needregen = pop @typedefidentifierschanged;
    pop @typedefidentifiersvector;
    regenerate_typedef_regex() if($needregen);
}

1
