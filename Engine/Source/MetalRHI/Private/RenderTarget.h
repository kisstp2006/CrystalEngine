#pragma once

namespace CE::Metal
{

    class RenderTarget : public RHI::RenderTarget
    {
    public:
        RenderTarget(Device* device, const RHI::RenderTargetLayout& rtLayout);
        virtual ~RenderTarget();
        
        RenderTarget* Clone(const Array<RHI::Format>& newColorFormats, RHI::Format depthStencilFormat, u32 subpassSelection) override;
        
        RenderTarget* Clone(MultisampleState msaa, const Array<RHI::Format>& newColorFormats, RHI::Format depthStencilFormat, u32 subpassSelection) override;
        
        void GetAttachmentFormats(Array<RHI::Format>& outColorFormats, RHI::Format& outDepthStencilFormat, u32 subpassSelection) override;
        
        int GetSubpassCount() const { return [renderPassDescArray count]; }
        
        MTLRenderPassDescriptor* GetSubpass(int index) { return (MTLRenderPassDescriptor*)[renderPassDescArray objectAtIndex:index]; }
        
    private:
        
        Device* device = nullptr;
        RHI::RenderTargetLayout rtLayout{};
        
        NSMutableArray* renderPassDescArray = nil;
    };
    
} // namespace CE::Metal
