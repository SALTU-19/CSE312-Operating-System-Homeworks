#ifndef _MAKEFILESYSTEM_H_
#define _MAKEFILESYSTEM_H_

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
DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES];
FATEntry fat_table[MAX_BLOCKS];

void getArgs(int argc, char *argv[], char *filename, int *block_size);
void initializeFilesystem(int block_size, int fat_table_pos, int directory_table_pos, int data_blocks_pos);
void createFileSystem(char *filename, int block_size);
void loadFileSystem(char *filename);

#endif // _MAKEFILESYSTEM_H_
