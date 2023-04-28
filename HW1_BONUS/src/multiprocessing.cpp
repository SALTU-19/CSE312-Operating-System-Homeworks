
#include <multiprocessing.h>

using namespace myos;
using namespace myos::common;

void printf(char *str);
void printInt(int num);

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
    this->SetCPUState(cpustate);
}
Process::~Process()
{
}
int Process::GetPID()
{
    return pid;
}
void Process::SetPPID(int ppid)
{
    this->ppid = ppid;
}
int Process::GetPPID()
{
    return ppid;
}
void Process::SetState(int state)
{
    this->state = state;
}
int Process::GetState()
{
    return state;
}
int Process::nextpid = 1;
CPUState *Process::GetCPUState()
{
    return cpustate;
}
void Process::SetCPUState(CPUState *cpustate)
{
    this->cpustate->eax = (uint32_t)cpustate->eax;
    this->cpustate->ebx = (uint32_t)cpustate->ebx;
    this->cpustate->ecx = (uint32_t)cpustate->ecx;
    this->cpustate->edx = (uint32_t)cpustate->edx;

    // this->cpustate->esi = 0;
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
    // this->cpustate->eip = (uint32_t)cpustate->eip;
    // this->cpustate->cs = (uint32_t)cpustate->cs;
    // cpustate -> ss = ;
    this->cpustate->eflags = (uint32_t)cpustate->eflags;
}

ProcessTable::ProcessTable()
{
    numProcesss = 0;
    currentProcess = -1;
    schedulingFlag = true;
}

ProcessTable::~ProcessTable()
{
}

bool ProcessTable::AddProcess(Process *process)
{
    if (numProcesss >= 256)
        return false;
    processes[numProcesss] = process;
    processes[numProcesss]->SetState(PROCESS_STATE_RUNNING);
    numProcesss++;
    return true;
}
Process *ProcessTable::GetProcess(int pid)
{
    for (int i = 0; i < numProcesss; i++)
    {
        if (processes[i]->GetPID() == pid)
            return processes[i];
    }
    return 0;
}
bool ProcessTable::UpdateProcessState(int pid, int state)
{
    for (int i = 0; i < numProcesss; i++)
    {
        if (processes[i]->GetPID() == pid)
        {
            processes[i]->SetState(state);
            return true;
        }
    }
    return false;
}
void ProcessTable::StartScheduling()
{
    schedulingFlag = true;
}
void ProcessTable::StopScheduling()
{
    schedulingFlag = false;
}
CPUState *ProcessTable::Schedule(CPUState *cpustate)
{

    if (numProcesss <= 0)
        return cpustate;

    if (currentProcess >= 0)
    {
        processes[currentProcess]->cpustate = cpustate;
    }

    // print current process cpu state
    // printf("current process: ");
    // printInt(currentProcess);
    // printf(" eax: ");
    // printInt(cpustate->eax);
    // printf(" ebx: ");
    // printInt(cpustate->ebx);
    // printf(" ecx: ");
    // printInt(cpustate->ecx);
    // printf(" edx: ");
    // printInt(cpustate->edx);
    // printf(" edi: ");
    // printInt(cpustate->edi);
    // printf(" ebp: ");
    // printInt(cpustate->ebp);
    // printf(" eip: ");
    // printInt(cpustate->eip);
    // printf(" eflags: ");
    // printInt(cpustate->eflags);

    if (++currentProcess >= numProcesss)
    {
        currentProcess %= numProcesss;
    }
    while (processes[currentProcess]->GetState() != PROCESS_STATE_RUNNING)
    {
        if (++currentProcess >= numProcesss)
        {
            currentProcess %= numProcesss;
        }
    }
    return processes[currentProcess]->cpustate;
}
Process *ProcessTable::GetCurrentProcess()
{
    return processes[currentProcess];
}
int ProcessTable::GetNumProcesss()
{
    return numProcesss;
}
int ProcessTable::GetCurrentProcessID()
{
    return currentProcess;
}
