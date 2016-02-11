cmake_minimum_required(VERSION 3.3)
project(CVM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra -Wpedantic -Wstrict-overflow -fno-strict-aliasing -DDEBUGGINGENABLED") #

set(SOURCE_FILES Main.c Main.h Strings.h BytecodeTable.h ExitCodes.h Component.c Component.h TypedObject.c TypedObject.h ChannelWrapper.h)
add_subdirectory(GC)
add_subdirectory(Collections)
add_subdirectory(Logger)
add_subdirectory(Test)
add_subdirectory(ScopeStack)
add_subdirectory(Channels)
add_executable(CVM ${SOURCE_FILES})
target_link_libraries(CVM GC Collections Logger ScopeStack Channels)