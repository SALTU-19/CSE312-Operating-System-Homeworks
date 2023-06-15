#include <makeFileSystem.h>

int main(int argc, char *argv[])
{
    char filename[256];
    unsigned short int block_size;
    getArgs(argc, argv, filename, &block_size);
    createFileSystem(filename);

    return 0;
}