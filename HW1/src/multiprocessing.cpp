
#include <multiprocessing.h>

using namespace myos;
using namespace myos::common;

Process::Process(GlobalDescriptorTable *gdt, void entrypoint(), int ppid)
    : pid(nextpid++), ppid(ppid), state(PROCESS_STATE_READY)
{
    cpustate = (CPUState *)(stack + 4096 - sizeof(CPUState));

    cpustate->eax = 0;
    cpustate->ebx = 0;
    cpustate->ecx = 0;
    cpustate->edx = 0;

    cpustate->esi = 0;
    cpustate->edi = 0;
    cpustate->ebp = 0;

    /*
    cpustate -> gs = 0;
    cpustate -> fs = 0;
    cpustate -> es = 0;
    cpustate -> ds = 0;
    */

    // cpustate -> error = 0;

    // cpustate -> esp = ;
    cpustate->eip = (uint32_t)entrypoint;
    cpustate->cs = gdt->CodeSegmentSelector();
    // cpustate -> ss = ;
    cpustate->eflags = 0x202;
}
Process::Process(GlobalDescriptorTable *gdt, void entrypoint())
    : Process(gdt, entrypoint, 0)
{
}
Process::Process(CPUState *cpustate, int ppid)
    : pid(nextpid++), ppid(ppid), state(PROCESS_STATE_READY)
{
    this->cpustate = (CPUState *)(stack + 4096 - sizeof(CPUState));
    this->cpustate->eax = (uint32_t)cpustate->eax;
    this->cpustate->ebx = (uint32_t)cpustate->ebx;
    this->cpustate->ecx = (uint32_t)cpustate->ecx;
    this->cpustate->edx = (uint32_t)cpustate->edx;

    this->cpustate->esi = (uint32_t)cpustate->esi;
    this->cpustate->edi = (uint32_t)cpustate->edi;
    this->cpustate->ebp = (uint32_t)cpustate->ebp;

    /*
    this->cpustate -> gs = 0;
    this->cpustate -> fs = 0;
    this->cpustate -> es = 0;
    this->cpustate -> ds = 0;
    */

    // this->cpustate -> error = 0;

    // this->cpustate -> esp = ;
    this->cpustate->eip = (uint32_t)cpustate->eip;
    this->cpustate->cs = (uint32_t)cpustate->cs;
    // cpustate -> ss = ;
    this->cpustate->eflags = 0x202;
}

Process::~Process()
{
}
int Process::GetPID()
{
    return pid;
}
int Process::GetPPID()
{
    return ppid;
}
int Process::GetState()
{
    return state;
}
int Process::nextpid = 5;
CPUState *Process::GetCPUState()
{
    return cpustate;
}

ProcessManager::ProcessManager()
{
    numProcesss = 0;
    currentProcess = -1;
}

ProcessManager::~ProcessManager()
{
}

bool ProcessManager::AddProcess(Process *process)
{
    // ProcessControlBlock *pcb;
    // pcb->cpustate = process->cpustate;
    // pcb->pid = process->pid;
    // pcb->ppid = process->ppid;
    // pcb->state = process->state;
    if (numProcesss >= 256)
        return false;
    processes[numProcesss++] = process;
    return true;
}

CPUState *ProcessManager::Schedule(CPUState *cpustate)
{
    if (numProcesss <= 0)
        return cpustate;

    if (currentProcess >= 0)
    {
        processes[currentProcess]->cpustate = cpustate;
    }
    if (++currentProcess >= numProcesss)
    {
        currentProcess %= numProcesss;
    }
    return processes[currentProcess]->cpustate;
}
Process *ProcessManager::GetCurrentProcess()
{
    return processes[currentProcess];
}
