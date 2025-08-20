#pragma once

namespace Render
{
class IMaterialManager;
}
#include <BinaryReaderWriter/BinaryReader.h>
#include <D3DGpuResourceManager/GpuTypes.h>
#include <RenderSystem/RenderType.h>
#include <memory>
#include <string>

namespace JsonParser
{
class Parser;
}

namespace Quad
{
//.shader를 읽어 RenderSystem 이 받아 드리는 형태로 가공하는 역할을가진다.
class EditorShaderImporter
{
  public:
    static EditorShaderImporter *GetInstance();
    EditorShaderImporter(Render::IMaterialManager *materialManager);
    ~EditorShaderImporter();

    bool Import(const std::string &shaderFile);

  private:
    uint32_t CacluateConstantBufferSize(uint32_t size) const;

    Render::EShaderResourceType ConvertToRenderShaderResourceType(GRM::EBufferUsage bufferUsage) const;

  private:
    static EditorShaderImporter *mInstance;
    Render::IMaterialManager *mGpuMaterialManager;
    std::unique_ptr<JsonParser::Parser> mJsonParser;

    QuadRW::BinaryReader mBinaryReader;
};

} // namespace Quad
