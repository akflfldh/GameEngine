#include "ShaderCompiler.h"
#include <Logger/Logger.h>
#include <RenderSystem/MaterialType.h>
ShaderCompiler *ShaderCompiler::GetInstance()
{
    static ShaderCompiler instance;
    return &instance;
}

ShaderCompiler::ShaderCompiler() {}

ShaderCompiler::~ShaderCompiler() {}

bool ShaderCompiler::Compile(const Render::ShaderSourceInfo &shaderInfo, std::vector<uint8_t> &oBytecode)
{

#ifdef D3DX
    return CompileD3D(shaderInfo, oBytecode);

#endif

    return false;
}

#ifdef D3DX
#include <d3dcompiler.h>
#include <wrl.h>

bool ShaderCompiler::CompileD3D(const Render::ShaderSourceInfo &shaderInfo, std::vector<uint8_t> &oBytecode)
{
    oBytecode.clear();

    Microsoft::WRL::ComPtr<ID3DBlob> blob = nullptr;
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    ID3DBlob *errorBlob = nullptr;

    uint8_t *pShader = shaderInfo.mShadeCode;
    size_t shaderSize = shaderInfo.mShaderCodeSize;

    // D3DCompile은 배열 끝의 null sentinel을 요구한다. 문자열은 shaderInfo가 컴파일 동안 소유한다.
    std::vector<D3D_SHADER_MACRO> shaderMacros;
    const D3D_SHADER_MACRO *shaderMacroData = nullptr;
    if (shaderInfo.mShaderMacros.empty() == false)
    {
        shaderMacros.reserve(shaderInfo.mShaderMacros.size() + 1);
        for (const Render::ShaderMacroDefinition &macro : shaderInfo.mShaderMacros)
            shaderMacros.push_back({macro.mMacro.c_str(), macro.mValue.c_str()});
        shaderMacros.push_back({nullptr, nullptr});
        shaderMacroData = shaderMacros.data();
    }

    HRESULT ret =
        D3DCompile(pShader, shaderSize, nullptr, shaderMacroData, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                   shaderInfo.mEntryPoint.c_str(), shaderInfo.mTarget.c_str(), compileFlags, 0, &blob, &errorBlob);

    if (FAILED(ret))
    {
        if (errorBlob != nullptr)
        {
            const char *error = (const char *)errorBlob->GetBufferPointer();
            LOG_MESSAGE_ERROR("ShaderCompile", error);
            OutputDebugStringA(error);
            errorBlob->Release();
        }

        return false;
    }

    if (errorBlob != nullptr)
        errorBlob->Release();

    oBytecode.resize(blob->GetBufferSize());
    memcpy(oBytecode.data(), blob->GetBufferPointer(), blob->GetBufferSize());

    return true;
}

#endif