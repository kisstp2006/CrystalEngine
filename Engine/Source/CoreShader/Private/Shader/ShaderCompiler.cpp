
#include "CoreShader.h"

#include <PAL/Common/PlatformSystemIncludes.h>

#if PLATFORM_WINDOWS
#else
#   define __EMULATE_UUID
#endif

#if PLATFORM_DESKTOP
#include <spirv_cross/spirv_msl.hpp>
#include "spirv_cross/spirv_reflect.hpp"
#include "spirv_cross/spirv_parser.hpp"
#include <spirv-tools/libspirv.hpp>
#endif

#undef SIZE_T
#include <dxc/dxcapi.h>

#include <locale>
#include <codecvt>
#include <string>

namespace CE
{

	static std::wstring ToWString(const std::string& stringToConvert)
	{
		std::wstring wideString = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(stringToConvert);
		return wideString;
	}

	static std::wstring ToWString(const String& string)
	{
		std::string stringToConvert = string.ToStdString();
		std::wstring wideString =
			std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(stringToConvert);
		return wideString;
	}
	
	struct ShaderCompiler::Impl
	{
		CComPtr<IDxcUtils> utils;
		CComPtr<IDxcCompiler3> compiler;
		CComPtr<IDxcIncludeHandler> includeHandler;

		~Impl()
		{
			utils.Release();
			compiler.Release();
			includeHandler.Release();
		}
	};

