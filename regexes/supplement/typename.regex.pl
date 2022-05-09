use Data::Dumper;

my sub print {CORE::print(@_) if( $ENV{'DEBUG'} )}

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

my $ok = qr{};
my $notok = qr{(*F)};

my %types;
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

my %qualifiers;

@qualifiers{
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

%keywords = (%qualifiers, %types, %keywords);

my %typeandqualifs = (%qualifiers, %types);

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
    print Dumper %typedefs;
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
    #print "$typedef_regex\n";
    #print "checking". $^N . "\n";
    if (checktypedef2 $^N) {
        print "$^N -> typedefname\n";
        return $ok 
    }
    return $notok;
}

sub checkident {
    #print "checking" .$^N . "\n";
    if ((not checktypedef2 $^N) and (not exists $keywords{$^N})) {
        print "$^N -> ident\n";
        return $ok 
    }
    return $notok;
}

sub checkidentpermissive {
    #print "checking" .$^N . "\n";
    if ((not exists $keywords{$^N})) {
        print "$^N -> ident-permissive\n";
        return $ok 
    }
    return $notok;
}

sub checktypeorqualif {
    #print "checking". $^N . "\n";

    inc2 "facet";
    if (exists $typeandqualifs{$^N}) {
        print "$^N -> qualifortype\n";
        if(exists $types{$^N}) {
            eval {$matches[-1]{typefound} = $^N};
            call "add_type"
        } else {
            eval {$matches[-1]{qualiffound} = $^N};
            call "add_qualif"
        }
        dec2 "facet";
        return $ok 
    }
    dec2 "facet";
    return $notok;
}

sub endfulldecl {
    if($needregen) {
        regenerate_typedef_regex();
    }
}

sub identifier_decl {
    return if($nesteddecl);
    
    my $identifier = $_[0]{'ident'};
    return if not $identifier;
    #$last_object_identifier = $identifier;
    my $priorstate = exists ${$typedefidentifiersvector[-1]}{$identifier} ? ${$typedefidentifiersvector[-1]}{$identifier} : -1;
    my $currentstate = $_[0]{'typedefkey'};
    ${$typedefidentifiersvector[-1]}{$identifier} = $currentstate ? 1 : 0;
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
