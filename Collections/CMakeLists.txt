cmake_minimum_required(VERSION 3.3)
project(Collections)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra")

set(SOURCE_FILES IteratedList.h IteratedListConstruct.c IteratedListContains.c IteratedListDisplayList.c IteratedListGetElementN.c IteratedListGetListLength.c IteratedListGetNextElement.c IteratedListInsert.c IteratedListIsEmpty.c IteratedListRemove.c IteratedListRemoveAll.c IteratedListSearchList.c Stack.h Stack.c Strings.h)
add_library(Collections ${SOURCE_FILES})
target_link_libraries(Collections GC)