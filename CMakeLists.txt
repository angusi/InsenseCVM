cmake_minimum_required(VERSION 3.3)
project(CVM)

set(DEBUGGINGENABLED FALSE CACHE BOOL "Debugging Enabled")
set(TARGET "Linux" CACHE STRING "Compilation Target Platform")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra -Wpedantic -Wstrict-overflow -fno-strict-aliasing") #
IF(${DEBUGGINGENABLED})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUGGINGENABLED")
ENDIF(${DEBUGGINGENABLED})

set(SOURCE_FILES Main.c Main.h Strings.h BytecodeTable.h ExitCodes.h Component.c Component.h TypedObject.c TypedObject.h ChannelWrapper.h Procedure.h Procedure.c)
if(${TARGET} STREQUAL "Linux")
    set(SOURCE_FILES ${SOURCE_FILES} UnixVM/Component.c)
ENDIF(${TARGET} STREQUAL "Linux")

MESSAGE( STATUS "SOURCE_FILES: " ${SOURCE_FILES})
MESSAGE( STATUS "TARGET: " ${TARGET})

add_subdirectory(GC)
add_subdirectory(Collections)
add_subdirectory(Logger)
add_subdirectory(Test)
add_subdirectory(ScopeStack)
add_subdirectory(Channels)
add_subdirectory(InsenseRuntimeCVM)
add_executable(CVM ${SOURCE_FILES})
target_link_libraries(CVM GC Collections Logger ScopeStack Channels InsenseRuntimeCVM)