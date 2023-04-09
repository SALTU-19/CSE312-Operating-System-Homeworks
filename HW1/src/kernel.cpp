
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

bool flag[2] = {false, false};
int turn;
int process1State = 0;
int process2State = 0;
ProcessManager processManager;
GlobalDescriptorTable gdt;
int currentProcess = -2;
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
    strcpy(foo, "0000000000000000");
    int i = 0;
    int j = 15;
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
    strcpy(foo, "0000000000000000");
}
void sysprintf(char *str)
{
    asm("int $0x80"
        :
        : "a"(4), "b"(str));
}
void processA();
void processB();
void processC();
void processD();
void BinarySearch();
Process childProcess(&gdt, processC);
int fork(CPUState *cpu)
{
    childProcess.SetCPUState(cpu);
    childProcess.SetPPID(processManager.GetCurrentProcess()->GetPID());
    processManager.AddProcess(&childProcess);
    return 0;
}

int sysfork()
{
    asm("int $0x80"
        :
        : "a"(2));

    // read eax register and return the value
    int eax = 0;
    asm("mov %%eax, %0"
        : "=r"(eax));
    return eax;
}
void processA()
{
    // int b = sysfork();
    while (1)
    {
        flag[0] = true;
        turn = 1;
        while (flag[1] == true && turn == 1)
        {
            // busy wait
        }

        // critical section
        char *a = "Coskun ";
        sysprintf(a);
        // printInt(b);
        // end of critical section
        flag[0] = false;
    }
}
void processB()
{
    // fork();
    while (1)
    {
        flag[1] = true;
        turn = 0;
        while (flag[0] == true && turn == 0)
        {
            // busy wait
        }
        // critical section
        char *b = "Saltu\n";
        // printInt(currentProcess);
        sysprintf(b);

        // end of critical section
        flag[1] = false;
    }
}
void processC()
{
    
    int value = sysfork();
    while (1)
    {
        if(value == 0)
        {
            sysprintf("child\n");
        }
        else
        {
            sysprintf("parent\n");
        }
    }
}
void processD()
{

    while (1)
        // for (int i = 0; i < 10; i++)
        sysprintf("b\n");
}
void BinarySearch()
{
    int arr[10] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170};
    int key = 110;
    int start = 0;
    int end = 9;
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

    Process process1(&gdt, processC);
    Process process2(&gdt, processD);

    processManager.AddProcess(&process1);
    processManager.AddProcess(&process2);

    InterruptManager interrupts(0x20, &gdt, &processManager);
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
