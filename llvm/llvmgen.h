#pragma once
#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
extern "C" {
#else
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#endif
    void startfunctionparamdecl();
    void endfunctionparamdecl();
    void endmodule();
    void startmodule(const char*modulename, size_t szmodulename);
    void insertinttoimm(const char *str, size_t szstr);
    void binary(const char *str, size_t szstr);
    void beginconstantexpr();
    void endconstantexpr();
    void startdeclaration(const char* str, size_t szstr, int isinsidedecl, bool bistypedef, const char *typesandqualifiersortypedef, size_t szstr1, bool bwhich);
    void enddeclaration();
    void finalizedeclaration();
    void finalizedeclarationtypename();
    void addsubtotype();
    void addptrtotype(const char *quailifers, size_t szstr);
    void beginscope();
    void endscope();
#ifdef __cplusplus
}
#endif