#include <fileSystemOper.h>

void getArgs(int argc, char *argv[], char *filename, char *operation, char *parameters)
{
    if (argc < 3)
    {
        printf("Usage: %s <filename> <operation> <parameters>\n", argv[0]);
        exit(1);
    }
    strcpy(filename, argv[1]);
    strcpy(operation, argv[2]);
    if (argc > 3)
    {
        strcpy(parameters, argv[3]);
    }
}