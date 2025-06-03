#pragma once

namespace CE::Metal
{

    class TextureView : public RHI::TextureView
    {
    public:
        
        TextureView(Device* device, const RHI::TextureViewDescriptor& desc);
        
        virtual ~TextureView();

    private:
    
        Device* device = nullptr;
        
        Metal::Texture* texture = nullptr;
        
        MTLPixelFormat mtlFormat;
        MTLTextureType mtlTextureType;
        
        // - ObjC -
        
        id<MTLTexture> mtlTextureView = nil;
    };
    
} // namespace CE::Metal
