#include <fileSystemOper.h>

int main(int argc, char *argv[])
{
    char filename[100];
    char operation[100];
    char parameters[100];
    getArgs(argc, argv, filename, operation, parameters);
    loadFileSystem(filename);
    handleOperation(operation, parameters);
    writeFileSystem(filename);

    return 0;
}
