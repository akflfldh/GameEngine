

#pragma once

#include <cstdint>
#include <vector>

namespace Render
{
struct ShaderSourceInfo;
}

// 에디터의 HLSL 컴파일을 담당한다. 입력 소스와 출력 바이트코드는 호출자가 소유하며,
// GPU 머터리얼 등록이나 PSO 생성·소유는 렌더 시스템에 맡긴다.
class ShaderCompiler
{
  public:
    static ShaderCompiler *GetInstance();
    ShaderCompiler();
    ~ShaderCompiler();

    // 기존 ShaderSourceInfo의 소스·엔트리 포인트·타깃·매크로를 재사용한다.
    // 출력은 파일 저장과 런타임 전달에 사용할 원시 바이트코드이며, 반환값은 컴파일 성공 여부다.
    bool Compile(const Render::ShaderSourceInfo &shaderInfo, std::vector<uint8_t> &oBytecode);

  private:
    bool CompileD3D(const Render::ShaderSourceInfo &shaderInfo, std::vector<uint8_t> &oBytecode);
};
