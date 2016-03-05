cmake_minimum_required(VERSION 3.3)
project(InsenseRuntimeCVM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra -Wpedantic -Wstrict-overflow -fno-strict-aliasing")

set(SOURCE_FILES StandardFunctions.h StandardFunctionsAbsInt.c StandardFunctionsAbsReal.c StandardFunctionsAvgIntArray.c
        StandardFunctionsAvgRealArray.c StandardFunctionsByteArrayEqual.c StandardFunctionsGetString.c
        StandardFunctionsIntToByte.c StandardFunctionsIntToUnsigned.c StandardFunctionsParseInt.c
        StandardFunctionsParseReal.c StandardFunctionsParseUnsigned.c StandardFunctionsPrintByte.c
        StandardFunctionsPrintInt.c StandardFunctionsPrintReal.c StandardFunctionsPrintString.c
        StandardFunctionsPrintTCByte.c StandardFunctionsPrintTCInt.c StandardFunctionsPrintTCReal.c
        StandardFunctionsPrintTCString.c StandardFunctionsPrintTCUnsignedInt.c StandardFunctionsPrintUnsignedInt.c
        StandardFunctionsRealToInt.c StandardFunctionsSquareInt.c StandardFunctionsSquareReal.c
        StandardFunctionsSquareRoot.c StandardFunctionsStringEquals.c)
add_library(Collections ${SOURCE_FILES})
target_link_libraries(Collections GC)