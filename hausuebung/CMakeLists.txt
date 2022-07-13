cmake_minimum_required(VERSION 3.21)
project(hausuebung C)

set(CMAKE_C_STANDARD 11)

add_executable(hausuebung
        njvm/src/njvm.c njvm/src/opCodes.h njvm/src/bigint/include/support.h njvm/src/bigint/include/bigint.h njvm/src/bigint/lib/libbigint.h njvm/src/bigint/include/support.c njvm/src/garbCollect.c njvm/src/garbCollect.h njvm/src/stack.c njvm/src/StackSlot.h njvm/bigint/include/bigint.h njvm/bigint/include/support.h)
