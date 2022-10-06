# CPerllexer

Regex is better for writing parsers.

This is the lexer using solely Perl RegExp (with Perl)

It lexes the C language (context sensitive syntax is also supported - by keeping internal typedef hash)

You can plug it in with the backend (cllvmgen) for a full llvm ir toolchain