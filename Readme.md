# CPerllexer

Regex is better for writing parsers.

This is the lexer using solely Perl RegExp (with Perl)

It lexes the C language (context sensitive syntax is also supported - by keeping internal typedef hash)

You can plug it in with the backend (cllvmgen) for a full llvm ir toolchain

You can run either via the backend binary or with the perl interpreter itself

perl ./parse.pl ./bulk/tests/test.c

Just use this patch (if not already merged - it increases speed by 2x (not a joke) https://github.com/Perl/perl5/pull/19814)

You need to check - I've recently disabled verbose logging (if you fancy to see the output)

I may post info here - https://cparser2.blogspot.com


Backend - compile and run regularc from here - https://github.com/AnFunctionArray/cllvmbackend

Backend doesn't support variable initializations.

Void must be typedefed (I don't like it).

Function overriding is somewhat supported (because of needs)

Variadaic arguments are something that I've not even came to imlement (although the ... is parsed (and also void as single keyword meaning no parameters))

Each thread generates its own IR file.

Tested on huge 8.5 MB files - if there is an error check the right number - it's offset in the file being parsed. 
