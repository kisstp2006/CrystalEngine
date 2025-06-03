#include "MetalRHIPrivate.h"

namespace CE::Metal
{

    RenderTarget::RenderTarget(Device* device, const RHI::RenderTargetLayout& rtLayout)
        : device(device), rtLayout(rtLayout)
    {
        
    }

    RenderTarget::~RenderTarget()
    {
        
    }

    RenderTarget* RenderTarget::Clone(const Array<RHI::Format>& newColorFormats, RHI::Format depthStencilFormat, u32 subpassSelection)
    {
        RHI::RenderTargetLayout newRtLayout = rtLayout;
        
        // TODO
        
        return new RenderTarget(device, newRtLayout);
    }

    RenderTarget* RenderTarget::Clone(MultisampleState msaa, const Array<RHI::Format>& newColorFormats, RHI::Format depthStencilFormat, u32 subpassSelection)
    {
        
    }

    void RenderTarget::GetAttachmentFormats(Array<RHI::Format>& outColorFormats, RHI::Format& outDepthStencilFormat, u32 subpassSelection)
    {
        
    }

}
