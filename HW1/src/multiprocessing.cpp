
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

ProcessManager::ProcessManager()
{
    numProcesss = 0;
    currentProcess = -1;
    schedulingFlag = true;
}

ProcessManager::~ProcessManager()
{
}

bool ProcessManager::AddProcess(Process *process)
{
    if (numProcesss >= 256)
        return false;
    processes[numProcesss] = process;
    numProcesss++;
    return true;
}
void ProcessManager::StartScheduling()
{
    schedulingFlag = true;
}
void ProcessManager::StopScheduling()
{
    schedulingFlag = false;
}
CPUState *ProcessManager::Schedule(CPUState *cpustate)
{
    // printf("Scheduling 1\n");
    if (!schedulingFlag)
        return cpustate;
    // printf("Scheduling 2\n");
    if (numProcesss <= 0)
        return cpustate;

    if (currentProcess >= 0)
    {
        // printf("Scheduling 3\n");
        processes[currentProcess]->cpustate = cpustate;
    }
    // printInt(processes[currentProcess]->cpustate->eip);
    // printf("\t");
    // printInt(numProcesss);
    // printf("\n");
    if (++currentProcess >= numProcesss)
    {
        // printf("Scheduling 4\n");
        currentProcess %= numProcesss;
    }
    // printf("Scheduling 5\n");
    // printInt(processes[currentProcess]->cpustate->eip);
    return processes[currentProcess]->cpustate;
}
Process *ProcessManager::GetCurrentProcess()
{
    return processes[currentProcess];
}
int ProcessManager::GetNumProcesss()
{
    return numProcesss;
}
int ProcessManager::GetCurrentProcessID()
{
    return currentProcess;
}
