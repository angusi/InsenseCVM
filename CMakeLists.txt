cmake_minimum_required(VERSION 3.3)
project(CVM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra -DDEBUGGINGENABLED") #

set(SOURCE_FILES Main.c Main.h Strings.h BytecodeTable.h ExitCodes.h Component.c Component.h VariableStorage/ScopeStack.c VariableStorage/ScopeStack.h VariableStorage/VariableStorage.c VariableStorage/VariableStorage.h)
add_subdirectory(GC)
add_subdirectory(Collections)
add_subdirectory(Logger)
add_subdirectory(Test)
add_executable(CVM ${SOURCE_FILES})
target_link_libraries(CVM GC Collections Logger)