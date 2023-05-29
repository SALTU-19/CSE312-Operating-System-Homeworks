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

using namespace std;

class VirtualMemory
{
private:
    class VirtualPage
    {
    private:
        /* data */
        int pageNumber[256];
        int offset[256];

    public:
        VirtualPage();
        ~VirtualPage();
    };
    /* data */
    VirtualPage page[256];

public:
    VirtualMemory();
    ~VirtualMemory();
};

class PageTable
{
private:
    /* data */
    int pageFrameNumber[256];
    int reference[256];
    int present[256];
    int modified[256];
    
public:
    PageTable(/* args */);
    ~PageTable();
};