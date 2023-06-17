#include <fileSystemOper.h>

int main(int argc, char *argv[])
{
    char filename[100];
    char operation[100];
    char parameters[100];
    getArgs(argc, argv, filename, operation, parameters);
    printf("Filename: %s\n", filename);
    printf("Operation: %s\n", operation);
    printf("Parameters: %s\n", parameters);
    loadFileSystem(filename);

    return 0;
}
