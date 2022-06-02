my sub Dumper  {use Data::Dumper; Dumper(@_) if( not $silent )}


sub assignment {
    push2 \@matches, {"binoplast" => $+{assignop}};
    call 'binary'; 
    pop2 \@matches
}

sub comma {
    shift;
    my $flagsarg = shift;
    my $res = shift;
    #print "in begin_binary\n";
    #print Dumper \@$flagsarg;
    #print
    
    $$res = not existsflag "callexpr", {"newexpr"}, $flagsarg;
}
=begin
sub begin_binary {
    shift;
    my $flagsarg = shift;
    my $res = shift;
    print "in begin_binary\n";
    print Dumper \@$flagsarg;
    print $$res = not existsflag "ternaryexpr", {"normalexpr"}, $flagsarg;
}

sub end_binary {
    shift;
    my $flagsarg = shift;
    my $res = shift;
    print $$res = not existsflag "ternaryexpr", {"normalexpr"}, $flagsarg;
}
=cut

1