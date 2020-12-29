#pragma once
#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
extern "C" {
#else

#define JUST_PARSE

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#endif
#ifdef JUST_PARSE
#define LLVM_FUNCS_MODIFIERS static
#define LLVM_FUNCS_DEF {}
#else
#define LLVM_FUNCS_MODIFIERS extern
#define LLVM_FUNCS_DEF ;
#endif
    LLVM_FUNCS_MODIFIERS void startfunctionparamdecl() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endfunctionparamdecl() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endmodule() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void startmodule(const char*modulename, size_t szmodulename) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void insertinttoimm(const char *str, size_t szstr) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void binary(const char *str, size_t szstr) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void beginconstantexpr() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endconstantexpr() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void startdeclaration(const char* str, size_t szstr, int isinsidedecl, bool bistypedef) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void finalizedeclaration() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void finalizedeclarationtypename() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addsubtotype() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addptrtotype(const char *quailifers, size_t szstr) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void beginscope() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endscope() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endfunctiondef() LLVM_FUNCS_DEF
#ifdef __cplusplus
}
#endif