# RegularC

Regex is better for writing parsers. Prove me wrong. Working with default perl.

Parser and compiler (Perl & C++/LLVM)

# Limitations:

Initialization is not supported.

Requires preprocessed input.

# Usage:

*Don't forget to clone recursively for test examples*

*git clone --recursive repo-url*

## Only parsing:

perl ./regexes/parse.pl ./regularcbulk/tests/test.c

## Parsing and compiling (produces .bc and .ll files in the same directory)

./regularc ./regexes/parse.pl ./regularcbulk/tests/test.c

# Command to compile:

## Initially:

LLVM_CONFIG=llvm-config-mp-14 cmake . -Drange-v3_DIR=$HOME/vcpkg/packages/range-v3_arm64-osx/share/range-v3

## Later on:

LLVM_CONFIG=llvm-config-mp-14 make -B

## Packages needed:

librange-v3 llvm-14 clang-14 libperl cmake