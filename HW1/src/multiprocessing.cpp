
#include <multiprocessing.h>

using namespace myos;
using namespace myos::common;

void  printf(char *str);
void  printInt(int num);

Process::Process(GlobalDescriptorTable *gdt, void *entrypoint, int ppid)
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
Process::Process(GlobalDescriptorTable *gdt, void *entrypoint)
    : Process(gdt, entrypoint, 0)
{
}
Process::Process(uint32_t esp, int ppid)
    : pid(nextpid++), ppid(ppid), state(PROCESS_STATE_READY)
{
    this->cpustate = (CPUState *)cpustate->esp;
    // this->cpustate = (CPUState *)(stack + 4096 - sizeof(CPUState));
    // this->cpustate->eax = cpustate->eax;
    // this->cpustate->ebx = cpustate->ebx;
    // this->cpustate->ecx = cpustate->ecx;
    // this->cpustate->edx = cpustate->edx;

    // this->cpustate->esi = cpustate->esi;
    // this->cpustate->edi = cpustate->edi;
    // this->cpustate->ebp = cpustate->ebp;

    // /*
    // this->cpustate -> gs = 0;
    // this->cpustate -> fs = 0;
    // this->cpustate -> es = 0;
    // this->cpustate -> ds = 0;
    // */

    // // this->cpustate -> error = 0;

    // // this->cpustate -> esp = ;
    // this->cpustate->eip = (uint32_t)cpustate->eip;
    // this->cpustate->cs = (uint32_t)cpustate->cs;
    // // cpustate -> ss = ;
    // this->cpustate->eflags = cpustate->eflags;
}
Process::Process(GlobalDescriptorTable *gdt, void *entrypoint, void **param)
    : pid(nextpid++), ppid(0), state(PROCESS_STATE_READY)
{
    cpustate = (CPUState *)(stack + 4096 - sizeof(CPUState));

    cpustate->eax = 0;
    cpustate->ebx = 0;
    cpustate->ecx = 0;
    cpustate->edx = (uint32_t)param;

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
void Process::SetCPUState(CPUState *cpustate)
{
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
    //  this->cpustate->eip = (uint32_t)cpustate->eip;
    //  this->cpustate->cs = (uint32_t)cpustate->cs;
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
    // processes[numProcesss]->pid = process->pid;
    // processes[numProcesss]->ppid = process->ppid;
    // processes[numProcesss]->state = process->state;

    // processes[numProcesss]->cpustate->eax = process->cpustate->eax;
    // processes[numProcesss]->cpustate->ebx = process->cpustate->ebx;
    // processes[numProcesss]->cpustate->ecx = process->cpustate->ecx;
    // processes[numProcesss]->cpustate->edx = process->cpustate->edx;

    // processes[numProcesss]->cpustate->esi = process->cpustate->esi;
    // processes[numProcesss]->cpustate->edi = process->cpustate->edi;
    // processes[numProcesss]->cpustate->ebp = process->cpustate->ebp;

    // processes[numProcesss]->cpustate->eip = process->cpustate->eip;
    // processes[numProcesss]->cpustate->cs = process->cpustate->cs;
    // processes[numProcesss]->cpustate->eflags = 0x202;

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
Process *ProcessManager::GetCurrentProcess(Process *process)
{
    process->pid = processes[currentProcess]->pid;
    process->ppid = processes[currentProcess]->ppid;
    process->state = processes[currentProcess]->state;

    process->cpustate->eax = (uint32_t)processes[currentProcess]->cpustate->eax;
    process->cpustate->ebx = (uint32_t)processes[currentProcess]->cpustate->ebx;
    process->cpustate->ecx = (uint32_t)processes[currentProcess]->cpustate->ecx;
    process->cpustate->edx = (uint32_t)processes[currentProcess]->cpustate->edx;

    process->cpustate->esi = (uint32_t)processes[currentProcess]->cpustate->esi;
    process->cpustate->edi = (uint32_t)processes[currentProcess]->cpustate->edi;
    process->cpustate->ebp = (uint32_t)processes[currentProcess]->cpustate->ebp;

    process->cpustate->eip = (uint32_t)processes[currentProcess]->cpustate->eip;
    process->cpustate->cs = (uint32_t)processes[currentProcess]->cpustate->cs;

    process->cpustate->eflags = (uint32_t)processes[currentProcess]->cpustate->eflags;

    return process;
}
int ProcessManager::GetNumProcesss()
{
    return numProcesss;
}
int ProcessManager::GetCurrentProcessID()
{
    return currentProcess;
}
