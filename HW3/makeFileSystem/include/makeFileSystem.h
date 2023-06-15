#ifndef _MAKEFILESYSTEM_H_
#define _MAKEFILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 10
#define MAX_DIRECTORY_ENTRIES 256
#define KB 1024
#define MB 1024 * KB
#define MAX_BLOCK_SIZE 4 * KB
#define MAX_BLOCKS 4096 // because of FAT12 (12 bits per entry)

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
    unsigned int size;                // Size of the file
    unsigned short int last_mod_date; // Last modification date
    unsigned short int last_mod_time; // Last modification time
    unsigned int start_block;         // Starting block of the file
} DirectoryEntry;

typedef struct
{
    unsigned short int fat_entry; // FAT entry value
} FATEntry;

Superblock superblock;
DirectoryEntry directory_table[MAX_DIRECTORY_ENTRIES];
FATEntry fat_table[MAX_BLOCKS];

void getArgs(int argc, char *argv[], char *filename, unsigned short int *block_size);
void initializeFilesystem(unsigned short int block_size);
void createFileSystem(char *filename);

#endif // _MAKEFILESYSTEM_H_
