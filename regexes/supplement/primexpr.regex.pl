
sub assignment {
    push2 \@matches, {"binoplast" => $+{assignop}};
    call 'binary'; 
    call 'end_binary'; 
    pop2 \@matches
}

sub ternary0 {
    call 'begin_branch'; 
}

sub ternary1 {
    push2 \@matches, {"binoplast" => "&&"};
    call 'binary'; 
    pop2 \@matches;
    call 'begin_branch'; 
}

sub ternary2 {
    push2 \@matches, {"binoplast" => "||"};
    call 'binary'; 
    pop2 \@matches
}


1