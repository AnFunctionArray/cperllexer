use Data::Dumper;

my %typedefs;

=begin
(?<keywordlist>(?&typesandqualifiersmask)
	|\b(typedef|struct|enum|sizeof|break|case|continue|default|do|else|for|goto|if|return|switch|while)\b)

(?<typesandqualifiersmask>(\bint\b|\bchar\b|\bshort\b|\blong\b
						|\bsigned\b|\bunsigned\b|\bfloat\b|\bdouble\b)|
						(?<qualifiers>\bconst\b|\brestrict\b|\bvolatile\b)
						|(?<storageclass>\bextern\b|\bstatic\b|\bauto\b|\bregister\b))
=cut

my $ok = qr{};
my $notok = qr{(*F)};

my %typeandqualifs;
@typeandqualifs{
    "int",
    "char",
    "short",
    "long",
    "signed",
    "unsigned",
    "float",
    "double",
    "const",
    "restrict",
    "volatile",
    "extern",
    "static",
    "auto",
    "register"
} = ();

my %keywords;
@keywords{
    "typedef",
    "struct",
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


sub regenerate_typedef_regex {
    #my $ident = "(*F)";
    %typedefs = ();

    my %disallowed;
    foreach my $typedefidentifier (reverse @typedefidentifiersvector) {
        foreach my $k (keys %$typedefidentifier) {
            $typedefs{$k} = 1 if($typedefidentifier->{$k} and not exists $disallowed{$k});
            $disallowed{$k} = 1;
        }
    }
    #use if $ARGV[1] eq 2, re => qw(Debug EXECUTE);
    #$ident = qr{(?>(?<typedef>(?<ident>(*F)))|$ident)(?(<ident>)(*F))}sxx;
    #$ident = "(?>$ident)";
    print Dumper %typedefs;
    $needregen = 0;
}

sub checktypedef {
    #regenerate_typedef_regex() if($needregen);
    #print "$typedef_regex\n";
    #print "checking". $+{ident} . "\n";
    if (exists $typedefs{$+{ident}}) {
        print "$+{ident} -> typedefname\n";
        return $ok 
    }
    return $notok;
}

sub checkident {
    #print "checking" .$+{ident} . "\n";
    if ((not exists $typedefs{$+{ident}}) and (not exists $keywords{$+{ident}}) and (not exists $typeandqualifs{$+{ident}})) {
        print "$+{ident} -> ident\n";
        return $ok 
    }
    return $notok;
}

sub checkidentqualified {
    #print "checking" .$+{ident} . "\n";
    if ((not exists $keywords{$+{ident}}) and (not exists $typeandqualifs{$+{ident}})) {
        print "$+{ident} -> ident\n";
        return $ok 
    }
    return $notok;
}

sub checktypeorqualif {
    #print "checking". $+{ident} . "\n";
    if (exists $typeandqualifs{$+{ident}}) {
        print "$+{ident} -> qualifortype\n";
        return $ok 
    }
    return $notok;
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
