//
// Insense Virtual Machine
// Logger
//
#include "Logger.h"

log_Level_t logged_level = INFO;

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

void log_logMessage(log_Level_t level, char *source, char *message) {
    if(level >= logged_level) {
        printf("%s %s: %s\n", getLevelString(level), source, message);
    }
}

void log_setLogLevel(char* level) {
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