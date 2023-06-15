#include <makeFileSystem.h>

void getArgs(int argc, char *argv[], char *filename, unsigned short int *block_size)
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
void initializeFilesystem(unsigned short int block_size)
{
    superblock.block_size = block_size;
    superblock.root_directory_pos = sizeof(Superblock) + sizeof(FATEntry) * MAX_BLOCKS;
    superblock.fat_table_pos = sizeof(Superblock);
    superblock.data_blocks_pos = superblock.root_directory_pos + sizeof(DirectoryEntry) * MAX_DIRECTORY_ENTRIES;
    superblock.total_blocks = MAX_BLOCKS;
}

void createFileSystem(char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        return;
    }

    initializeFilesystem(MAX_BLOCK_SIZE);

    fwrite(&superblock, sizeof(Superblock), 1, file);
    printf("Superblock size: %ld\n", sizeof(Superblock));
    fwrite(fat_table, sizeof(FATEntry), MAX_BLOCKS, file);
    printf("FAT table size: %ld\n", sizeof(FATEntry) * MAX_BLOCKS);

    // Create the root directory
    DirectoryEntry root_dir;
    strcpy(root_dir.filename, "root");
    root_dir.size = 0;
    root_dir.last_mod_date = 0;
    root_dir.last_mod_time = 0;
    root_dir.start_block = 0;
    fwrite(&root_dir, sizeof(DirectoryEntry), 1, file);
    fseek(file, superblock.data_blocks_pos, SEEK_SET);
    fputc(0, file); // Set the first data block as occupied

    fwrite(directory_table, sizeof(DirectoryEntry), MAX_DIRECTORY_ENTRIES, file);
    printf("Directory table size: %ld\n", sizeof(DirectoryEntry) * MAX_DIRECTORY_ENTRIES);

    // Move the file position indicator to the end of the file to create empty data blocks
    fseek(file, superblock.total_blocks * superblock.block_size * 1024 - 1, SEEK_SET);
    fputc(0, file);

    fclose(file);
}
