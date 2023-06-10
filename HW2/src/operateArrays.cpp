#include <operateArrays.h>

void getArg(int argc, char const *argv[], int *frameSize, int *pysPageSize, int *virtPageSize, char *pageRepAlgo, int *memoryAccesses, char *diskFileName)
{
    if (argc != 7)
    {
        cout << "Usage: ./operateArrays <frameSize> <physicalMemorySize> <virtualMemorySize> <pageReplacementAlgo> <memoryAccesses> <diskFileName>" << endl;
        exit(1);
    }
    *frameSize = atoi(argv[1]);
    *pysPageSize = atoi(argv[2]);
    *virtPageSize = atoi(argv[3]);
    strcpy(pageRepAlgo, argv[4]);
    *memoryAccesses = atoi(argv[5]);
    strcpy(diskFileName, argv[6]);
}

VirtualMemory::VirtualMemory(int pageSize, int frameSize)
{
    this->frameSize = pow(2, frameSize);
    this->pageSize = pow(2, pageSize);
    this->frames = new Frame[this->pageSize];
    for (int i = 0; i < this->pageSize; i++)
    {
        // call copy constructor
        this->frames[i] = Frame(frameSize);
    }
    fillVirtualMemory();
}
VirtualMemory::~VirtualMemory()
{
}
void VirtualMemory::fillVirtualMemory()
{
    int size = frameSize * pageSize;
    int frameCounter = -1;
    for (int i = 0; i < size; i++)
    {
        if (i % frameSize == 0)
        {
            frameCounter++;
        }
        frames[frameCounter].setOffset(i % frameSize, i + 1);
    }
}
VirtualMemory::Frame *VirtualMemory::getFrames()
{
    return this->frames;
}
int VirtualMemory::getFrameSize()
{
    return this->frameSize;
}
int VirtualMemory::getPageSize()
{
    return this->pageSize;
}
VirtualMemory::Frame::Frame(int frameSize)
{
    this->frameSize = frameSize;
    this->pageNumber = frameCounter;
    this->valid = false;
    this->dirty = false;
    this->offset = new int[frameSize];
    frameCounter++;
}

VirtualMemory::Frame::~Frame()
{
}
VirtualMemory::Frame::Frame()
{
}
VirtualMemory::Frame::Frame(const Frame &frame)
{
    this->frameSize = frame.frameSize;
    this->pageNumber = frame.pageNumber;
    this->valid = frame.valid;
    this->dirty = frame.dirty;
    this->offset = new int[frameSize];
    for (int i = 0; i < frameSize; i++)
    {
        this->offset[i] = frame.offset[i];
    }
}
void VirtualMemory::Frame::operator=(const Frame &frame)
{
    this->frameSize = frame.frameSize;
    this->pageNumber = frame.pageNumber;
    this->valid = frame.valid;
    this->dirty = frame.dirty;
    this->offset = new int[frameSize];
    for (int i = 0; i < frameSize; i++)
    {
        this->offset[i] = frame.offset[i];
    }
}
void VirtualMemory::Frame::setOffset(int index, int value)
{
    this->offset[index] = value;
}
int VirtualMemory::Frame::getOffset(int index)
{
    return this->offset[index];
}
void VirtualMemory::printVirtualMemory()
{
    for (int i = 0; i < pageSize; i++)
    {
        cout << "Frame " << i << ": ";
        for (int j = 0; j < frameSize; j++)
        {
            cout << frames[i].getOffset(j) << " ";
        }
        cout << endl;
    }
}

