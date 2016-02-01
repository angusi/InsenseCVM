/*
 * Insense Bytecode Table
 *
 * Copyright (c) 2015, Angus Ireland
 * School of Computer Science, St. Andrews University
 *
 * Built from the IBytecodeTable.java definitions from "A new compiler for Insense",
 * Copyright (c) 2012 Pitiphol Pholpabu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CVM_BYTECODETABLE_H
#define CVM_BYTECODETABLE_H

// Types
#define BYTECODE_TYPE_UNKNOWN           0
#define BYTECODE_TYPE_INTEGER           1
#define BYTECODE_TYPE_UNSIGNED_INTEGER  2
#define BYTECODE_TYPE_REAL              3
#define BYTECODE_TYPE_BOOL              4
#define BYTECODE_TYPE_BYTE              5
#define BYTECODE_TYPE_STRING            6
#define BYTECODE_TYPE_ARRAY             7
#define BYTECODE_TYPE_COMPONENT         8
#define BYTECODE_TYPE_INTERFACE         9
#define BYTECODE_TYPE_IN                10
#define BYTECODE_TYPE_OUT               11


// Bytecodes
// Note that BYTECODE_POP and BYTECODE_TYPE are not defined in the Java VM or Bytecode compiler.
#define BYTECODE_STOP            0 //STOP [COMPONENT_VARIABLE_NAME]
#define BYTECODE_ENTERSCOPE      1
#define BYTECODE_EXITSCOPE       2
#define BYTECODE_PUSH            3 //PUSH [TYPE] [VALUE]
//#define BYTECODE_POP           ??
#define BYTECODE_DECLARE         4 //DECLARE [VARIABLE_NAME] [TYPE]
#define BYTECODE_LOAD            5 //LOAD [VARIABLE_NAME]
#define BYTECODE_STORE           6 //STORE [VARIABLE_NAME]
#define BYTECODE_ADD             7
#define BYTECODE_SUB             8
#define BYTECODE_MUL             9
#define BYTECODE_DIV             10
#define BYTECODE_MOD             11
#define BYTECODE_LESS            12
#define BYTECODE_LESSEQUAL       13
#define BYTECODE_MORE            14
#define BYTECODE_MOREEQUAL       15
#define BYTECODE_EQUAL            16
#define BYTECODE_UNEQUAL         17
#define BYTECODE_AND             18
#define BYTECODE_OR              19
#define BYTECODE_NOT             20
#define BYTECODE_BITAND          21
#define BYTECODE_BITXOR          22
#define BYTECODE_BITNOT          23
//#define BYTECODE_TYPE          ?? //TYPE [TYPE_VARIABLE_NAME] [TYPE] {[NO_OF_CHANNEL] {[DIRECTION] [TYPE] [NAME_OF_CHANNEL] ...}}
#define BYTECODE_COMPONENT       25 //COMPONENT [COMPONENT_NAME] [NO_OF_INTERFACE] {[NO_OF_CHANNEL] {[DIRECTION] [TYPE] [CHANNEL_NAME] ...}}
#define BYTECODE_CALL            26 //CALL [COMPONENT_NAME] [NUMBER_OF_PARAMETERS]
#define BYTECODE_CONSTRUCTOR     27 //CONSTRUCTOR [NUMBER_OF_PARAMETERS] {[TYPE] [PARAMETER_NAME] ...}
#define BYTECODE_BEHAVIOUR_JUMP  28 //BEHAVIOUR_JUMP [INTEGER_TYPE] [4-byte INTEGER]
#define BYTECODE_JUMP            29 //JUMP [INTEGER_TYPE] [4-byte INTEGER]
#define BYTECODE_IF              30 //IF [BYTE_JUMP]
#define BYTECODE_ELSE            31 //ELSE [BYTE_JUMP]
#define BYTECODE_CONNECT         32 //CONNECT [COMPONENT_VARIABLE_NAME] [CHANNEL_NAME] [COMPONENT_VARIABLE_NAME] [CHANNEL_NAME]
#define BYTECODE_DISCONNECT      33 //DISCONNECT [COMPONENT_VARIABLE_NAME] [CHANNEL_NAME]
#define BYTECODE_SEND            34 //SEND [CHANNEL_NAME]
#define BYTECODE_RECEIVE         35 //RECEIVE [CHANNEL_NAME]

#endif //CVM_BYTECODETABLE_H
