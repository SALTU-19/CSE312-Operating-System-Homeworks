
#include <syscall.h>

using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;

SyscallHandler::SyscallHandler(InterruptManager *interruptManager, uint8_t InterruptNumber)
    : InterruptHandler(interruptManager, InterruptNumber + interruptManager->HardwareInterruptOffset())
{
}

SyscallHandler::~SyscallHandler()
{
}

CPUState *newCpu;
void printf(char *);
int fork(uint32_t esp);
int value;
uint32_t SyscallHandler::HandleInterrupt(uint32_t esp)
{
    CPUState *cpu = (CPUState *)esp;

    switch (cpu->eax)
    {
    case 2:
        // newCpu->eax = cpu->eax;
        // newCpu->ebx = cpu->ebx;
        // newCpu->ecx = cpu->ecx;
        // newCpu->edx = cpu->edx;
        // newCpu->esi = cpu->esi;
        // newCpu->edi = cpu->edi;
        // newCpu->ebp = cpu->ebp;
        // newCpu->eip = cpu->eip;
        // newCpu->cs = cpu->cs;
        // newCpu->eflags = cpu->eflags;
        value = fork(esp);
        break;
    case 4:
        printf((char *)cpu->ebx);
        break;

    default:
        break;
    }

    return esp;
}
