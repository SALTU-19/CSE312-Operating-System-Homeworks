#ifndef _FILESYSTEMOPER_H_
#define _FILESYSTEMOPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 10
#define KB 1024
#define MB 1024 * KB
#define MAX_BLOCK_SIZE 4 * KB
#define MAX_BLOCKS 4 * KB // because of FAT12 (12 bits per entry)
#define MAX_DIRECTORY_ENTRIES MAX_BLOCKS / sizeof(DirectoryEntry)

typedef struct
{
    unsigned short int block_size;   // Block size in KB
    unsigned int root_directory_pos; // Position of the root directory
    unsigned int fat_table_pos;      // Position of the FAT table
    unsigned int data_blocks_pos;    // Position of the data blocks
    unsigned int total_blocks;       // Total number of blocks
} Superblock;

typedef struct
{
    char filename[MAX_FILENAME_LENGTH];
    unsigned int size;        // Size of the file
    unsigned int start_block; // Starting block of the file
} DirectoryEntry;

typedef struct
{
    unsigned int fat_entry; // FAT entry value
} FATEntry;

Superblock superblock;
FATEntry fat_table[MAX_BLOCKS];
char **fileSystem;

void getArgs(int argc, char *argv[], char *filename, char *operation, char *param1, char *param2);
void loadFileSystem(char *filename);
void handleOperation(char *operation, char *param1, char *param2);
void mkdir(unsigned int block, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], int count, int size);
void rmdir(unsigned int block, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], int count, int size);
void dir(unsigned int block, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], int count, int size);
void write(unsigned int block, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], char *linuxFile, int count, int size);
void read(unsigned int block, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], char *linuxFile, int count, int size);
void parsePath(char *path, char dirs[MAX_DIRECTORY_ENTRIES][MAX_FILENAME_LENGTH], int *count);
void readDirectoryTable(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES], unsigned int block);
void createDirectoryTable(unsigned int block);
void createFile(char *buffer, unsigned int block);
char *readFile(char *buffer, unsigned int block);
void writeDirectoryTable(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES], unsigned int block);
void putDirectoryEntry(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES], DirectoryEntry entry);
void removeDirectoryEntry(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES], int index);
int findEmptyBlock(int size);
void writeFATEntry(unsigned int block, unsigned int value);
void writeFileSystem(char *filename);
int findDirectoryEntry(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES], char *filename);
void printDirectoryTable(DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES]);
char *readLinuxFile(char *filename, char *buffer);
void writeLinuxFile(char *filename, char *buffer);

#endif // _FILESYSTEMOPER_H_