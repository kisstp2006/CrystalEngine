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
        
        
    private:
        
        Device* device = nullptr;
        RHI::RenderTargetLayout rtLayout{};
    };
    
} // namespace CE::Metal
