cmake_minimum_required(VERSION 3.3)
project(GC)

set(CMAKE_C_FLAGS "-std=c99 -lpthread -Wall -Wextra -Wpedantic -Wstrict-overflow -fno-strict-aliasing") #-DDEBUGGINGENABLED

set(SOURCE_FILES GC_mem.h GC_mem_common.c Strings.h)
add_library(GC ${SOURCE_FILES})