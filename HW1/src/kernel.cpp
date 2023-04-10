
#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multiprocessing.h>
#include <syscall.h>

// #define GRAPHICSMODE

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::gui;

ProcessTable processTable;
GlobalDescriptorTable gdt;
void BinarySearch();
void LinearSearch();
void Collatz();
void forkProcess();
void firstStrategy();
void secondStrategy();
void finalStrategy();
Process *processArray[10];
int index = 0;
void printf(char *str)
{
    static uint16_t *VideoMemory = (uint16_t *)0xb8000;

    static uint8_t x = 0, y = 0;

    for (int i = 0; str[i] != '\0'; ++i)
    {
        switch (str[i])
        {
        case '\n':
            x = 0;
            y++;
            break;
        default:
            VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
            x++;
            break;
        }

        if (x >= 80)
        {
            x = 0;
            y++;
        }

        if (y >= 25)
        {
            for (y = 0; y < 25; y++)
                for (x = 0; x < 80; x++)
                    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

void printfHex(uint8_t key)
{
    char *foo = "00";
    char *hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}
int strcpy(char *dest, char *src)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return i;
}
void printInt(int num)
{
    char *foo;
    strcpy(foo, "0000");
    int i = 0;
    int j = 4;
    // if the number is negative
    if (num < 0)
    {
        foo[0] = '-';
        num *= -1;
    }
    while (num > 0)
    {
        foo[j - i] = num % 10 + '0';
        num /= 10;
        i++;
    }
    printf(foo);
    strcpy(foo, "0000");
}
void sysprintf(char *str)
{
    asm("int $0x80"
        :
        : "a"(4), "b"(str));
}
int fork(CPUState *cpu, int index)
{
    processTable.StopScheduling();
    int pid = processTable.GetCurrentProcess()->GetPID();
    processArray[index]->SetCPUState(cpu);
    processArray[index]->SetPPID(pid);
    processTable.AddProcess(processArray[index]);
    processTable.StartScheduling();
    return processArray[index]->GetPID();
}

int sysfork()
{
    asm("int $0x80"
        :
        : "a"(2), "b"(index));

    // read eax register and return the value
    int eax = 0;
    asm("mov %%eax, %0"
        : "=r"(eax));
    return eax;
}
int waitpid(int pid)
{
    if (processTable.GetProcess(pid)->GetState() == PROCESS_STATE_BLOCKED)
    {

        return 0;
    }
    else
    {
        return -1;
    }
}
int syswaitpid(int pid)
{
    asm("int $0x80"
        :
        : "a"(7), "b"(pid));
    int eax = 0;
    asm("mov %%eax, %0"
        : "=r"(eax));
    return eax;
}
void forkProcess()
{
    int value = 0;
    if (processTable.GetCurrentProcess()->GetPPID() == 0)
        value = sysfork();
    while (1)
    {
        if (value == 0)
        {

            sysprintf("child\n");
        }
        else
        {

            sysprintf("parent\n");
        }
    }
}
void BinarySearch()
{

    int arr[10] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170};
    int key = 110, start = 0, end = 9, mid;
    int pid = processTable.GetCurrentProcess()->GetPID();
    bool found = false;
    while (1)
    {
        while (start <= end)
        {
            mid = (start + end) / 2;
            if (arr[mid] == key)
            {
                // Element found
                found = true;
            }
            else if (arr[mid] < key)
            {
                // Look in the right half
                start = mid + 1;
            }
            else
            {
                // Look in the left half
                end = mid - 1;
            }
        }
        if (!found)
        {
            printf("Binary Search Element found at index: ");
            printInt(mid);
            printf("\n");
        }
        else
        {
            printf("Binary Search Element found in the array \n");
        }
        for (int i = 0; i < 100000000; i++)
            ;
        processTable.UpdateProcessState(pid, PROCESS_STATE_BLOCKED);
    }
}
void LinearSearch()
{
    int pid = processTable.GetCurrentProcess()->GetPID();
    int arr[10] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170};
    int key = 110;
    bool found = false;
    int i;
    while (1)
    {
        for (i = 0; i < 10; i++)
        {
            if (arr[i] == key)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            printf("Liner Search Element not found in the array \n");
        }
        else
        {
            printf("Liner Search Element found at index: ");
            printInt(i);
            printf("\n");
        }
        for (int i = 0; i < 100000000; i++)
            ;
        processTable.UpdateProcessState(pid, PROCESS_STATE_BLOCKED);
    }
}
void Collatz()
{
    int pid = processTable.GetCurrentProcess()->GetPID();
    int num = 10;

    while (1)
    {
        while (num != 1)
        {
            if (num % 2 == 0)
            {
                num /= 2;
            }
            else
            {
                num = num * 3 + 1;
            }
            printInt(num);
            printf(" ");
        }
        printf("Collatz Sequence: ");
        printInt(num);
        printf(" ");
        printf("\n");
        for (int i = 0; i < 100000000; i++)
            ;
        processTable.UpdateProcessState(pid, PROCESS_STATE_BLOCKED);
    }
}
Process processA(&gdt, BinarySearch);
Process processB(&gdt, LinearSearch);
Process processC(&gdt, Collatz);
void firstStrategy()
{
    int num = 0;
    processTable.AddProcess(&processA);
    processTable.AddProcess(&processB);
    processTable.AddProcess(&processC);
    int pidA = processA.GetPID();
    int pidB = processB.GetPID();
    int pidC = processC.GetPID();
    bool terminated = false;
    num = processTable.GetNumProcesss();
    while (1)
    {
        if (terminated)
        {
            // sysprintf("Process Number: ");
            // printInt(num);
            // sysprintf("         All processes terminated\n");
            break;
        }
        else
        {
            sysprintf("All processes running\n");
            if (syswaitpid(pidA) == 0)
            {
                sysprintf("Binary Search terminated\n");
                terminated = true;
            }
            else
            {
                sysprintf("Binary Search running\n");
                terminated = false;
            }
            if (syswaitpid(pidB) == 0)
            {
                sysprintf("Linear Search terminated\n");
                terminated = true;
            }
            else
            {
                sysprintf("Linear Search running\n");
                terminated = false;
            }
            if (syswaitpid(pidC) == 0)
            {
                sysprintf("CollatZ terminated\n");
                terminated = true;
            }
            else
            {
                sysprintf("CollatZ running\n");
                terminated = false;
            }
        }
    }
}