	ShaderCompiler::ShaderCompiler()
	{
		impl = new Impl();

		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&impl->utils));
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&impl->compiler));
		
		impl->utils->CreateDefaultIncludeHandler(&impl->includeHandler);
	}

	ShaderCompiler::~ShaderCompiler()
	{
		delete impl;
	}

    ShaderCompiler::ErrorCode ShaderCompiler::BuildMSL(const void* data, u32 dataSize, const ShaderBuildConfig& buildConfig, BinaryBlob& outByteCode, Array<std::wstring>& extraArgs)
    {
#if !PLATFORM_MAC
        return ERR_UnsupportedPlatform;
#endif
        
        HRESULT status = 0;
        ShaderBuildConfig& config = const_cast<ShaderBuildConfig&>(buildConfig);

        CComPtr<IDxcBlobEncoding> source = nullptr;

        status = impl->utils->CreateBlob(data, dataSize, DXC_CP_UTF8, &source);

        if (!SUCCEEDED(status))
        {
            this->errorMessage = "Failed to load source file";
            return ERR_FailedToLoadFile;
        }

        DxcBuffer buffer;
        buffer.Ptr = source->GetBufferPointer();
        buffer.Size = source->GetBufferSize();
        buffer.Encoding = DXC_CP_UTF8;
        
        BinaryBlob spirvCode;

        ErrorCode result = BuildSpirv(buffer, config, spirvCode, extraArgs);
        if (result != ERR_Success)
        {
            return result;
        }
        
        spirv_cross::CompilerReflection* reflection = new spirv_cross::CompilerReflection((const uint32_t*)spirvCode.GetDataPtr(), spirvCode.GetDataSize() / 4);
        defer(&)
        {
            delete reflection;
        };

        auto resources = reflection->get_shader_resources();
        
        spirv_cross::CompilerMSL compiler((u32*)spirvCode.GetDataPtr(), spirvCode.GetDataSize() / 4);
        
        spirv_cross::CompilerMSL::Options mslOptions;
#if PLATFORM_MAC
        mslOptions.platform = spirv_cross::CompilerMSL::Options::Platform::macOS;
#elif PLATFORM_IOS
        mslOptions.platform = spirv_cross::CompilerMSL::Options::Platform::iOS;
#endif
        mslOptions.msl_version = spirv_cross::CompilerMSL::Options::make_msl_version(2, 3);
        compiler.set_msl_options(mslOptions);
        
        u32 resourceIndex = 0;
        for (auto& ubo : resources.uniform_buffers)
        {
            compiler.set_decoration(ubo.id, spv::DecorationBinding, resourceIndex++);
        }
        for (auto& ssbo : resources.storage_buffers)
        {
            compiler.set_decoration(ssbo.id, spv::DecorationBinding, resourceIndex++);
        }
        for (auto& image : resources.separate_images)
        {
            compiler.set_decoration(image.id, spv::DecorationBinding, resourceIndex++);
        }
        for (auto& image : resources.sampled_images)
        {
            compiler.set_decoration(image.id, spv::DecorationBinding, resourceIndex++);
        }
        for (auto& image : resources.storage_images)
        {
            compiler.set_decoration(image.id, spv::DecorationBinding, resourceIndex++);
        }
        for (auto& sampler : resources.separate_samplers)
        {
            compiler.set_decoration(sampler.id, spv::DecorationBinding, resourceIndex++);
        }
        
        std::string resultStr = compiler.compile();
        if (resultStr.empty())
            return ERR_CompilationFailure;
        
        outByteCode.LoadData(resultStr.c_str(), resultStr.length() + 1); // +1 for null-terminator
        
        return ERR_Success;
    }

	ShaderCompiler::ErrorCode ShaderCompiler::BuildSpirv(const IO::Path& hlslPath, const ShaderBuildConfig& buildConfig, BinaryBlob& outByteCode, Array<std::wstring>& extraArgs)
	{
		if (!hlslPath.Exists())
			return ERR_FileNotFound;
		if (hlslPath.IsDirectory())
			return ERR_InvalidFile;

		HRESULT status;
		ShaderBuildConfig& config = const_cast<ShaderBuildConfig&>(buildConfig);

		CComPtr<IDxcBlobEncoding> source = nullptr;
		std::wstring str = ToWString(hlslPath.GetString());
		std::wstring fileNameStr = ToWString(hlslPath.GetFileName().GetString());

		status = impl->utils->LoadFile(str.data(), nullptr, &source);
		
		if (!SUCCEEDED(status))
		{
			this->errorMessage = "Failed to load source file";
			return ERR_FailedToLoadFile;
		}

		DxcBuffer buffer;
		buffer.Ptr = source->GetBufferPointer();
		buffer.Size = source->GetBufferSize();
		buffer.Encoding = DXC_CP_UTF8;

		return BuildSpirv(buffer, config, outByteCode, extraArgs);
	}

	ShaderCompiler::ErrorCode ShaderCompiler::BuildSpirv(const void* data, u32 dataSize, const ShaderBuildConfig& buildConfig, 
		BinaryBlob& outByteCode, Array<std::wstring>& extraArgs)
	{
		HRESULT status = 0;
		ShaderBuildConfig& config = const_cast<ShaderBuildConfig&>(buildConfig);

		CComPtr<IDxcBlobEncoding> source = nullptr;

		status = impl->utils->CreateBlob(data, dataSize, DXC_CP_UTF8, &source);

		if (!SUCCEEDED(status))
		{
			this->errorMessage = "Failed to load source file";
			return ERR_FailedToLoadFile;
		}

		DxcBuffer buffer;
		buffer.Ptr = source->GetBufferPointer();
		buffer.Size = source->GetBufferSize();
		buffer.Encoding = DXC_CP_UTF8;

		return BuildSpirv(buffer, config, outByteCode, extraArgs);
	}

	ShaderCompiler::ErrorCode ShaderCompiler::BuildSpirv(DxcBuffer buffer, const ShaderBuildConfig& buildConfig, BinaryBlob& outByteCode, Array<std::wstring>& extraArgs)
	{
		HRESULT status = 0;
		ShaderBuildConfig& config = const_cast<ShaderBuildConfig&>(buildConfig);

		Array<const wchar_t*> wcharArgs{};
		for (const auto& arg : extraArgs)
		{
			wcharArgs.Add(arg.data());
		}

		wcharArgs.Add(L"-spirv");

		Array<std::wstring> includeSearchPathsWString = config.includeSearchPaths.Transform<std::wstring>([&](IO::Path& path) -> std::wstring
			{
				return ToWString(path.GetString());
			});

		for (const auto& includePath : includeSearchPathsWString)
		{
			wcharArgs.AddRange({ L"-I", includePath.data() });
		}

		std::wstring entryName = ToWString(config.entry);

		wcharArgs.AddRange({ L"-E", entryName.data() });

		if (buildConfig.stage == RHI::ShaderStage::Vertex)
		{
			wcharArgs.AddRange({ L"-T", L"vs_6_0" });
		}
		else if (buildConfig.stage == RHI::ShaderStage::Fragment)
		{
			wcharArgs.AddRange({ L"-T", L"ps_6_0" });
		}
		else if (buildConfig.stage == RHI::ShaderStage::Compute)
		{
			wcharArgs.AddRange({ L"-T", L"cs_6_0" });
		}
		else
		{
			return ERR_InvalidArgs;
		}

		Array<std::wstring> globalDefinesWString = config.globalDefines.Transform<std::wstring>([&](String& string)
			{
				return ToWString(string);
			});

		for (const auto& define : globalDefinesWString)
		{
			wcharArgs.AddRange({ L"-D", define.data() });
		}

		CComPtr<IDxcResult> results;
		status = impl->compiler->Compile(
			&buffer,                // Source buffer.
			wcharArgs.GetData(),     // Array of pointers to arguments.
			wcharArgs.GetSize(),      // Number of arguments.
			impl->includeHandler,    // User-provided interface to handle #include directives (optional).
			IID_PPV_ARGS(&results)  // Compiler output status, buffer, and errors.
		);
		
		defer(&)
		{
			results.Release();
		};

		results->GetStatus(&status);

		if (!SUCCEEDED(status))
		{
			CComPtr<IDxcBlobEncoding> error;
			results->GetErrorBuffer(&error);
			char* message = (char*)error->GetBufferPointer();
			auto size = error->GetBufferSize();
			this->errorMessage = message;
			return ERR_CompilationFailure;
		}

		CComPtr<IDxcBlob> blob;
		status = results->GetResult(&blob);

		if (!SUCCEEDED(status))
		{
			return ERR_CompilationFailure;
		}

		outByteCode.LoadData(blob->GetBufferPointer(), blob->GetBufferSize());

		return ERR_Success;
	}

} // namespace CE::Editor

