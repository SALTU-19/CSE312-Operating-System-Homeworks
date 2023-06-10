#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <queue>

using namespace std;

class VirtualMemory
{

public:
    class Frame
    {
    private:
        /* data */
        int frameSize;
        int pageNumber;
        bool valid;
        bool dirty;
        int *offset;

    public:
        Frame();
        Frame(int frameSize);
        Frame(const Frame &frame);
        void operator=(const Frame &frame);
        void setOffset(int index, int value);
        int getOffset(int index);
        ~Frame();
    };
    VirtualMemory(int pageSize, int frameSize);
    ~VirtualMemory();
    void fillVirtualMemory();
    Frame *getFrames();
    int getFrameSize();
    int getPageSize();
    void printVirtualMemory();

private:
    /* data */
    int frameSize;
    int pageSize;
    static int frameCounter;
    Frame *frames;
};

class PageTable
{
public:
    class Page
    {
    private:
        /* data */
        int pageNumber; // virtual page number
        int frameNumber; // pyscial frame number
        bool present;    // is the page present in the pyscial memory
        bool referenced; // is the page referenced

    public:
        Page(/* args */);
        ~Page();
        Page(const Page &page);
        void operator=(const Page &page);
        void setFrameNumber(int frameNumber);
        int getFrameNumber();
        void setPresent(bool present);
        bool isPresent();
        void setReferenced(bool referenced);
        bool isReferenced();
        int getPageNumber();
        void setPageNumber(int pageNumber);
    };
    PageTable(int pageSize);
    ~PageTable();
    void mapVirtualToPhysical(int pyhsicalPageSize);
    Page getPage(int index);
    void setPage(int index, Page page);

private:
    /* data */
    int pageSize;
    static int pageCounter;
    Page *pages;
};

class PhysicalMemory
{

public:
    class Frame
    {
    private:
        /* data */
        int frameSize;
        int pageNumber;
        bool valid;
        bool dirty;
        int *offset;

    public:
        Frame();
        Frame(int frameSize);
        ~Frame();
        Frame(const Frame &frame);
        void operator=(const Frame &frame);
        void setOffset(int index, int value);
        int getOffset(int index);
    };
    PhysicalMemory(int pageSize, int frameSize);
    ~PhysicalMemory();
    Frame *getFrames();
    void setFrame(int index, Frame &frame);
    int getFrameSize();
    int getPageSize();
    void printPhysicalMemory();

private:
    /* data */
    int frameSize;
    int pageSize;
    static int frameCounter;
    Frame *frames;
};
class MemoryManager
{
private:
    /* data */
    int memoryAccesses;
    char pageRepAlgo[256];
    char diskFileName[256];
    int frameSize;
    int pysPageSize;
    int virtPageSize;
    VirtualMemory *virtualMemory;
    PhysicalMemory *physicalMemory;
    PageTable *pageTable;
    // second chance queue
    queue<int> secondChanceQueue;

public:
    MemoryManager(int frameSize, int pysPageSize, int virtPageSize, char *pageRepAlgo, int memoryAccesses, char *diskFileName);
    ~MemoryManager();
    void fillPhysicalMemoryandDisk();
    void writeFrameToDisk(VirtualMemory::Frame &frame, int virtualIndex, int frameSize, char *diskFileName);
    void printMemoryManager();
    int runLinearSearch(int data);
    void readFrameFromDisk(int virtualIndex, int frameSize, char *diskFileName);
    void printSecondChanceQueue(); 
};

void getArg(int argc, char const *argv[], int *frameSize, int *pysPageSize, int *virtPageSize, char *pageRepAlgo, int *memoryAccesses, char *diskFileName);