void secondStrategy()
{
    int num = 0;
    bool terminated = false;
    int index = 0;
    for (int i = 0; i < 10; i++)
    {
        *processArray[i] = Process(&gdt, LinearSearch);
    }
    for (int i = 0; i < 10; i++)
    {
        index = i;
        sysfork();
    }
    //  num = processTable.GetNumProcesss();
    while (1)
    {

        if (terminated)
        {
            // sysprintf("Process Number: ");
            // printInt(num);
            // sysprintf("         All processes terminated\n");
            break;
        }
        else
        {
            sysprintf("All processes running\n");
            for (int i = 0; i < 10; i++)
            {
                if (syswaitpid(processArray[i]->GetPID()) == 0)
                {
                    sysprintf("Linear Search terminated\n");
                    terminated = true;
                }
                else
                {
                    sysprintf("Linear Search running\n");
                    terminated = false;
                }
            }
        }
    }
}
void finalStrategy()
{
    int num = 0;
    bool terminated = false;
    int index = 0;
    for (int i = 0; i < 6; i++)
    {
        if (i < 3)
            *processArray[i] = Process(&gdt, LinearSearch);
        else
            *processArray[i] = Process(&gdt, BinarySearch);
    }
    for (int i = 0; i < 6; i++)
    {
        index = i;
        sysfork();
    }
    // num = processTable.GetNumProcesss();
    while (1)
    {
        if (terminated)
        {
            // sysprintf("Process Number: ");
            // printInt(num);
            // sysprintf("         All processes terminated\n");
            break;
        }
        else
        {
            sysprintf("All processes running\n");
            for (int i = 0; i < 6; i++)
            {
                if (syswaitpid(processArray[i]->GetPID()) == 0)
                {
                    terminated = true;
                }
                else
                {
                    terminated = false;
                }
            }
        }
    }
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(char c)
    {
        char *foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;

public:
    MouseToConsole()
    {
        uint16_t *VideoMemory = (uint16_t *)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 | (VideoMemory[80 * y + x] & 0xF000) >> 4 | (VideoMemory[80 * y + x] & 0x00FF);
    }

    virtual void OnMouseMove(int xoffset, int yoffset)
    {
        static uint16_t *VideoMemory = (uint16_t *)0xb8000;
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 | (VideoMemory[80 * y + x] & 0xF000) >> 4 | (VideoMemory[80 * y + x] & 0x00FF);

        x += xoffset;
        if (x >= 80)
            x = 79;
        if (x < 0)
            x = 0;
        y += yoffset;
        if (y >= 25)
            y = 24;
        if (y < 0)
            y = 0;

        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 | (VideoMemory[80 * y + x] & 0xF000) >> 4 | (VideoMemory[80 * y + x] & 0x00FF);
    }
};

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void kernelMain(const void *multiboot_structure, uint32_t /*multiboot_magic*/)
{
    printf("Hello World! --- http://www.AlgorithMan.de\n");

    Process init(&gdt, firstStrategy);
    processTable.AddProcess(&init);

    // Process init(&gdt, secondStrategy);
    // processTable.AddProcess(&init);

    // Process init(&gdt, finalStrategy);
    // processTable.AddProcess(&init);

    InterruptManager interrupts(0x20, &gdt, &processTable);
    SyscallHandler syscalls(&interrupts, 0x80);

    printf("Initializing Hardware, Stage 1\n");

#ifdef GRAPHICSMODE
    Desktop desktop(320, 200, 0x00, 0x00, 0xA8);
#endif

    DriverManager drvManager;

#ifdef GRAPHICSMODE
    KeyboardDriver keyboard(&interrupts, &desktop);
#else
    PrintfKeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);
#endif
    drvManager.AddDriver(&keyboard);

#ifdef GRAPHICSMODE
    MouseDriver mouse(&interrupts, &desktop);
#else
    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);
#endif
    drvManager.AddDriver(&mouse);

    PeripheralComponentInterconnectController PCIController;
    PCIController.SelectDrivers(&drvManager, &interrupts);

    VideoGraphicsArray vga;

    printf("Initializing Hardware, Stage 2\n");
    drvManager.ActivateAll();

    printf("Initializing Hardware, Stage 3\n");

#ifdef GRAPHICSMODE
    vga.SetMode(320, 200, 8);
    Window win1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
    desktop.AddChild(&win1);
    Window win2(&desktop, 40, 15, 30, 30, 0x00, 0xA8, 0x00);
    desktop.AddChild(&win2);
#endif

    interrupts.Activate();

    while (1)
    {
#ifdef GRAPHICSMODE
        desktop.Draw(&vga);
#endif
    }
}
