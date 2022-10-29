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
