
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
int Process::nextpid = 0;
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
    if (numProcesss >= 256)
        return false;
    processs[numProcesss++] = process;
    return true;
}

CPUState *ProcessManager::Schedule(CPUState *cpustate)
{
    if (numProcesss <= 0)
        return cpustate;

    if (currentProcess >= 0)
    {
        processs[currentProcess]->cpustate = cpustate;
        processs[currentProcess - 1]->state = PROCESS_STATE_BLOCKED;
        processs[currentProcess]->state = PROCESS_STATE_RUNNING;
        processs[currentProcess + 1]->state = PROCESS_STATE_READY;
    }
    if (++currentProcess >= numProcesss)
    {
        currentProcess %= numProcesss;
    }
    return processs[currentProcess]->cpustate;
}

ProcessTable::ProcessTable()
{
    numProcesss = 0;
}
ProcessTable::~ProcessTable()
{
}
bool ProcessTable::AddProcess(Process *process)
{
    int pid = process->GetPID();
    int ppid = process->GetPPID();
    int state = process->GetState();
    CPUState *cpustate = process->GetCPUState();
    ProcessControlBlock *pcb;
    pcb->cpustate = cpustate;
    pcb->pid = pid;
    pcb->ppid = ppid;
    pcb->state = state;
    if (numProcesss >= 256)
        return false;
    processsControlBlocks[numProcesss++] = pcb;
    return true;
}
int ProcessTable::GetProcessState(int pid)
{
    for (int i = 0; i < numProcesss; i++)
    {
        if (processsControlBlocks[i]->pid == pid)
            return processsControlBlocks[i]->state;
    }
    return -1;
}
void ProcessTable::SetProcessState(int pid, int state)
{
    for (int i = 0; i < numProcesss; i++)
    {
        if (processsControlBlocks[i]->pid == pid)
            processsControlBlocks[i]->state = state;
    }
}
int ProcessTable::GetNumProcesss()
{
    return numProcesss;
}
