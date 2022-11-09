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
} = ();

@storageclass{
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

%qualifsnstrgcls_all = (%qualifiers, %storageclass, "typedef");

%keywords = (%qualifsnstrgcls_all, %types, %keywords);

%typeandqualifs = (%qualifsnstrgcls_all, %types);

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
    #use Devel::Peek;
    # CORE::print ("Dumping typs - $threadid\n");
   # $silent = 0;
    #CORE::print (Dump(\@{$typedefidentifiersvector}));
    #CORE::print (Dumper(\@{$typedefidentifiersvector}));
    #$silent = 1;
    foreach my $typedefidentifier (reverse @{$typedefidentifiersvector}) {
        return $typedefidentifier->{$ident} if(exists $typedefidentifier->{$ident})
    }
    return 0
}

sub flattentypedefs {
    my $typeorqualifss = "";
    my %rejects = ();
    foreach my $typedefidentifier (reverse @typedefidentifiersvector) {
        foreach my $ident (keys %$typedefidentifier) {
            if (!$typedefidentifier->{$ident}) {
                $rejects{$ident} = undef;
            }
            elsif (! exists $rejects{$ident}) {
                $typeorqualifss = $typeorqualifss . "|" . $ident
            }
        }
    }
    return  "\\b(" . (substr($typeorqualifss, 1) // "(*F)") . ")\\b"
}

sub checktypedef {
    #regenerate_typedef_regex() if($needregen);
    #print3 "$typedef_regex\n";
    #print3 "checking". $^N . "\n";
    if (checktypedef2 $^N) {
        print3 "$^N -> typedefname\n";
        return 1
    }
    return 0;
}

sub checkident  {
    #print3 "checking" .$^N . "\n";
    if ((not checktypedef2 $^N) and (not exists $keywords{$^N})) {
        print3 "$^N -> ident\n";
        return 1 
    }
    return 0;
}

sub checkidentpermissive  {
    #print3 "checking" .$^N . "\n";
    print3 Dumper(\%keywords) . "\n";
    if ((not exists $keywords{$^N})) {
        print3 "$^N -> ident-permissive\n";
        return 1
    }
    return 0;
}

sub checktypeorqualifortypdf  {
    #print3 "checking". $^N . "\n";

    my $force = $_[0];

    #inc2 "facet";
   # CORE::print ("at g " . Dumper(\%typeandqualifs) . "\n");
    my $input = $^N;
    if (exists $typeandqualifs{$input}) {
        print3 "$^N -> qualifortype\n";
        if(exists $types{$input}) {
            eval {$matches[-1]{typefound} = $input};
            call "add_type" if (not $force);
            return 1;
        } elsif($force || eval { existsflag("outter", {"optoutter" => undef, "outterparams" })
            || existsflag("outterparams", {"optoutter" => undef, "outter" })}) {
            if($input eq 'typedef') {
                eval {$matches[-1]{typedefkey} = $input};
            } else {
                eval {$matches[-1]{qualiffound} = $input};
                if(not exists $storageclass{$input}) {
                    eval {$matches[-1]{qualifnonstoragefnd} = $input}
                }
                call "add_qualif" if (not $force);
            }
            return 1;
        }
        #dec2 "facet"
    }
    elsif (not eval {exists $matches[-1]{typedefnmmatched} or exists $matches[-1]{typefound} or 
            exists $matches[-1]{qualifnonstoragefnd} or exists $matches[-1]{strc}} and checktypedef2($input)) {
        #CORE::print("there typ" . Dumper(\@matches));
        eval {$matches[-1]{typedefnmmatched} = $input};
        return 1;
    }
    #dec2 "facet";
    return 0;
}

sub checkqualifonly {
    return exists $qualifiers{$^N}
}

sub flattentypeorqualif {
    my $typeorqualifss = "";
    for my $el (keys %typeandqualifs) {
        $typeorqualifss = $typeorqualifss . "|" . $el
    }
    return "\\b(" .  substr($typeorqualifss, 1) . ")\\b"
}

sub endfulldecl {
    if($needregen) {
        regenerate_typedef_regex();
    }
}

sub register_decl{
    return if($nesteddecl);

    return if(existsflag "bitfl", {"nonbitfl"});
    #$silent = 0;
    #$sielnt = 1;
    my $identifier = $_[0]{'ident'};
    #CORE::print ("register $threadid ". $identifier . "\n");
    return if not $identifier;
    #$last_object_identifier = $identifier;
    my $priorstate = exists ${$typedefidentifiersvector->[-1]}{$identifier} ? ${$typedefidentifiersvector->[-1]}{$identifier} : -1;
    my $currentstate = exists $_[0]{'typedefkey'};
    ${$typedefidentifiersvector->[-1]}{$identifier} = $currentstate ? 1 : 0;

    #$qtypdfs->enqueue([scalar($currentstate ? 1 : 0),$identifier]) if($threadid eq 0);

    #print3 "$priorstate -> $currentstate\n";
    #$regenerate_needed = 1 
    if($priorstate ne $currentstate) { #and $currentstate ? 1 : $priorstate ne -1) {
        #regenerate_typedef_regex();
        $needregen = $typedefidentifierschanged[-1] = 1;
        #$regenerate_needed = 0;
        $typedefs_changed = 1;
    }
}

sub beginscope {
    push @{$typedefidentifiersvector}, {};
    push @{$typedefidentifierschanged}, 0;
}

sub endscope  {
    $needregen = pop @{$typedefidentifierschanged};
    pop @{$typedefidentifiersvector};
    regenerate_typedef_regex() if($needregen);
}

sub declident {
    if (not eval {exists $matches[-1]{typefound} or exists $matches[-1]{typedefnmmatched}}) {
        return checktypeorqualif($^N)
    }
}

1
