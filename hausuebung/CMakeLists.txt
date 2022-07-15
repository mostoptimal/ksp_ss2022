cmake_minimum_required(VERSION 3.16)
project(njvm C)

set(CMAKE_C_STANDARD 99)

add_compile_options(-g -Wall -pedantic)

add_executable(hausuebung
        njvm/src/njvm.c njvm/src/opCodes.h njvm/src/bigint/include/support.h njvm/src/bigint/include/bigint.h njvm/src/StackSlot.h njvm/src/bigint/include/bigint.h njvm/src/bigint/include/support.h)
