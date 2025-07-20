#include "FusionCore.h"

#undef DLL_EXPORT
#define FT_PUBLIC_FUNCTION_ATTRIBUTE

#include <ft2build.h>
#include "freetype/freetype.h"

#include "FontConstants.inl"

namespace CE
{

    FSDFFontAtlas::FSDFFontAtlas()
    {

    }

    void FSDFFontAtlas::Init(const FSDFFontAtlasInitInfo& initInfo)
    {
        ZoneScoped;

        if (atlasImageMips.NotEmpty())
            return;

		sdfGlyphShader = initInfo.sdfGlyphShader;
        
    }

    void FSDFFontAtlas::Flush(u32 imageIndex)
    {
        ZoneScoped;

        if (!flushRequiredPerImage[imageIndex])
            return;


    }

    void FSDFFontAtlas::AddGlyphs(const Array<u32>& charSet, bool isBold, bool isItalic)
    {
        ZoneScoped;

        if (charSet.IsEmpty())
            return;

        FT_Face face = regular;

        if (isBold && isItalic)
        {
            if (boldItalic != nullptr)
                face = boldItalic;
            else if (bold != nullptr)
                face = bold;
        }
        else if (isBold)
        {
            if (bold != nullptr)
                face = bold;
        }
        else if (isItalic)
        {
            if (italic != nullptr)
                face = italic;
        }

        static HashSet<FT_ULong> nonDisplayCharacters = { ' ', '\n', '\r', '\t' };

        u32 dpi = PlatformApplication::Get()->GetSystemDpi();
        f32 scaling = PlatformApplication::Get()->GetSystemDpiScaling();

        FT_Set_Char_Size(face, 0, SDFFontSize << 6, dpi, dpi);

        for (int i = 0; i < charSet.GetSize(); ++i)
        {
            FT_ULong charCode = charSet[i];
            char c = charCode;

            
        }
    }

} // namespace CE

