#include "MetalRHIPrivate.h"

#include "TextureFormats.inl"

namespace CE::Metal
{

    inline MTLLoadAction ToMtlLoadAction(RHI::AttachmentLoadAction loadAction)
    {
        switch (loadAction)
        {
            case RHI::AttachmentLoadAction::Load:
                return MTLLoadActionLoad;
            case RHI::AttachmentLoadAction::Clear:
                return MTLLoadActionClear;
            case RHI::AttachmentLoadAction::DontCare:
                return MTLLoadActionDontCare;
        }
        return MTLLoadActionDontCare;
    }

    inline MTLStoreAction ToMtlStoreAction(RHI::AttachmentStoreAction storeAction)
    {
        switch (storeAction)
        {
            case RHI::AttachmentStoreAction::Store:
                return MTLStoreActionStore;
            case RHI::AttachmentStoreAction::DontCare:
                return MTLStoreActionDontCare;
        }
        return MTLStoreActionDontCare;
    }

    RenderTarget::RenderTarget(Device* device, const RHI::RenderTargetLayout& rtLayout)
        : device(device), rtLayout(rtLayout)
    {
        for (int i = 0; i < rtLayout.subpasses.GetSize(); i++)
        {
            const auto& subpass = rtLayout.subpasses[i];
            
            MTLRenderPassDescriptor* rpDesc = [[MTLRenderPassDescriptor alloc] init];
            
            for (int j = 0; j < subpass.colorAttachments.GetSize(); j++)
            {
                u32 attachmentIdx = subpass.colorAttachments[j];
                const auto& colorAttachment = rtLayout.attachmentLayouts[attachmentIdx];
                
                rpDesc.colorAttachments[j].loadAction = ToMtlLoadAction(colorAttachment.loadAction);
                rpDesc.colorAttachments[j].storeAction = ToMtlStoreAction(colorAttachment.storeAction);
            }
            
            if (!subpass.depthStencilAttachment.IsEmpty())
            {
                u32 attachmentIdx = subpass.depthStencilAttachment[0];
                const auto& depthStencilAttachment = rtLayout.attachmentLayouts[attachmentIdx];
                
                rpDesc.depthAttachment.loadAction = ToMtlLoadAction(depthStencilAttachment.loadAction);
                rpDesc.depthAttachment.storeAction = ToMtlStoreAction(depthStencilAttachment.storeAction);
                
                rpDesc.stencilAttachment.loadAction = ToMtlLoadAction(depthStencilAttachment.loadActionStencil);
                rpDesc.stencilAttachment.storeAction = ToMtlStoreAction(depthStencilAttachment.storeActionStencil);
            }
            
            [renderPassDescArray addObject:rpDesc];
        }
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
