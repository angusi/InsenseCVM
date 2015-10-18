cmake_minimum_required(VERSION 3.3)
project(CVM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(gc REQUIRED)
include_directories(${BOEHMGC_INCLUDE_DIRS})
set(LIBS ${LIBS} ${BOEHMGC_LIBRARIES})

set(SOURCE_FILES Main.c Main.h Strings.h BytecodeTable.h ExitCodes.h Component.c Component.h)
add_executable(CVM ${SOURCE_FILES})
target_link_libraries(CVM ${BOEHMGC_LIBRARIES})