#pragma once

namespace CE::Metal
{

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
        
    private:
        
        MetalRHI* rhi = nullptr;
        bool isInitialized = false;
        
        // - ObjC -
        
        id<MTLDevice> mtlDevice = nil;
    };

}

