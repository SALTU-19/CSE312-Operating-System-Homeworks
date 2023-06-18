#include <fileSystemOper.h>

int main(int argc, char *argv[])
{
    char filename[100];
    char operation[100];
    char param1[100];
    char param2[100];
    getArgs(argc, argv, filename, operation, param1, param2);
    loadFileSystem(filename);
    handleOperation(operation, param1, param2);
    writeFileSystem(filename);

    return 0;
}
