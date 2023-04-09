
#ifndef __MYOS__MULTIPROCESSING_H
#define __MYOS__MULTIPROCESSING_H

#include <common/types.h>
#include <gdt.h>

#define PROCESS_STATE_RUNNING 0
#define PROCESS_STATE_READY 1
#define PROCESS_STATE_BLOCKED 2

namespace myos
{

    struct CPUState
    {
        common::uint32_t eax;
        common::uint32_t ebx;
        common::uint32_t ecx;
        common::uint32_t edx;

        common::uint32_t esi;
        common::uint32_t edi;
        common::uint32_t ebp;

        /*
        common::uint32_t gs;
        common::uint32_t fs;
        common::uint32_t es;
        common::uint32_t ds;
        */
        common::uint32_t error;

        common::uint32_t eip;
        common::uint32_t cs;
        common::uint32_t eflags;
        common::uint32_t esp;
        common::uint32_t ss;
    } __attribute__((packed));

    class Process
    {
        friend class ProcessManager;

    private:
        common::uint8_t stack[4096]; // 4 KiB
        CPUState *cpustate;
        int pid;
        int ppid;
        int state;
        static int nextpid;

    public:
        Process(GlobalDescriptorTable *gdt, void entrypoint());
        Process(GlobalDescriptorTable *gdt, void entrypoint(), int ppid);
        Process(CPUState *cpustate, int ppid);
        ~Process();
        int GetPID();
        int GetPPID();
        int GetState();
        void SetState(int state);
        void SetPPID(int ppid);
        CPUState *GetCPUState();
        void SetCPUState(CPUState *cpustate);
    };

    class ProcessManager
    {
    private:
        struct ProcessControlBlock
        {
            int pid;
            int ppid;
            int state;
            CPUState *cpustate;
        };
        Process *processes[256];
        int numProcesss;
        int currentProcess;
        bool schedulingFlag;

    public:
        ProcessManager();
        ~ProcessManager();
        bool AddProcess(Process *process);
        CPUState *Schedule(CPUState *cpustate);
        Process *GetCurrentProcess();
        int GetCurrentProcessID();
        void StopScheduling();
        void StartScheduling();
        int GetNumProcesss();
    };
}

#endif