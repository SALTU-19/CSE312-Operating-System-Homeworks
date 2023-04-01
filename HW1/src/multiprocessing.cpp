
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
// set static variable
int Process::nextpid = 0;
int Process::GetPID()
{
    return pid;
}

Process::~Process()
{
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
        processs[currentProcess]->cpustate = cpustate;

    if (++currentProcess >= numProcesss)
        currentProcess %= numProcesss;
    return processs[currentProcess]->cpustate;
}
ProcessControlBlock::ProcessControlBlock(int pid, int ppid, int state, CPUState *cpustate)
{
    this->pid = pid;
    this->ppid = ppid;
    this->state = state;
    this->cpustate = cpustate;
}

CPUState *ProcessControlBlock::GetCPUState()
{
    return this->cpustate;
}
int ProcessControlBlock::GetPID()
{
    return this->pid;
}
int ProcessControlBlock::GetPPID()
{
    return this->ppid;
}
int ProcessControlBlock::GetState()
{
    return this->state;
}
ProcessTable::ProcessTable()
{
    this->numProcessControlBlocks = 0;
}
ProcessTable::~ProcessTable()
{
}
bool ProcessTable::AddProcessControlBlock(ProcessControlBlock *pcb)
{
    if (numProcessControlBlocks >= 256)
        return false;
    processControlBlocks[numProcessControlBlocks++] = pcb;
    return true;
}
ProcessControlBlock *ProcessTable::GetProcessControlBlock(int pid)
{
    for (int i = 0; i < numProcessControlBlocks; i++)
    {
        if (processControlBlocks[i]->GetPID() == pid)
            return processControlBlocks[i];
    }
    return 0;
}
