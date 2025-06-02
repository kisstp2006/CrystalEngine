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
    }

    void Device::PreShutdown()
    {
        isInitialized = false;
    }

    void Device::Shutdown()
    {
        mtlDevice = nil;
    }
}
