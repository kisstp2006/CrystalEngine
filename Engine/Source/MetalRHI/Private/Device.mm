#include "MetalRHIPrivate.h"

namespace CE::Metal
{

    Device::Device(MetalRHI* rhi) : rhi(rhi)
    {
        
    }

    Device::~Device()
    {
        
    }
    
    void Device::Initialize()
    {
        mtlDevice = MTLCreateSystemDefaultDevice();
        
        isInitialized = true;
        
        primaryQueue = new CommandQueue(this, RHI::HardwareQueueClassMask::All);
    }

    void Device::PreShutdown()
    {
        isInitialized = false;
    }

    void Device::Shutdown()
    {
        delete primaryQueue; primaryQueue = nullptr;
        
        mtlDevice = nil;
    }
}
