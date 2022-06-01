cmake_minimum_required(VERSION 3.21)
project(hausuebung C)

set(CMAKE_C_STANDARD 11)

add_executable(hausuebung
        njvm/src/njvm.c njvm/src/opCodes.h)
