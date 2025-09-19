#pragma once

namespace CE::Metal
{

    class ShaderResourceGroup : public RHI::ShaderResourceGroup
    {
    public:
        
        ShaderResourceGroup(Device* device, const RHI::ShaderResourceGroupLayout& srgLayout);

        virtual ~ShaderResourceGroup();
        
    private:
        
        Device* device = nullptr;
        RHI::ShaderResourceGroupLayout srgLayout;
    };
    
} // namespace CE::Metal
