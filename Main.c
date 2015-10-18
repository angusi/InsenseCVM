#include "Main.h"


int main(int argc, char* argv[]) {
    GC_INIT();
    printf("%s %s\n", PROGRAM_NAME, PROGRAM_VERSION);

    if(argc != 2) {
        printf(PROGRAM_USAGE, argv[0]);
        return EXITCODE_INVALID_ARGUMENTS;
    }

    char* mainFile = getMainFilePath(argv[1]);
    Component mainComponent = component_constructor(mainFile);

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
    char* mainFile = GC_MALLOC_ATOMIC(bufferSize);
    strcat(mainFile, directory);
    if(directory[strlen(directory) - 1] != PATH_SEPARATOR) {
        mainFile[strlen(directory)] = PATH_SEPARATOR;
    }
    strcat(mainFile, "Main.isc");
    strcat(mainFile, "\0");

    return mainFile;
}