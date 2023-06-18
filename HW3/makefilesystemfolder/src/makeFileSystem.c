#include <makeFileSystem.h>

void getArgs(int argc, char *argv[], char *filename, int *block_size)
{
    if (argc != 3)
    {
        printf("Usage: %s <block_size> <filename>\n", argv[0]);
        exit(1);
    }

    *block_size = atoi(argv[1]);
    strcpy(filename, argv[2]);

    if (*block_size <= 0 || *block_size > MAX_BLOCK_SIZE)
    {
        printf("Invalid block size. Please provide a value between 1 and %d.\n", MAX_BLOCK_SIZE);
        exit(1);
    }
}
void initializeFilesystem(int block_size, int fat_table_pos, int directory_table_pos, int data_blocks_pos)
{
    superblock.block_size = block_size;
    superblock.root_directory_pos = directory_table_pos;
    superblock.fat_table_pos = fat_table_pos;
    superblock.data_blocks_pos = data_blocks_pos;
    superblock.total_blocks = MAX_BLOCKS;

    printf("Block size: %d\n", superblock.block_size);
    printf("Root directory position: %d\n", superblock.root_directory_pos);
    printf("FAT table position: %d\n", superblock.fat_table_pos);
    printf("Data blocks position: %d\n", superblock.data_blocks_pos);
    printf("Total blocks: %d\n", superblock.total_blocks);
}

void createFileSystem(char *filename, int block_size)
{

    char **fileSystem;

    int fat_table_size;
    int fat_table_pos;
    int directory_table_size;
    int directory_table_pos;
    int data_blocks_pos;

    fileSystem = (char **)malloc(sizeof(char *) * MAX_BLOCKS);
    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        fileSystem[i] = (char *)malloc(sizeof(char) * block_size);
    }

    directory_table_size = sizeof(DirectoryEntry) * MAX_DIRECTORY_ENTRIES;
    fat_table_size = sizeof(FATEntry) * MAX_BLOCKS;
    fat_table_pos = 1;
    directory_table_pos = fat_table_pos + fat_table_size / block_size;
    data_blocks_pos = directory_table_pos + directory_table_size / block_size;

    initializeFilesystem(block_size, fat_table_pos, directory_table_pos, data_blocks_pos);

    // Fill FAT table with empty entries
    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        if (i < data_blocks_pos)
            fat_table[i].fat_entry = -1;
        else
            fat_table[i].fat_entry = -2;
    }
    // Fill directory table with empty entries
    for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++)
    {
        directory_table[i].size = -1;
        directory_table[i].start_block = -1;
        strcpy(directory_table[i].filename, "");
    }

    // // Create the root directory
    // DirectoryEntry root_dir;
    // strcpy(root_dir.filename, "\\");
    // root_dir.size = 0;
    // root_dir.start_block = superblock.root_directory_pos;

    // directory_table[0].size = root_dir.size;
    // directory_table[0].start_block = root_dir.start_block;
    // strcpy(directory_table[0].filename, root_dir.filename);

    printf("Root directory size: %d\n", directory_table[0].size);
    printf("Fat table first entry: %d\n", fat_table[0].fat_entry);
    printf("Supreblock block size: %d\n", superblock.block_size);

    // Put superblock in the first block
    char *superblock_ptr = fileSystem[0];
    memcpy(superblock_ptr, &superblock, sizeof(Superblock));

    // Put FAT table in the following blocks
    char *fat_table_ptr = fileSystem[fat_table_pos];
    for (int i = 0; i < fat_table_size / block_size; i++)
    {
        for (int j = 0; j < superblock.block_size / sizeof(FATEntry); j++)
        {
            memcpy(fat_table_ptr, &fat_table[i * (superblock.block_size / sizeof(FATEntry)) + j], sizeof(FATEntry));
            fat_table_ptr += sizeof(FATEntry);
        }
    }

    // Put directory table in the following blocks
    char *directory_table_ptr = fileSystem[directory_table_pos];
    for (int i = 0; i < directory_table_size / block_size; i++)
    {
        for (int j = 0; j < superblock.block_size / sizeof(DirectoryEntry); j++)
        {
            memcpy(directory_table_ptr, &directory_table[i * (superblock.block_size / sizeof(DirectoryEntry)) + j], sizeof(DirectoryEntry));
            directory_table_ptr += sizeof(DirectoryEntry);
        }
    }

    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        return;
    }

    // Write the file system to the file
    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        fwrite(fileSystem[i], sizeof(char), block_size, file);
    }

    fclose(file);
}
