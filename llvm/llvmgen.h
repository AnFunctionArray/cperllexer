#pragma once
#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
extern "C" {
#else
#include <stdint.h>
#include <stddef.h>
#endif
    void insertinttoimm(const char *str, size_t szstr);
    void mul(const char *str, size_t szstr);
    void beginconstantexpr();
    void endconstantexpr();
#ifdef __cplusplus
}
#endif