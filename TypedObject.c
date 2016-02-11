/*
 * 
 *
 * Copyright (c) 2015, Angus Ireland
 * School of Computer Science, St. Andrews University
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

#include "TypedObject.h"
#include "BytecodeTable.h"

static void TypedObject_decRef(TypedObject_PNTR pntr);

struct TypedObject {
    void (*decRef)(TypedObject_PNTR pntr);
    int type;
    void* object;
};

TypedObject_PNTR TypedObject_construct(unsigned int type, void* object) {
    TypedObject_PNTR newObject = GC_alloc(sizeof(TypedObject_s), true);
    newObject->type = type;
    GC_assign(&(newObject->object), object);
    newObject->decRef = TypedObject_decRef;
    return newObject;
}

void TypedObject_setObject(TypedObject_PNTR this, void* object) {
    this->object = object;
}

void* TypedObject_getObject(TypedObject_PNTR this) {
    return this->object;
}

void TypedObject_setTypeByteCode(TypedObject_PNTR this, unsigned int type) {
    this->type = type;
}

int TypedObject_getTypeByteCode(TypedObject_PNTR this) {
    return this->type;
}

void TypedObject_decRef(TypedObject_PNTR this) {
    GC_decRef(this->object);
}

bool TypedObject_isNumber(TypedObject_PNTR this) {
    return this->type == BYTECODE_TYPE_INTEGER ||
           this->type == BYTECODE_TYPE_REAL ||
           this->type == BYTECODE_TYPE_BYTE ||
           this->type == BYTECODE_TYPE_UNSIGNED_INTEGER;
}

size_t TypedObject_getSize(unsigned int type) {
    switch(type) {
        case BYTECODE_TYPE_INTEGER:
            return sizeof(int);
        case BYTECODE_TYPE_UNSIGNED_INTEGER:
            return sizeof(unsigned int);
        case BYTECODE_TYPE_REAL:
            return sizeof(double);
        case BYTECODE_TYPE_BOOL:
            return sizeof(bool);
        case BYTECODE_TYPE_BYTE:
            return sizeof(char);
        case BYTECODE_TYPE_STRING:
        case BYTECODE_TYPE_ARRAY:
        default:
            return 0; //"size of a string/array", as well as any other type, is meaningless in this context
    }
}