int VirtualMemory::frameCounter = 0;
PageTable::PageTable(int pageSize)
{
    this->pageSize = pow(2, pageSize);
    this->pages = new Page[this->pageSize];
}
PageTable::~PageTable()
{
}
void PageTable::mapVirtualToPhysical(int pyhsicalPageSize)
{
    int pageCounter = -1;
    // iterate through pages and map to physical memory pages
    for (int i = 0; i < pageSize; i++)
    {
        // set page number
        pages[i].setPageNumber(i);
        if (i % pyhsicalPageSize == 0)
        {
            pageCounter++;
        }
        pages[i].setFrameNumber(i % pyhsicalPageSize);
        if (pageCounter == 0)
        {
            pages[i].setPresent(true);
            // pages[i].setReferenced(true);
        }
        else
        {
            pages[i].setPresent(false);
        }
    }
}
void PageTable::Page::setPresent(bool present)
{
    this->present = present;
}
PageTable::Page::Page(/* args */)
{
}
PageTable::Page::~Page()
{
}
PageTable::Page PageTable::getPage(int index)
{
    return this->pages[index];
}
void PageTable::setPage(int index, Page page)
{
    this->pages[index] = page;
}
void PageTable::Page::setFrameNumber(int frameNumber)
{
    this->frameNumber = frameNumber;
}
int PageTable::Page::getFrameNumber()
{
    return this->frameNumber;
}
bool PageTable::Page::isPresent()
{
    return this->present;
}
void PageTable::Page::setReferenced(bool referenced)
{
    this->referenced = referenced;
}
bool PageTable::Page::isReferenced()
{
    return this->referenced;
}
int PageTable::Page::getPageNumber()
{
    return this->pageNumber;
}
void PageTable::Page::setPageNumber(int pageNumber)
{
    this->pageNumber = pageNumber;
}
void PageTable::Page::operator=(const Page &page)
{
    this->pageNumber = page.pageNumber;
    this->frameNumber = page.frameNumber;
    this->present = page.present;
    this->referenced = page.referenced;
}
PageTable::Page::Page(const Page &page)
{
    this->pageNumber = page.pageNumber;
    this->frameNumber = page.frameNumber;
    this->present = page.present;
    this->referenced = page.referenced;
}
int PageTable::pageCounter = 0;
PhysicalMemory::PhysicalMemory(int pageSize, int frameSize)
{
    this->frameSize = pow(2, frameSize);
    this->pageSize = pow(2, pageSize);
    this->frames = new Frame[this->pageSize];
    for (int i = 0; i < this->pageSize; i++)
    {
        this->frames[i] = Frame(frameSize);
    }
}

PhysicalMemory::~PhysicalMemory()
{
}
PhysicalMemory::Frame::Frame(int frameSize)
{
    this->frameSize = frameSize;
    this->pageNumber = frameCounter;
    this->valid = false;
    this->dirty = false;
    this->offset = new int[frameSize];
    frameCounter++;
}
PhysicalMemory::Frame::~Frame()
{
}
PhysicalMemory::Frame::Frame(const Frame &frame)
{
    this->frameSize = frame.frameSize;
    this->pageNumber = frame.pageNumber;
    this->valid = frame.valid;
    this->dirty = frame.dirty;
    this->offset = new int[frameSize];
    for (int i = 0; i < frameSize; i++)
    {
        this->offset[i] = frame.offset[i];
    }
}
PhysicalMemory::Frame::Frame()
{
}
void PhysicalMemory::Frame::operator=(const Frame &frame)
{
    this->frameSize = frame.frameSize;
    this->pageNumber = frame.pageNumber;
    this->valid = frame.valid;
    this->dirty = frame.dirty;
    this->offset = new int[frameSize];
    for (int i = 0; i < frameSize; i++)
    {
        this->offset[i] = frame.offset[i];
    }
}
PhysicalMemory::Frame *PhysicalMemory::getFrames()
{
    return this->frames;
}
int PhysicalMemory::getFrameSize()
{
    return this->frameSize;
}
int PhysicalMemory::getPageSize()
{
    return this->pageSize;
}
void PhysicalMemory::Frame::setOffset(int index, int value)
{
    this->offset[index] = value;
}
int PhysicalMemory::Frame::getOffset(int index)
{
    return this->offset[index];
}
void PhysicalMemory::printPhysicalMemory()
{
    for (int i = 0; i < pageSize; i++)
    {
        cout << "Frame " << i << ": ";
        for (int j = 0; j < frameSize; j++)
        {
            cout << frames[i].getOffset(j) << " ";
        }
        cout << endl;
    }
}
void PhysicalMemory::setFrame(int index, Frame &frame)
{
    this->frames[index] = frame;
}
int PhysicalMemory::frameCounter = 0;

