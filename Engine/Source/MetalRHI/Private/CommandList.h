#pragma once

namespace CE::Metal
{
    class Fence;

    class CommandList : public RHI::CommandList
    {
    public:
        CommandList(Device* device, id<MTLCommandQueue> mtlCommandQueue, RHI::CommandListType type);
        
        virtual ~CommandList();
        
        id<MTLCommandBuffer> GetMtlCommandBuffer() const { return mtlCommandBuffer; }
        
        void Begin() override;
        
        void End() override;
        
        void BeginRenderTarget(RHI::RenderTarget* renderTarget, RHI::RenderTargetBuffer* renderTargetBuffer, RHI::AttachmentClearValue* clearValuesPerAttachment) override;
        
        void EndRenderTarget() override;
        
        void ResourceBarrier(u32 count, ResourceBarrierDescriptor* barriers) override;
        
        void SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup* >& srgs) override;
        
        void ClearShaderResourceGroups() override;
        
        void SetRootConstants(u32 offset, u32 num32BitValues, const void* srcData) override;
        
        void SetViewports(u32 count, ViewportState* viewports) override;
        
        void SetScissors(u32 count, ScissorState* scissors) override;
        
        void CommitShaderResources() override;
        
        void BindPipelineState(RHI::PipelineState* pipelineState) override;
        
        void BindVertexBuffers(u32 firstInputSlot, u32 count, const RHI::VertexBufferView* bufferViews) override;
        
        void BindIndexBuffer(const RHI::IndexBufferView& bufferView) override;
        
        void DrawIndexed(const DrawIndexedArguments& args) override;
        
        void DrawLinear(const DrawLinearArguments& args) override;
        
        void Dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) override;
        
        void CopyTextureRegion(const BufferToTextureCopy& region) override;
        
        void CopyTextureRegion(const TextureToBufferCopy& region) override;
        
        void CopyBufferRegion(const BufferCopy& copy) override;
        
        void RegisterFence(Fence* fence);
        void DeregisterFence(Fence* fence);

    private:

        Device* device = nullptr;
        
        id<MTLCommandQueue> mtlCommandQueue = nil;
        id<MTLCommandBuffer> mtlCommandBuffer = nil;
        
        List<Fence*> fenceInstances;
        SharedMutex fenceLock;
        
    };
    
} // namespace CE::Metal
