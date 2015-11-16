/*
 * Logger.
 *
 * Methods here are used for managing logging output.
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
#include <stdarg.h>
#include <pthread.h>
#include "Logger.h"

log_Level_t logged_level = INFO;

pthread_mutex_t* logMutex;

void log_init() {
    if(logMutex == NULL) {
        logMutex = malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(logMutex, NULL);
        printf("Logging subsystem initialised\n");
    }
}

char* getLevelString(log_Level_t level) {
    switch(level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case FATAL:
            return "FATAL";
        default:
            return "LOG";
    }
}

void log_vf_logMessage(log_Level_t level, const char* source, const char* message, va_list argp) {
    if(level >= logged_level) {
        pthread_mutex_lock(logMutex);
        printf("%s - %s: ", getLevelString(level), source);
        vfprintf(stdout, message, argp);
        printf("\n");
        pthread_mutex_unlock(logMutex);
    }
}

void log_logMessage(log_Level_t level, const char *source, const char *message, ...) {
    va_list argp;
    va_start(argp, message);
    log_vf_logMessage(level, source, message, argp);
    va_end(argp);
}

void log_setLogLevel(const char* level) {
    //Each comparison is negated, since strcmp returns 0 (boolean FALSE) for match!
    if(!strcmp(level,"DEBUG")) {
        logged_level = DEBUG;
    } else if(!strcmp(level,"INFO")) {
        logged_level = INFO;
    } else if(!strcmp(level,"WARNING")) {
        logged_level = WARNING;
    } else if(!strcmp(level,"ERROR")) {
        logged_level = ERROR;
    } else if(!strcmp(level,"FATAL")) {
        logged_level = FATAL;
    } else {
        log_logMessage(FATAL, "Logger", "Unknown Log Level");
        exit(EXITCODE_UNKNOWN_LOG_LEVEL);
    }
}