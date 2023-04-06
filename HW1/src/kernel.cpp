
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

int fork()
{
    Process *parentProcess = processManager.GetCurrentProcess();
    CPUState *parentCPUState = (CPUState *)parentProcess->GetCPUState();
    Process childProcess(parentCPUState, parentProcess->GetPID());

    // processManager.AddProcess(&childProcess);

    return parentProcess->GetPID();
}

void sysfork()
{
    asm("int $0x80"
        :
        : "a"(2));
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

void sysprintf(char *str)
{
    asm("int $0x80"
        :
        : "a"(4), "b"(str));
}

void processA()
{
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
        // end of critical section
        flag[0] = false;
    }
}
void processB()
{

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
        sysprintf(b);
        // end of critical section
        flag[1] = false;
    }
}
void processC()
{
    // while (1)
    for (int i = 0; i < 40; i++)
        sysprintf("Process C ");
}
void processD()
{

    // while (1)
    for (int i = 0; i < 10; i++)
        sysprintf("Process D ");
}

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

    char a;
    char b;
    char c;
    Process process1(&gdt, processA);
    Process process2(&gdt, processB);

    processManager.AddProcess(&process1);
    for (int i = 0; i < 1000; i++)
    {
        printf("a\n");
        printf("b\n");
        printf("c\n");
        printf("d\n");
        printf("e\n");
        printf("f\n");
        printf("g\n");
    }
    Process process3(&gdt, processB);
    processManager.AddProcess(&process3);
    // a = '0' + process1.GetPID();
    // b = '0' + process2.GetPID();
    // c = '0' + fork();

    // printf("Process 1 PID: ");
    // printf(&a);
    // printf("\n");
    // printf("Process 2 PID: ");
    // printf(&b);
    // printf("\n");

    // printf("Process 3 PID: ");
    // printf(&c);
    // printf("\n");

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
