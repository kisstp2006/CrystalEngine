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
        
        const auto& subpass = newRtLayout.subpasses[subpassSelection];
        
        for (int i = 0; i < subpass.colorAttachments.GetSize(); i++)
        {
            if (i >= newColorFormats.GetSize())
                break;
            if (newColorFormats[i] == RHI::Format::Undefined)
                continue;
            
            u32 attachmentIndex = subpass.colorAttachments[i];
            
            RHI::RenderAttachmentLayout& attachment = newRtLayout.attachmentLayouts[attachmentIndex];
            attachment.format = newColorFormats[i];
        }
        
        if (subpass.depthStencilAttachment.GetSize() > 0 && depthStencilFormat != RHI::Format::Undefined)
        {
            u32 attachmentIndex = subpass.depthStencilAttachment[0];
            newRtLayout.attachmentLayouts[attachmentIndex].format = depthStencilFormat;
        }
        
        return new RenderTarget(device, newRtLayout);
    }

    RenderTarget* RenderTarget::Clone(MultisampleState msaa, const Array<RHI::Format>& newColorFormats, RHI::Format depthStencilFormat, u32 subpassSelection)
    {
        RHI::RenderTargetLayout newRtLayout = rtLayout;
        
        const auto& subpass = newRtLayout.subpasses[subpassSelection];
        
        for (int i = 0; i < subpass.colorAttachments.GetSize(); i++)
        {
            if (i >= newColorFormats.GetSize())
                break;
            if (newColorFormats[i] == RHI::Format::Undefined)
                continue;
            
            u32 attachmentIndex = subpass.colorAttachments[i];
            RHI::RenderAttachmentLayout& attachment = newRtLayout.attachmentLayouts[attachmentIndex];
            
            attachment.format = newColorFormats[i];
            attachment.multisampleState = msaa;
        }
        
        if (subpass.depthStencilAttachment.GetSize() > 0 && depthStencilFormat != RHI::Format::Undefined)
        {
            u32 attachmentIndex = subpass.depthStencilAttachment[0];
            newRtLayout.attachmentLayouts[attachmentIndex].format = depthStencilFormat;
            newRtLayout.attachmentLayouts[attachmentIndex].multisampleState = msaa;
        }
        
        return new RenderTarget(device, newRtLayout);
    }

    void RenderTarget::GetAttachmentFormats(Array<RHI::Format>& outColorFormats, RHI::Format& outDepthStencilFormat, u32 subpassSelection)
    {
        outColorFormats.Clear();
        outDepthStencilFormat = RHI::Format::Undefined;
        
        const auto& subpass = rtLayout.subpasses[subpassSelection];
        
        for (int i = 0; i < subpass.colorAttachments.GetSize(); i++)
        {
            u32 attachmentIndex = subpass.colorAttachments[i];
            outColorFormats.Add(rtLayout.attachmentLayouts[attachmentIndex].format);
        }
        
        if (subpass.depthStencilAttachment.GetSize() > 0)
        {
            u32 attachmentIndex = subpass.depthStencilAttachment[0];
            outDepthStencilFormat = rtLayout.attachmentLayouts[attachmentIndex].format;
        }
    }

}
