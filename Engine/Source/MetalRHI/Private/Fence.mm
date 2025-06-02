#include "MetalRHIPrivate.h"

namespace CE::Metal
{

    Fence::Fence(Device* device, bool initiallySignalled)
        : device(device), signalled(initiallySignalled)
    {
        
    }

    Fence::~Fence() noexcept
    {
        if (cmdList)
        {
            cmdList->DeregisterFence(this);
        }
        cmdList = nullptr;
    }

    void Fence::Reset()
    {
        signalled = false;
    }

    void Fence::WaitForFence()
    {
        if (cmdList)
        {
            [cmdList->GetMtlCommandBuffer() waitUntilCompleted];
        }
        signalled = true;
    }

    bool Fence::IsSignalled()
    {
        return cmdList == nullptr || signalled;
    }

    void Fence::SetCommandList(CommandList* cmdList)
    {
        if (this->cmdList == cmdList)
        {
            return;
        }
        
        if (this->cmdList)
        {
            this->cmdList->DeregisterFence(this);
        }
        
        this->cmdList = cmdList;
        
        if (cmdList)
        {
            cmdList->RegisterFence(this);
        }
    }
    
} // namespace CE::Metal
