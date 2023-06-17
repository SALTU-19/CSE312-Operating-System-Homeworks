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

    // Create the root directory
    DirectoryEntry root_dir;
    strcpy(root_dir.filename, "\\");
    root_dir.size = 0;
    root_dir.last_mod_date = 0;
    root_dir.last_mod_time = 0;
    root_dir.start_block = 0;

    directory_table[0] = root_dir;

    // put superblock in the first block
    memcpy(fileSystem[0], &superblock, sizeof(Superblock));
    // put FAT table in the following blocks
    memcpy(fileSystem[fat_table_pos], fat_table, fat_table_size);

    // put directory table in the following blocks
    memcpy(fileSystem[directory_table_pos], directory_table, directory_table_size);

    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        return;
    }
    // write the file system to the file
    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        fwrite(fileSystem[i], sizeof(char), block_size, file);
    }

    fclose(file);
}
