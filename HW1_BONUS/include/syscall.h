 
#ifndef __MYOS__SYSCALL_H
#define __MYOS__SYSCALL_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <multiprocessing.h>

namespace myos
{
    
    class SyscallHandler : public hardwarecommunication::InterruptHandler
    {
        
    public:
        SyscallHandler(hardwarecommunication::InterruptManager* interruptManager, myos::common::uint8_t InterruptNumber);
        ~SyscallHandler();
        
        virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);

    };
    
    
}


#endif