MemoryManager::MemoryManager(int frameSize, int pysPageSize, int virtPageSize, char *pageRepAlgo, int memoryAccesses, char *diskFileName)
{
    this->frameSize = frameSize;
    this->pysPageSize = pysPageSize;
    this->virtPageSize = virtPageSize;
    strcpy(this->pageRepAlgo, pageRepAlgo);
    this->memoryAccesses = memoryAccesses;
    strcpy(this->diskFileName, diskFileName);

    this->virtualMemory = new VirtualMemory(virtPageSize, frameSize);

    this->physicalMemory = new PhysicalMemory(pysPageSize, frameSize);

    this->pageTable = new PageTable(virtPageSize);
    pageTable->mapVirtualToPhysical(physicalMemory->getPageSize());

    fillPhysicalMemoryandDisk();
}
MemoryManager::~MemoryManager()
{
}
int MemoryManager::runLinearSearch(int data)
{
    int frameSize = physicalMemory->getFrameSize();
    int virtualPageSize = virtualMemory->getPageSize();
    PhysicalMemory::Frame *physicalFrames = physicalMemory->getFrames();
    int physicalPageIndex = -1;
    int flag = 0;

    for (int i = 0; i < virtualPageSize; i++)
    {
        PageTable::Page page = pageTable->getPage(i);

        if (page.isPresent())
        {

            for (int j = 0; j < frameSize; j++)
            {
                if (physicalFrames[page.getFrameNumber()].getOffset(j) == data)
                {
                    // Data found in memory, update reference bit
                    physicalPageIndex = page.getFrameNumber();
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {
                break;
            }
            // Set reference bit to 1
            page.setReferenced(true);
            pageTable->setPage(i, page);
        }
        else
        {
            if (strcmp(pageRepAlgo, "SC") == 0)
            {
                // Second chance algorithm with reference bit in page controlling the disk file
                int virtualPageIndex = -1;
                PageTable::Page scPage;

                while (virtualPageIndex == -1)
                {
                    virtualPageIndex = secondChanceQueue.front();

                    scPage = pageTable->getPage(virtualPageIndex);
       
                    if (scPage.isReferenced())
                    {
          
                        // Set reference bit to 0 and move the page to the end of the queue
                        scPage.setReferenced(false);
                        pageTable->setPage(virtualPageIndex, scPage);
                        secondChanceQueue.pop();
                        secondChanceQueue.push(virtualPageIndex);
                        virtualPageIndex = -1; // Reset virtualPageIndex to continue searching
      
                    }
                }

                // Read frame from disk file
                readFrameFromDisk(i, frameSize, diskFileName);

                physicalPageIndex = scPage.getFrameNumber();
            }
            i--;
        }
    }
    if(flag == 0){
        return -1;
    }
    return physicalPageIndex;
}

void MemoryManager::printMemoryManager()
{
    cout << "Virtual Memory" << endl;
    virtualMemory->printVirtualMemory();
    cout << "Physical Memory" << endl;
    physicalMemory->printPhysicalMemory();
}

void MemoryManager::fillPhysicalMemoryandDisk()
{
    // int pyhsicalPageIndex = 0;
    int frameSize = this->physicalMemory->getFrameSize();
    int virtualPageSize = this->virtualMemory->getPageSize();
    VirtualMemory::Frame *virtualFrames = this->virtualMemory->getFrames();
    // int physicalPageSize = physicalMemory->getPageSize();
    PhysicalMemory::Frame *physicalFrames = this->physicalMemory->getFrames();
    for (int i = 0; i < virtualPageSize; i++)
    {
        PageTable::Page page = pageTable->getPage(i);
        if (page.isPresent())
        {
            for (int j = 0; j < frameSize; j++)
            {
                physicalFrames[page.getFrameNumber()].setOffset(j, virtualFrames[i].getOffset(j));
            }
            secondChanceQueue.push(i);
        }
        else
        {
            // write to disk
            writeFrameToDisk(virtualFrames[i], i, frameSize, diskFileName);
        }
    }
}
void MemoryManager::writeFrameToDisk(VirtualMemory::Frame &frame, int virtualIndex, int frameSize, char *diskFileName)
{
    ofstream diskFile;
    diskFile.open(diskFileName, ios::app);
    diskFile << "-";
    diskFile << virtualIndex << endl;
    for (int i = 0; i < frameSize; i++)
    {
        diskFile << frame.getOffset(i) << endl;
    }
    diskFile.close();
}
void MemoryManager::readFrameFromDisk(int virtualIndex, int frameSize, char *diskFileName)
{
    ifstream diskFile;
    diskFile.open(diskFileName);
    string line;
    PhysicalMemory::Frame frame(frameSize);
    // read line by line and find '-Index'
    while (getline(diskFile, line))
    {
        if (line == "-" + to_string(virtualIndex))
        {
            // read next frameSize lines
            for (int i = 0; i < frameSize; i++)
            {
                getline(diskFile, line);
                frame.setOffset(i, stoi(line));
            }
            break;
        }
    }
    // update physical memory using page table
    PageTable::Page page = pageTable->getPage(virtualIndex);
    page.setPresent(true);
    pageTable->setPage(virtualIndex, page);
    physicalMemory->setFrame(page.getFrameNumber(), frame);

    diskFile.close();
}
void MemoryManager::printSecondChanceQueue()
{
    queue<int> temp = secondChanceQueue;
    while (!temp.empty())
    {
        cout << temp.front() << " ";
        temp.pop();
    }
    cout << endl;
}