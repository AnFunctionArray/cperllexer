# RegularC

Regex is better for writing parsers. Prove me wrong. Working with default perl.

Parser and compiler (Perl & C++/LLVM)

# Limitations:

Initialization is not supported (or actually recently added it but didn't bother to test it since I don't need it for my purposes (or I may have exceeded the memory with my purposes).

Bit-fields are a no no.

Everything else including nested unnamed structures should in theory work.

Requires preprocessed input.

# Usage:

*Don't forget to clone recursively for test examples*

*git clone --recursive repo-url*

## And on the topic:

First probably you would need to fix some things manually but after running the preprocessor you may try:

perl ./regularcbulk/supplement/parsedefs.pl in.pp out_fixed.pp

You may run after (on a seperate file with only the function defs):

perl ./regularcbulk/supplement/splitfunctions.pl out_decls.pp

So you have *fresh* function declarations (note it will also as bonus fill your current directory with *functions*.pp files)

Also note it's better to use RECORD=record env and after REPLAY=record for less waiting times.

Also SILENT=1

And also applying the patch at https://github.com/Perl/perl5/pull/19814 (if not already merged) - so you don't wait all day.

Also read the troubleshooting part.

Also note currently (although it should work on other platforms) my primary target and host is Win32 x86.

And if you have overloading that works too although the sybmbols are mangled by in non standard way (but even overloading on the pointer type should work) and only when needed (meaning normally they are **not** mangled).

## Only parsing:

perl ./regexes/parse.pl ./regularcbulk/tests/test.c

## Parsing and compiling (produces .bc and .ll files in the same directory)

./regularc ./regexes/parse.pl ./regularcbulk/tests/test.c

# Command to compile:

(EDIT: Make it llvm 15 with opaque pointers)

## Initially:

LLVM_CONFIG=llvm-config-mp-14 cmake . -Drange-v3_DIR=$HOME/vcpkg/packages/range-v3_arm64-osx/share/range-v3

## Later on:

LLVM_CONFIG=llvm-config-mp-14 make -B

## Packages needed:

librange-v3 llvm-14 clang-14 libperl cmake

## Troubleshooting:

Since I don't do line numbers you will have to manually troubleshoot. 

You can use hex editor to inspect the record file and see where it starts misbehaving - or just drop the SILENT flag and split the file into smaller parts debug those. This is for lexing errors. For backend errors the tool will always dump what has been generated already in ll file at exit - you can orient yourself on that.

## Target audience

Reverse engineering - recompiling purposes.
