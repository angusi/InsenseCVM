cmake_minimum_required(VERSION 3.3)
project(IteratedList)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra")

set(SOURCE_FILES IteratedList.h IteratedListConstruct.c IteratedListContains.c IteratedListDisplayList.c IteratedListFreeNode.c IteratedListGetElementN.c IteratedListGetListLength.c IteratedListGetNextElement.c IteratedListInsert.c IteratedListIsEmpty.c IteratedListRemove.c IteratedListRemoveAll.c IteratedListSearchList.c Strings.h)
add_library(IteratedList ${SOURCE_FILES})