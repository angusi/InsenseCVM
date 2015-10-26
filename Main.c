#include "Main.h"


int main(int argc, char* argv[]) {
    GC_init();

    printf("%s %s\n", PROGRAM_NAME, PROGRAM_VERSION);

    if(argc < 2 || argc == 3 || argc > 4) {
        printf(PROGRAM_USAGE, argv[0]);
        return EXITCODE_INVALID_ARGUMENTS;
    }

    if(argc == 4) {
        log_setLogLevel(argv[3]);
    }

    char* mainFile = getMainFilePath(argv[1]);
    Component mainComponent = component_constructor(mainFile, NULL, 0);

    pthread_t mainThread;
    pthread_create(&mainThread, NULL, component_run, mainComponent);
    pthread_join(mainThread, NULL);

    return EXITCODE_SUCCESS;
}

char* getMainFilePath(char* directory) {
    size_t bufferSize = strlen(directory) + strlen("Main.isc") + 1;
    if(directory[strlen(directory) - 1] != PATH_SEPARATOR) {
        //Make room for the extra slash we need to add
        bufferSize += 1;
    }
    char* mainFile = GC_alloc(bufferSize, false);
    strcat(mainFile, directory);
    if(directory[strlen(directory) - 1] != PATH_SEPARATOR) {
        mainFile[strlen(directory)] = PATH_SEPARATOR;
    }
    strcat(mainFile, "Main.isc");
    strcat(mainFile, "\0");

    return mainFile;
}