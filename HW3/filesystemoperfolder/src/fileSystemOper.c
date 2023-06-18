#include <fileSystemOper.h>

void getArgs(int argc, char *argv[], char *filename, char *operation, char *param1, char *param2)
{
    if (argc <= 3)
    {
        printf("Usage: %s <filename> <operation> <param1> <param2>\n", argv[0]);
        exit(1);
    }
    strcpy(filename, argv[1]);
    strcpy(operation, argv[2]);
    strcpy(param1, argv[3]);
    if (argc > 4)
    {

        strcpy(param2, argv[4]);
    }
}
void loadFileSystem(char *filename)
{
    DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES];
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        return;
    }

    // Read superblock from the first block
    fread(&superblock, sizeof(Superblock), 1, file);

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

    // printf("Total blocks: %d\n", superblock.total_blocks);
    // printf("Block size: %d\n", superblock.block_size);
    // printf("Root directory position: %d\n", directory_table_pos);
    // printf("FAT table position: %d\n", fat_table_pos);
    // printf("Data blocks position: %d\n", data_blocks_pos);

    // printf("Root directory size: %d\n", directory_table[0].size);
    // printf("Fat table first entry: %d\n", fat_table[0].fat_entry);

    fclose(file);
}
void handleOperation(char *operation, char *param1, char *param2)
{
    if (strcmp(operation, "dir") == 0)
    {
        if (param1[0] != '/')
        {
            // print error
            printf("Invalid path.\n");
            exit(1);
        }
        char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH];
        int size = 0;
        int block;
        // parse path
        parsePath(param1, dirs, &size);
        dir(superblock.root_directory_pos, dirs, 0, size);
    }
    else if (strcmp(operation, "mkdir") == 0)
    {
        if (param1[0] != '/')
        {
            // print error
            printf("Invalid path.\n");
            exit(1);
        }
        if (param1[strlen(param1) - 1] == '/')
        {
            printf("Can not create root directory.\n");
            exit(1);
        }
        char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH];
        int size = 0;
        int block;
        // parse path
        parsePath(param1, dirs, &size);
        mkdir(superblock.root_directory_pos, dirs, 1, size);
    }
    else if (strcmp(operation, "rmdir") == 0)
    {
        if (param1[0] != '/')
        {
            // print error
            printf("Invalid path.\n");
            exit(1);
        }
        if (param1[strlen(param1) - 1] == '/')
        {
            printf("Can not delete root directory.\n");
            exit(1);
        }
        char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH];
        int size = 0;
        int block;
        // parse path
        parsePath(param1, dirs, &size);
        rmdir(superblock.root_directory_pos, dirs, 1, size);
    }
    else if (strcmp(operation, "write") == 0)
    {
        if (param1[0] != '/')
        {
            // print error
            printf("Invalid path.\n");
            exit(1);
        }
        if (param1[strlen(param1) - 1] == '/')
        {
            printf("Can not write root directory.\n");
            exit(1);
        }
        char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH];
        int size = 0;
        int block;
        // parse path
        parsePath(param1, dirs, &size);
        write(superblock.root_directory_pos, dirs, param2, 1, size);
    }
    else if (strcmp(operation, "read") == 0)
    {
        if (param1[0] != '/')
        {
            // print error
            printf("Invalid path.\n");
            exit(1);
        }
        if (param1[strlen(param1) - 1] == '/')
        {
            printf("Can not read root directory.\n");
            exit(1);
        }
        char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH];
        int size = 0;
        int block;
        // parse path
        parsePath(param1, dirs, &size);
        read(superblock.root_directory_pos, dirs, param2, 1, size);
    }
    else if (strcmp(operation, "del") == 0)
    {
        printf("del\n");
    }
    else if (strcmp(operation, "dumpe2fs") == 0)
    {
        printf("dumpe2fs\n");
    }
    else
    {
        printf("Invalid operation.\n");
    }
}
void mkdir(unsigned int block, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], int count, int size)
{
    DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES];
    // read directory table
    readDirectoryTable(directory_table, block);
    if (count == size)
    {
        if (findDirectoryEntry(directory_table, dirs[count - 1]) != -1)
        {
            printf("Directory already exists.\n");
            return;
        }
        int blockCount = MAX_DIRECTORY_ENTRIES * sizeof(DirectoryEntry) / superblock.block_size;
        int new_block = findEmptyBlock(blockCount + 1);
        if (block == -1)
        {
            printf("No empty blocks.\n");
            return;
        }
        DirectoryEntry directory_entry;
        directory_entry.start_block = new_block;
        directory_entry.size = 0;
        strcpy(directory_entry.filename, dirs[count - 1]);
        putDirectoryEntry(directory_table, directory_entry);
        writeDirectoryTable(directory_table, block);
        createDirectoryTable(new_block);
        return;
    }
    else
    {
        for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++)
        {
            if (strcmp(directory_table[i].filename, dirs[count - 1]) == 0)
            {
                mkdir(directory_table[i].start_block, dirs, count + 1, size);
                return;
            }
        }
        printf("No such directory.\n");
        return;
    }
}
void rmdir(unsigned int block, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], int count, int size)
{
    DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES];
    // read directory table
    readDirectoryTable(directory_table, block);
    if (count == size)
    {
        int index = findDirectoryEntry(directory_table, dirs[count - 1]);
        if (index == -1)
        {
            printf("There is no such directory.\n");
            return;
        }
        int start_block = directory_table[index].start_block;
        // edit Fat table
        while (1)
        {
            if (fat_table[start_block].fat_entry == -1)
            {
                writeFATEntry(start_block, -2);
                break;
            }
            else if (fat_table[start_block].fat_entry == -2)
            {
                break;
            }
            int temp = fat_table[start_block].fat_entry;
            writeFATEntry(start_block, -2);
            start_block = temp;
        }
        removeDirectoryEntry(directory_table, index);
        writeDirectoryTable(directory_table, block);
        return;
    }
    else
    {
        for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++)
        {
            if (strcmp(directory_table[i].filename, dirs[count - 1]) == 0)
            {
                rmdir(directory_table[i].start_block, dirs, count + 1, size);
                return;
            }
        }
        printf("No such directory.\n");
        return;
    }
}
void dir(unsigned int block, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], int count, int size)
{
    DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES];
    // read directory table
    readDirectoryTable(directory_table, block);
    if (count == size)
    {
        printDirectoryTable(directory_table);
        return;
    }
    else
    {
        for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++)
        {
            if (strcmp(directory_table[i].filename, dirs[count]) == 0)
            {
                dir(directory_table[i].start_block, dirs, count + 1, size);
                return;
            }
        }
        printf("No such directory.\n");
        return;
    }
}
void write(unsigned int block, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], char *linuxFile, int count, int size)
{
    DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES];
    // read directory table
    readDirectoryTable(directory_table, block);
    if (count == size)
    {
        if (findDirectoryEntry(directory_table, dirs[count - 1]) != -1)
        {
            printf("File already exists.\n");
            return;
        }
        char *buffer = readLinuxFile(linuxFile, buffer);
        int blockCount = sizeof(buffer) / superblock.block_size;
        int new_block = findEmptyBlock(blockCount + 1);
        if (block == -1)
        {
            printf("No empty blocks.\n");
            return;
        }
        DirectoryEntry directory_entry;
        directory_entry.start_block = new_block;
        directory_entry.size = strlen(buffer);
        printf("%d\n", directory_entry.size);
        strcpy(directory_entry.filename, dirs[count - 1]);
        putDirectoryEntry(directory_table, directory_entry);
        writeDirectoryTable(directory_table, block);
        createFile(buffer, new_block);
        return;
    }
    else
    {
        for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++)
        {
            if (strcmp(directory_table[i].filename, dirs[count - 1]) == 0)
            {
                write(directory_table[i].start_block, dirs, linuxFile, count + 1, size);
                return;
            }
        }
        printf("No such directory.\n");
        return;
    }
}
void read(unsigned int block, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], char *linuxFile, int count, int size)
{
    DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES];
    // read directory table
    readDirectoryTable(directory_table, block);
    if (count == size)
    {
        int index = findDirectoryEntry(directory_table, dirs[count - 1]);
        if (index == -1)
        {
            printf("There is no such file.\n");
            return;
        }
        int start_block = directory_table[index].start_block;
        char *buffer = malloc(sizeof(char) * directory_table[index].size);
        buffer = readFile(buffer, start_block);
        printf("%d\n", directory_table[index].size);
        writeLinuxFile(linuxFile, buffer);
        return;
    }
    else
    {
        for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++)
        {
            if (strcmp(directory_table[i].filename, dirs[count - 1]) == 0)
            {
                read(directory_table[i].start_block, dirs, linuxFile, count + 1, size);
                return;
            }
        }
        printf("No such directory.\n");
        return;
    }
}
void parsePath(char *path, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], int *count)
{
    // parse path using strtok '\\' and store in dirs
    char *token = strtok(path, "/");
    int i = 0;
    while (token != NULL)
    {
        strcpy(dirs[i], token);
        token = strtok(NULL, "/");
        i++;
    }
    *count = i;
}
int findEmptyBlock(int size)
{
    int count = 0;
    int start_block = -1;
    int end_block = -1;

    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        if (fat_table[i].fat_entry == -2)
        {
            writeFATEntry(i, -1);
            if (count == 0)
            {
                start_block = i;
                end_block = i;
            }
            else
            {
                writeFATEntry(end_block, i);
                end_block = i;
            }
            count++;
            if (count == size)
            {
                return start_block;
            }
        }
    }
    return -1;
}
void readDirectoryTable(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES], unsigned int block)
{
    // Read directory table from the following blocks
    char *directory_table_ptr = fileSystem[block];
    for (int i = 0; i < sizeof(DirectoryEntry) * MAX_DIRECTORY_ENTRIES / superblock.block_size; i++)
    {
        for (int j = 0; j < superblock.block_size / sizeof(DirectoryEntry); j++)
        {
            memcpy(&directory_table[i * (superblock.block_size / sizeof(DirectoryEntry)) + j], directory_table_ptr, sizeof(DirectoryEntry));
            directory_table_ptr += sizeof(DirectoryEntry);
        }
    }
}
void createDirectoryTable(unsigned int block)
{
    DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES];
    // Fill directory table with empty entries
    for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++)
    {
        directory_table[i].size = -1;
        directory_table[i].start_block = -1;
        strcpy(directory_table[i].filename, "");
    }
    // Write directory table to the following blocks
    char *directory_table_ptr = fileSystem[block];
    for (int i = 0; i < sizeof(DirectoryEntry) * MAX_DIRECTORY_ENTRIES / superblock.block_size; i++)
    {
        for (int j = 0; j < superblock.block_size / sizeof(DirectoryEntry); j++)
        {
            memcpy(directory_table_ptr, &directory_table[i * (superblock.block_size / sizeof(DirectoryEntry)) + j], sizeof(DirectoryEntry));
            directory_table_ptr += sizeof(DirectoryEntry);
        }
    }
}
void writeDirectoryTable(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES], unsigned int block)
{
    // Write directory table to the following blocks
    char *directory_table_ptr = fileSystem[block];
    for (int i = 0; i < sizeof(DirectoryEntry) * MAX_DIRECTORY_ENTRIES / superblock.block_size; i++)
    {
        for (int j = 0; j < superblock.block_size / sizeof(DirectoryEntry); j++)
        {
            memcpy(directory_table_ptr, &directory_table[i * (superblock.block_size / sizeof(DirectoryEntry)) + j], sizeof(DirectoryEntry));
            directory_table_ptr += sizeof(DirectoryEntry);
        }
    }
}
void createFile(char *buffer, unsigned int block)
{
    // Write file to the following blocks
    char *file_ptr = fileSystem[block];
    memcpy(file_ptr, buffer, strlen(buffer));
}
char *readFile(char *buffer, unsigned int block)
{
    // Read file from the following blocks
    char *file_ptr = fileSystem[block];
    memcpy(buffer, file_ptr, strlen(file_ptr));
    return buffer;
}
void putDirectoryEntry(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES], DirectoryEntry entry)
{
    int i;
    for (i = 0; i < MAX_DIRECTORY_ENTRIES; i++)
    {
        if (directory_table[i].size == -1)
        {
            directory_table[i] = entry;
            break;
        }
    }
    return;
}
void removeDirectoryEntry(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES], int index)
{
    directory_table[index].size = -1;
    directory_table[index].start_block = -1;
    strcpy(directory_table[index].filename, "");
    return;
}
int findDirectoryEntry(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES], char *filename)
{
    for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++)
    {
        if (strcmp(directory_table[i].filename, filename) == 0)
        {
            return i;
        }
    }
    return -1;
}
void writeFileSystem(char *filename)
{
    FILE *file = fopen(filename, "wb");
    // Write the file system to the file
    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        fwrite(fileSystem[i], sizeof(char), superblock.block_size, file);
    }

    fclose(file);
}
void writeFATEntry(unsigned int block, unsigned int value)
{
    fat_table[block].fat_entry = value;
    // Write FAT table to the following blocks
    char *fat_table_ptr = fileSystem[superblock.fat_table_pos];
    for (int i = 0; i < sizeof(FATEntry) * MAX_BLOCKS / superblock.block_size; i++)
    {
        for (int j = 0; j < superblock.block_size / sizeof(FATEntry); j++)
        {
            memcpy(fat_table_ptr, &fat_table[i * (superblock.block_size / sizeof(FATEntry)) + j], sizeof(FATEntry));
            fat_table_ptr += sizeof(FATEntry);
        }
    }
}
void printDirectoryTable(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES])
{
    for (int i = 0; i < MAX_DIRECTORY_ENTRIES; i++)
    {
        if (directory_table[i].size != -1)
        {
            printf("%s\n", directory_table[i].filename);
        }
    }
}
char *readLinuxFile(char *filename, char *buffer)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error: File not found\n");
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    buffer = (char *)malloc(size * sizeof(char));
    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(char), size, file);
    fclose(file);
    return buffer;
}
void writeLinuxFile(char *filename, char *buffer)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Error: File not found\n");
        exit(1);
    }
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    fclose(file);
}