/*
 * Insense C Virtual Machine.
 *
 * An implementation in C of a Virtual Machine for Insense Bytecode programs.
 * This project builds on the work of Pitiphol Pholpabu's dissertation for MSc in Networks and Distributed Systems,
 * "A new compiler for Insense", at the University of St Andrews in 2012, as well as the DIAS Insense Project
 * by Jonathan Lewis, Alan Dearle, Pavlo Bazilinskyy, et. al, at the University of St Andrews.
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

#ifndef CVM_MAIN_H
#define CVM_MAIN_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>

int main(int argc, char* argv[]);
char* getFilePath(char* fileName);

#endif //CVM_MAIN_H
