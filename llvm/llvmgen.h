#pragma once
#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
#include "llvm/IR/Instructions.h"
extern "C" {
#else

//#define JUST_PARSE

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
    LLVM_FUNCS_MODIFIERS void endfunctionparamdecl(bool bisrest) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endmodule() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void startmodule(const char*modulename, size_t szmodulename) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void insertinttoimm(const char *str, size_t szstr, const char *suffix, size_t szstr1, int type) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void insertfloattoimm(const char* postfix, size_t szstr,
        const char* wholepart, size_t szstr1,
        const char* fractionpart, size_t szstr2,
        const char* exponent, size_t szstr3,
        const char* exponentsign, size_t szstr4) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void startifstatement() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endifstatement() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void continueifstatement() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void binary(const char *str, size_t szstr) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void unary(const char *str, size_t szstr) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void unaryincdec(const char *str, size_t szstr, bool postfix) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endsizeoftypename() LLVM_FUNCS_DEF;
    LLVM_FUNCS_MODIFIERS void endsizeofexpr() LLVM_FUNCS_DEF;
    LLVM_FUNCS_MODIFIERS void subscript() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void beginconstantexpr() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endconstantexpr() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void startdeclaration(const char* str_typedefname, size_t szstr) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void startbuildingstructorunion() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endbuildingstructorunion() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void memberaccess(const char*arrowordot, size_t szstr, const char*ident, size_t szstr1) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void applycast() LLVM_FUNCS_DEF
#ifndef __cplusplus
    LLVM_FUNCS_MODIFIERS void *splitbb(const char *identifier, size_t szident) LLVM_FUNCS_DEF
#else
    LLVM_FUNCS_MODIFIERS llvm::BranchInst *splitbb (const char *identifier, size_t szident) LLVM_FUNCS_DEF
#endif
    //LLVM_FUNCS_MODIFIERS void beginop(bool islogical) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void beginbinary() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void beginlogicalop(int blastorfirst) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endbinarybeforerevlogicops() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endbinary() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endforloopcond() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addforloopiter() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endforloop() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void startdowhileloop() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addbreak() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addcontinue() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void enddowhileloop() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void gotolabel(const char *identifier, size_t szident) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void startswitch() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endswitch() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addCase() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addDefaultCase() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void continuedeclaration() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void adddeclarationident(const char*str, size_t szstr, bool bistypedef) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void finalizedeclarationtypename() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addsubtotype() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addptrtotype(const char *quailifers, size_t szstr) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void beginscope() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endscope() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void finalizedeclaration() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void enddeclaration() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void constructstring() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endreturn() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addplaintexttostring(const char*str, size_t szstr) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void addescapesequencetostring(const char*str, size_t szstr) LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void startfunctioncall() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endfunctioncall() LLVM_FUNCS_DEF
    LLVM_FUNCS_MODIFIERS void endexpression() LLVM_FUNCS_DEF
    //LLVM_FUNCS_MODIFIERS bool obtainvalbyidentifier(const char*identifier, size_t szstr) LLVM_FUNCS_DEF
#ifdef __cplusplus
}
#endif