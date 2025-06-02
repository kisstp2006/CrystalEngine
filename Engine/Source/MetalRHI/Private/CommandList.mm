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
    }

    void CommandList::End()
    {
        // Do nothing
    }

    void CommandList::BeginRenderTarget(RenderTarget* renderTarget, RenderTargetBuffer* renderTargetBuffer, AttachmentClearValue* clearValuesPerAttachment)
    {
        
    }

    void CommandList::EndRenderTarget()
    {
        
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
        
        // TODO
        
        [blitEncoder endEncoding];
    }

    void CommandList::CopyTextureRegion(const TextureToBufferCopy& region)
    {
        
    }

    void CommandList::CopyBufferRegion(const BufferCopy& copy)
    {
        
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
