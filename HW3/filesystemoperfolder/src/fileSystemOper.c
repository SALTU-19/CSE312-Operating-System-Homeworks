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
void loadFileSystem(char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        return;
    }

    // Read superblock from the first block
    fread(&superblock, sizeof(Superblock), 1, file);

    char **fileSystem;

    int fat_table_size;
    int fat_table_pos;
    int directory_table_size;
    int directory_table_pos;
    int data_blocks_pos;

    fileSystem = (char **)malloc(sizeof(char *) * MAX_BLOCKS);
    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        fileSystem[i] = (char *)malloc(sizeof(char) * superblock.block_size);
    }

    directory_table_size = sizeof(DirectoryEntry) * MAX_DIRECTORY_ENTRIES;
    fat_table_size = sizeof(FATEntry) * MAX_BLOCKS;
    fat_table_pos = superblock.fat_table_pos;
    directory_table_pos = superblock.root_directory_pos;
    data_blocks_pos = superblock.data_blocks_pos;

    // Read the file system from the file
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        fread(fileSystem[i], sizeof(char), superblock.block_size, file);
    }

    // Read superblock from the first block
    memcpy(&superblock, fileSystem[0], sizeof(Superblock));

    // Read FAT table from the following blocks
    char *fat_table_ptr = fileSystem[fat_table_pos];
    for (int i = 0; i < fat_table_size / superblock.block_size; i++)
    {
        for (int j = 0; j < superblock.block_size / sizeof(FATEntry); j++)
        {
            memcpy(&fat_table[i * (superblock.block_size / sizeof(FATEntry)) + j], fat_table_ptr, sizeof(FATEntry));
            fat_table_ptr += sizeof(FATEntry);
        }
    }

    // Read directory table from the following blocks
    char *directory_table_ptr = fileSystem[directory_table_pos];
    for (int i = 0; i < directory_table_size / superblock.block_size; i++)
    {
        for (int j = 0; j < superblock.block_size / sizeof(DirectoryEntry); j++)
        {
            memcpy(&directory_table[i * (superblock.block_size / sizeof(DirectoryEntry)) + j], directory_table_ptr, sizeof(DirectoryEntry));
            directory_table_ptr += sizeof(DirectoryEntry);
        }
    }

    printf("Total blocks: %d\n", superblock.total_blocks);
    printf("Block size: %d\n", superblock.block_size);
    printf("Root directory position: %d\n", directory_table_pos);
    printf("FAT table position: %d\n", fat_table_pos);
    printf("Data blocks position: %d\n", data_blocks_pos);

    // Print root directory first index
    printf("Root directory filename: %s\n", directory_table[0].filename);

    fclose(file);
}