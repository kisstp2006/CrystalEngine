#pragma once

namespace CE
{
    struct FSDFFontAtlasInitInfo
    {
		RPI::Shader* sdfGlyphShader = nullptr;
	    Name fontName;
        Array<u32> characterSet;
	};

    CLASS()
    class FUSIONCORE_API FSDFFontAtlas : public Object
    {
        CE_CLASS(FSDFFontAtlas, Object)
    protected:

        FSDFFontAtlas();
        
    public:

		static constexpr u32 SDFFontSize = 64; // Size of the SDF font atlas in pixels

        void Init(const FSDFFontAtlasInitInfo& initInfo);

        //! @brief Flushes all the changes to GPU
        void Flush(u32 imageIndex);

	private:

        void AddGlyphs(const Array<u32>& characterSet, bool isBold = false, bool isItalic = false);

        using CharCode = u32;
        using FontSize = u32;

        struct RowSegment {
            int x, y;
            int height;
        };

        struct FAtlasImage : IntrusiveBase
        {
            virtual ~FAtlasImage()
            {
                delete ptr; ptr = nullptr;
            }

            u8* ptr = nullptr;
            u32 atlasSize = 0;
            Array<RowSegment> rows;

            HashMap<CharCode, FFontGlyphInfo> glyphsByCharCode;
        };


        Array<Ptr<FAtlasImage>> atlasImageMips;

        RPI::Shader* sdfGlyphShader = nullptr;

        StaticArray<bool, RHI::Limits::MaxSwapChainImageCount> flushRequiredPerImage;

        FT_Library ft = nullptr;
        FT_Face regular = nullptr; u8* regularData = nullptr;
        FT_Face italic = nullptr; u8* italicData = nullptr;
        FT_Face bold = nullptr; u8* boldData = nullptr;
        FT_Face boldItalic = nullptr; u8* boldItalicData = nullptr;

        friend class FFontManager;
    };
    
} // namespace CE

#include "FSDFFontAtlas.rtti.h"
