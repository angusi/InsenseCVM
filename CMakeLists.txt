cmake_minimum_required(VERSION 3.3)
project(CVM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUGGINGENABLED")

set(SOURCE_FILES Main.c Main.h Strings.h BytecodeTable.h ExitCodes.h Component.c Component.h Logger.c Logger.h Stack.c Stack.h)
add_subdirectory(GC)
add_subdirectory(IteratedList)
add_executable(CVM ${SOURCE_FILES})
target_link_libraries(CVM GC IteratedList)