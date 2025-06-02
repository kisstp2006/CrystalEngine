
#include "MetalRHI.h"
#include "MetalRHIPrivate.h"

namespace CE::Metal
{

    CommandQueue::CommandQueue(Device* device, HardwareQueueClassMask queueClassMask) : device(device)
    {
        mtlQueue = [device->GetHandle() newCommandQueue];
        
        this->queueMask = queueClassMask;
    }
    
    CommandQueue::~CommandQueue()
    {
        mtlQueue = nil;
    }

    bool CommandQueue::Execute(u32 count, RHI::CommandList** commandLists, RHI::Fence* fence)
    {
        for (int i = 0; i < count; i++)
        {
            auto cmdList = (Metal::CommandList*)commandLists[i];
            if (cmdList == nullptr)
                continue;
            
            auto metalFence = (Metal::Fence*)fence;
            if (metalFence != nullptr)
            {
                metalFence->SetCommandList(cmdList);
            }
            
            id<MTLCommandBuffer> mtlCmdBuffer = cmdList->GetMtlCommandBuffer();
            
            [mtlCmdBuffer addCompletedHandler:^(id<MTLCommandBuffer> cmdBuffer)
             {
                if (metalFence)
                {
                    metalFence->signalled = true;
                }
            }];
            
            [mtlCmdBuffer commit];
        }
        
        return true;
    }
}
