#include "MetalRHIPrivate.h"

namespace CE::Metal
{

    ShaderModule::ShaderModule(Device* device, const RHI::ShaderModuleDescriptor& desc)
        : device(device)
    {
        this->stage = desc.stage;
        this->name = desc.debugName;
        this->isValid = false;

        if (desc.byteSize == 0 || desc.byteCode == nullptr)
            return;
        
        NSString* source = [NSString stringWithCString:(char*)desc.byteCode];
        
        MTLCompileOptions* options = [[MTLCompileOptions alloc] init];
        
        NSError* error = nil;
        
        mtlLibrary = [device->GetHandle() newLibraryWithSource:source options:options error:&error];
        if (error)
        {
            CE_LOG(Info, All, "ShaderModule MTLLibrary failed!");
            return;
        }
    }

    ShaderModule::~ShaderModule()
    {
        mtlLibrary = nil;
    }
    
} // namespace CE::Metal
