my sub Dumper  {use Data::Dumper; Dumper(@_) if( not $silent )}

#my sub Dumper {"\n"}

my %typedefs;

=begin
(?<keywordlist>(?&typesandqualifiersmask)
	|\b(typedef|struct|enum|sizeof|break|case|continue|default|do|else|for|goto|if|return|switch|while)\b)

(?<typesandqualifiersmask>(\bint\b|\bchar\b|\bshort\b|\blong\b
						|\bsigned\b|\bunsigned\b|\bfloat\b|\bdouble\b)|
						(?<qualifiers>\bconst\b|\brestrict\b|\bvolatile\b)
						|(?<storageclass>\bextern\b|\bstatic\b|\bauto\b|\bregister\b))
=cut

#sub inittypenameconstants {

@types{
    "int",
    "char",
    "short",
    "long",
    "signed",
    "unsigned",
    "float",
    "double"
} = ();

@qualifiers{
    "const",
    "restrict",
    "volatile",
    "extern",
    "static",
    "auto",
    "register"
} = ();

@keywords{
    "typedef",
    "struct",
    "union",
    "enum",
    "sizeof",
    "break",
    "case",
    "continue",
    "default",
    "do",
    "else",
    "for",
    "goto",
    "if",
    "return",
    "switch",
    "while"
} = ();

%keywords = (%qualifiers, %types, %keywords);

%typeandqualifs = (%qualifiers, %types);

#}

=begin
sub regenerate_typedef_regex {
    #my $ident = "(*F)";
    %typedefs = ();

    my %disallowed;
    foreach my $typedefidentifier (reverse @typedefidentifiersvector) {
        foreach my $k (keys %$typedefidentifier) {
            $typedefs{$k} = undef if($typedefidentifier->{$k} and not exists $disallowed{$k});
            $disallowed{$k} = 1;
        }
    }
    #use if $ARGV[1] eq 2, re => qw(Debug EXECUTE);
    #$ident = qr{(?>(?<typedef>(?<ident>(*F)))|$ident)(?(<ident>)(*F))}sxx;
    #$ident = "(?>$ident)";
    print3 Dumper %typedefs;
    $needregen = 0;
}
=cut

sub checktypedef2 {
    my $ident = $_[0];
    foreach my $typedefidentifier (reverse @typedefidentifiersvector) {
        return $typedefidentifier->{$ident} if(exists $typedefidentifier->{$ident})
    }
    return 0
}

sub checktypedef {
    #regenerate_typedef_regex() if($needregen);
    #print3 "$typedef_regex\n";
    #print3 "checking". $^N . "\n";
    if (checktypedef2 $^N) {
        print3 "$^N -> typedefname\n";
        return qr{}o 
    }
    return qr{(*F)}o;
}

sub checkident  {
    #print3 "checking" .$^N . "\n";
    if ((not checktypedef2 $^N) and (not exists $keywords{$^N})) {
        print3 "$^N -> ident\n";
        return qr{}o 
    }
    return qr{(*F)}o;
}

sub checkidentpermissive  {
    #print3 "checking" .$^N . "\n";
    print3 Dumper(\%keywords) . "\n";
    if ((not exists $keywords{$^N})) {
        print3 "$^N -> ident-permissive\n";
        return qr{}o 
    }
    return qr{(*F)}o;
}

sub checktypeorqualif  {
    #print3 "checking". $^N . "\n";

    inc2 "facet";
    if (exists $typeandqualifs{$^N}) {
        print3 "$^N -> qualifortype\n";
        if(exists $types{$^N}) {
            eval {$matches[-1]{typefound} = $^N};
            call "add_type"
        } else {
            eval {$matches[-1]{qualiffound} = $^N};
            call "add_qualif"
        }
        dec2 "facet";
        return qr{}o 
    }
    dec2 "facet";
    return qr{(*F)}o;
}

sub endfulldecl {
    if($needregen) {
        regenerate_typedef_regex();
    }
}

sub register_decl{
    return if($nesteddecl);
    
    my $identifier = $_[0]{'ident'};
    return if not $identifier;
    #$last_object_identifier = $identifier;
    my $priorstate = exists ${$typedefidentifiersvector[-1]}{$identifier} ? ${$typedefidentifiersvector[-1]}{$identifier} : -1;
    my $currentstate = $_[0]{'typedefkey'};
    ${$typedefidentifiersvector[-1]}{$identifier} = $currentstate ? 1 : 0;
    #print3 "$priorstate -> $currentstate\n";
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

sub endscope  {
    $needregen = pop @typedefidentifierschanged;
    pop @typedefidentifiersvector;
    regenerate_typedef_regex() if($needregen);
}

1