
#include "FusionCore.h"

#include "FusionCore.private.h"
#include "Resource.h"

namespace CE
{
	RawData GetFusionShaderVert()
    {
        return RawData{ (u8*)Shaders_FusionShader_vert_spv_Data, Shaders_FusionShader_vert_spv_Length };
    }

    RawData GetFusionShaderFrag()
    {
        return RawData{ (u8*)Shaders_FusionShader_frag_spv_Data, Shaders_FusionShader_frag_spv_Length };
    }

    RawData GetFusionShaderVertJson()
    {
        return RawData{ (u8*)Shaders_FusionShader_vert_json_Data, Shaders_FusionShader_vert_json_Length };
    }

    RawData GetFusionShaderFragJson()
    {
        return RawData{ (u8*)Shaders_FusionShader_frag_json_Data, Shaders_FusionShader_frag_json_Length };
    }

    RawData GetFusionShader2Vert()
    {
        return RawData{ (u8*)Shaders_FusionShader2_vert_spv_Data, Shaders_FusionShader2_vert_spv_Length };
    }

    RawData GetFusionShader2Frag()
    {
        return RawData{ (u8*)Shaders_FusionShader2_frag_spv_Data, Shaders_FusionShader2_frag_spv_Length };
    }

    RawData GetFusionShader2VertJson()
    {
        return RawData{ (u8*)Shaders_FusionShader2_vert_json_Data, Shaders_FusionShader2_vert_json_Length };
    }

    RawData GetFusionShader2FragJson()
    {
        return RawData{ (u8*)Shaders_FusionShader2_frag_json_Data, Shaders_FusionShader2_frag_json_Length };
    }

    RawData GetFusionSDFGlyphGenVert()
	{
		return RawData{ (u8*)Shaders_FusionSDFGlyphGen_vert_spv_Data, Shaders_FusionSDFGlyphGen_vert_spv_Length };
	}

	RawData GetFusionSDFGlyphGenFrag()
	{
		return RawData{ (u8*)Shaders_FusionSDFGlyphGen_frag_spv_Data, Shaders_FusionSDFGlyphGen_frag_spv_Length };
	}

    RawData GetFusionSDFGlyphGenVertJson()
    {
        return RawData{ (u8*)Shaders_FusionSDFGlyphGen_vert_json_Data, Shaders_FusionSDFGlyphGen_vert_json_Length };
	}

    RawData GetFusionSDFGlyphGenFragJson()
    {
        return RawData{ (u8*)Shaders_FusionSDFGlyphGen_frag_json_Data, Shaders_FusionSDFGlyphGen_frag_json_Length };
    }

    class FusionCoreModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {

        }

        virtual void ShutdownModule() override
        {

        }

        virtual void RegisterTypes() override
        {

        }
    };
}

CE_IMPLEMENT_MODULE(FusionCore, CE::FusionCoreModule)
