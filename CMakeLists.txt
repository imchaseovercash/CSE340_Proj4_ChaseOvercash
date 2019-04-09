cmake_minimum_required(VERSION 3.13)
project(CSE340_Proj4_ChaseOvercash)

set(CMAKE_CXX_STANDARD 17)

add_executable(CSE340_Proj4_ChaseOvercash project4.cpp lexer.cpp lexer.h inputbuf.cpp inputbuf.h ir_debug.cpp ir_debug.h compiler.cpp compiler.h)