#include "MetalRHIPrivate.h"

namespace CE::Metal
{
    
    ShaderResourceGroup::ShaderResourceGroup(Device* device, const RHI::ShaderResourceGroupLayout& srgLayout) : srgLayout(srgLayout)
    {
        
    }

    ShaderResourceGroup::~ShaderResourceGroup()
    {
        
    }
    
} // namespace CE::Metal
