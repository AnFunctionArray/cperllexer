cmake_minimum_required(VERSION 3.16.3)

project(regularc)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/perltoc.c
    COMMAND xsubpp ${CMAKE_SOURCE_DIR}/src/perltoc.xs > ${CMAKE_SOURCE_DIR}/perltoc.c
)

find_package(PerlLibs 5.35)

if(NOT DEFINED ${LLVM_CONFIG})
    set(LLVM_CONFIG "llvm-config")
endif()

execute_process(COMMAND ${LLVM_CONFIG} --libs all
                OUTPUT_VARIABLE llvm_libraries)

execute_process(COMMAND ${LLVM_CONFIG} --includedir
                OUTPUT_VARIABLE llvm_includes)

execute_process(COMMAND ${LLVM_CONFIG} --libdir
                OUTPUT_VARIABLE llvm_libdir)

add_executable(regularc llvm/llvmgen.cpp src/main.c perltoc.c)

string(STRIP ${llvm_libraries} llvm_libraries)

string(STRIP ${llvm_includes} llvm_includes)

string(STRIP ${llvm_libdir} llvm_libdir)

target_link_directories(regularc PRIVATE ${llvm_libdir})

find_package(range-v3 CONFIG REQUIRED)

target_link_libraries(regularc PRIVATE ${llvm_libraries} -rdynamic ${PERL_LIBRARY} -lpthread range-v3 range-v3-meta range-v3::meta range-v3-concepts -lz -lcurses -ldl)

target_compile_options(regularc PRIVATE -rdynamic -Ofast -g -DNDEBUG)

target_include_directories(regularc PRIVATE ${PERL_INCLUDE_PATH} ${llvm_includes} src)

set_property(TARGET regularc PROPERTY CXX_STANDARD 20)

set(CMAKE_CXX_VISIBILITY_PRESET default)
