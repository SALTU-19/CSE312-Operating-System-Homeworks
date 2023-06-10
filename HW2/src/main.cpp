#include "operateArrays.h"

int main(int argc, char const *argv[])
{
    int frameSize, pysPageSize, virtPageSize, memoryAccesses;
    char diskFileName[256], pageRepAlgo[256];
    getArg(argc, argv, &frameSize, &pysPageSize, &virtPageSize, pageRepAlgo, &memoryAccesses, diskFileName);

    MemoryManager memoryManager(frameSize, pysPageSize, virtPageSize, pageRepAlgo, memoryAccesses, diskFileName);
    memoryManager.printMemoryManager();
    cout << endl;
    cout << "Running Linear Search..." << endl;
    cout << endl;
    int data = 53;
    cout << "Searching for " << data << "..." << endl;
    cout << endl;
    int index = memoryManager.runLinearSearch(data);
    cout << endl;
    memoryManager.printMemoryManager();
    cout << endl;
    if (index != -1)
        cout << "Index of " << data << " is " << index << endl;
    else
        cout << "Index of " << data << " is not found" << endl;
    return 0;
}
