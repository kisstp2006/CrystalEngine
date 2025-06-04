#include "MetalRHIPrivate.h"

namespace CE::Metal
{

    CommandList::CommandList(Device* device, id<MTLCommandQueue> mtlCommandQueue, RHI::CommandListType type)
        : device(device), mtlCommandQueue(mtlCommandQueue)
    {
        this->commandListType = type;
    }

    CommandList::~CommandList()
    {
        LockGuard guard{ fenceLock };
        
        for (int i = 0; i < fenceInstances.GetSize(); i++)
        {
            if (fenceInstances[i])
            {
                fenceInstances[i]->cmdList = nullptr;
                fenceInstances[i]->signalled = true;
            }
        }
    }

    void CommandList::Begin()
    {
        mtlCommandBuffer = [mtlCommandQueue commandBuffer];
        
        curRenderTarget = nullptr;
    }

    void CommandList::End()
    {
        // Do nothing
    }

    void CommandList::BeginRenderTarget(RHI::RenderTarget* renderTarget, RHI::RenderTargetBuffer* renderTargetBuffer, RHI::AttachmentClearValue* clearValuesPerAttachment)
    {
        curRenderTarget = (Metal::RenderTarget*)renderTarget;
        
        // TODO: Implement multiple subpass support
        
        MTLRenderPassDescriptor* rpDesc = curRenderTarget->GetSubpass(0);
        
        mtlRenderEncoder = [mtlCommandBuffer renderCommandEncoderWithDescriptor:rpDesc];
    }

    void CommandList::EndRenderTarget()
    {
        [mtlRenderEncoder endEncoding];
        mtlRenderEncoder = nil;
    }

    void CommandList::ResourceBarrier(u32 count, ResourceBarrierDescriptor* barriers)
    {
        
    }

    void CommandList::SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup*>& srgs)
    {
        
    }

    void CommandList::ClearShaderResourceGroups()
    {
        
    }

    void CommandList::SetRootConstants(u32 offset, u32 num32BitValues, const void* srcData)
    {
        
    }

    void CommandList::SetViewports(u32 count, ViewportState* viewports)
    {
        
    }

    void CommandList::SetScissors(u32 count, ScissorState* scissors)
    {
        
    }

    void CommandList::CommitShaderResources()
    {
        
    }

    void CommandList::BindPipelineState(RHI::PipelineState* pipelineState)
    {
        
    }

    void CommandList::BindVertexBuffers(u32 firstInputSlot, u32 count, const RHI::VertexBufferView* bufferViews)
    {
        
    }

    void CommandList::BindIndexBuffer(const RHI::IndexBufferView& bufferView)
    {
        
    }

    void CommandList::DrawIndexed(const DrawIndexedArguments& args)
    {
        
    }

    void CommandList::DrawLinear(const DrawLinearArguments& args)
    {
        
    }

    void CommandList::Dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ)
    {
        
    }

    void CommandList::CopyTextureRegion(const BufferToTextureCopy& region)
    {
        id<MTLBlitCommandEncoder> blitEncoder = [mtlCommandBuffer blitCommandEncoder];
        
        auto srcBuffer = (Metal::Buffer*)region.srcBuffer;
        u32 width = region.dstTexture->GetWidth();
        auto texture = (Metal::Texture*)region.dstTexture;
        
        u64 bytesPerRow = texture->GetWidth(region.mipSlice) * texture->GetBitsPerPixel() / 8;
        u64 bytesPerImage = bytesPerRow * texture->GetHeight(region.mipSlice);
        
        for (int layer = region.baseArrayLayer; layer < region.baseArrayLayer + region.layerCount; layer++)
        {
            [blitEncoder copyFromBuffer:srcBuffer->GetMtlBuffer()
                           sourceOffset:region.bufferOffset
                      sourceBytesPerRow:bytesPerRow
                    sourceBytesPerImage:bytesPerImage
                             sourceSize:MTLSizeMake(texture->GetWidth(region.mipSlice), texture->GetHeight(region.mipSlice), texture->GetDepth(region.mipSlice))
                              toTexture:texture->GetMtlTexture()
                       destinationSlice:region.mipSlice
                       destinationLevel:layer
                      destinationOrigin:MTLOriginMake(0, 0, 0)];
        }
        
        [blitEncoder endEncoding];
    }

    void CommandList::CopyTextureRegion(const TextureToBufferCopy& region)
    {
        id<MTLBlitCommandEncoder> blitEncoder = [mtlCommandBuffer blitCommandEncoder];
        
        auto srcTexture = (Metal::Texture*)region.srcTexture;
        auto dstBuffer = (Metal::Buffer*)region.dstBuffer;
        
        u64 bytesPerRow = srcTexture->GetWidth(region.mipSlice) * srcTexture->GetBitsPerPixel() / 8;
        u64 bytesPerImage = bytesPerRow * srcTexture->GetHeight(region.mipSlice);
        
        for (int layer = region.baseArrayLayer; layer < region.baseArrayLayer + region.layerCount; layer++)
        {
            [blitEncoder copyFromTexture:srcTexture->GetMtlTexture()
                             sourceSlice:region.mipSlice
                             sourceLevel:layer
                            sourceOrigin:MTLOriginMake(0, 0, 0)
                              sourceSize:MTLSizeMake(srcTexture->GetWidth(region.mipSlice), srcTexture->GetHeight(region.mipSlice), srcTexture->GetDepth(region.mipSlice))
                                toBuffer:dstBuffer->GetMtlBuffer()
                       destinationOffset:region.bufferOffset
                  destinationBytesPerRow:bytesPerRow
                destinationBytesPerImage:bytesPerImage];
        }
        
        [blitEncoder endEncoding];
    }

    void CommandList::CopyBufferRegion(const BufferCopy& copy)
    {
        id<MTLBlitCommandEncoder> blitEncoder = [mtlCommandBuffer blitCommandEncoder];
        
        auto srcBuffer = (Metal::Buffer*)copy.srcBuffer;
        auto dstBuffer = (Metal::Buffer*)copy.dstBuffer;
        
        [blitEncoder copyFromBuffer:srcBuffer->GetMtlBuffer()
                       sourceOffset:copy.srcOffset
                           toBuffer:dstBuffer->GetMtlBuffer()
                  destinationOffset:copy.dstOffset
                               size:copy.totalByteSize];
        
        [blitEncoder endEncoding];
    }

    void CommandList::RegisterFence(Fence* fence)
    {
        LockGuard guard{ fenceLock };
        
        fenceInstances.Add(fence);
    }

    void CommandList::DeregisterFence(Fence* fence)
    {
        LockGuard guard{ fenceLock };
        
        fenceInstances.Remove(fence);
    }



} // namespace CE::Metal
