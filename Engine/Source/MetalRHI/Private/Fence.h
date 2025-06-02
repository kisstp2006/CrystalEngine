#pragma once

namespace CE::Metal
{

    class Fence : public RHI::Fence
    {
    public:
        
        Fence(Device* device, bool initiallySignalled);
        virtual ~Fence();
        
        void Reset() override;
        
        void WaitForFence() override;
        
        bool IsSignalled() override;
        
        void SetCommandList(CommandList* cmdList);

    private:
        
        Device* device = nullptr;
        CommandList* cmdList = nullptr;
        
        bool signalled = false;
        
        friend class CommandList;
        friend class CommandQueue;
    
    };
    
} // namespace CE::Metal

