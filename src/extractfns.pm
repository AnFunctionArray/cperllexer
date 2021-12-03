use IO::Handle;
use re 'eval';

$filename = "D:/out/decls.h";
open $fdecls, '>', $filename or die "error opening $filename: $!";

sub extract_struc {
    print {$fdecls} $+{strucdecl} . "\n";
    $fdecls->flush()
}

my $func_n = 0;

sub extract_func {

    my $funcbody = $+{func};

    my $filename = "D:/out/func_$func_n.c";
    open my $fdefs, '>', $filename or die "error opening $filename: $!";
    print {$fdefs} "\#include \"decls.h\"\n\n" . $funcbody . "\n";
    close $fdefs;
    $func_n++
}



sub identifier_decl_facet {
    return identifier_decl(
        {
            'ident' => $_[0]{'identfacet'},
            'typedefkeyfacet' => $_[0]{'typedefkeyfacet'}
        })
}

sub extract_decls {
    print {$fdecls} $+{decloptorfndef} . "\n";
    $fdecls->flush()
}

#select(<>); 

{
    1
}