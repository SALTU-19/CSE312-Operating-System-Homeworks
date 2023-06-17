#include <makeFileSystem.h>

int main(int argc, char *argv[])
{
    char filename[256];
    int block_size;
    getArgs(argc, argv, filename, &block_size);
    block_size = block_size * KB;
    createFileSystem(filename, block_size);

    return 0;
}