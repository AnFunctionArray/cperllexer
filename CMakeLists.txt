cmake_minimum_required(VERSION 3.22)

project(regularc)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/perltoc.c
    COMMAND xsubpp ${CMAKE_SOURCE_DIR}/src/perltoc.xs > ${CMAKE_CURRENT_BINARY_DIR}/perltoc.c
)

execute_process(COMMAND llvm-config --libs all
                OUTPUT_VARIABLE llvm_libraries)

execute_process(COMMAND perl -e "print \"@INC[2]/CORE\n\";"
                OUTPUT_VARIABLE perl_includes)

execute_process(COMMAND llvm-config --includedir
                OUTPUT_VARIABLE llvm_includes)

execute_process(COMMAND llvm-config --libdir
                OUTPUT_VARIABLE llvm_libdir)

add_executable(regularc llvm/llvmgen.cpp src/main.c ${CMAKE_CURRENT_BINARY_DIR}/perltoc.c)

string(STRIP ${llvm_libraries} llvm_libraries)

string(STRIP ${perl_includes} perl_includes)

string(STRIP ${llvm_includes} llvm_includes)

string(STRIP ${llvm_libdir} llvm_libdir)

target_link_directories(regularc PRIVATE ${llvm_libdir} ${perl_includes})

find_package(range-v3 CONFIG REQUIRED)

target_link_libraries(regularc PRIVATE ${llvm_libraries} -lperl -lpthread range-v3 range-v3-meta range-v3::meta range-v3-concepts -lz -lcurses)

target_compile_options(regularc PRIVATE -Os -g)

target_include_directories(regularc PRIVATE ${perl_includes} ${llvm_includes} src)

target_compile_features(regularc PRIVATE cxx_std_20)