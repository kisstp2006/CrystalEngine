#pragma once

namespace CE::Metal
{
    class CommandQueue;

    class Device
    {
        CE_NO_COPY(Device);
    public:
        
        Device(MetalRHI* rhi);
        ~Device();
        
        bool IsInitialized() const
        {
            return isInitialized;
        }

        void Initialize();
        void PreShutdown();
        void Shutdown();
        
        id<MTLDevice> GetHandle() const { return mtlDevice; }
        
        CommandQueue* GetPrimaryQueue() const { return primaryQueue; }
        
    private:
        
        MetalRHI* rhi = nullptr;
        bool isInitialized = false;
        
        CommandQueue* primaryQueue = nullptr;
        
        // - ObjC -
        
        id<MTLDevice> mtlDevice = nil;
        
    };

}

