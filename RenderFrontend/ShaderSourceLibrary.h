#pragma once

#include <RenderFrontend/RenderFrontendDllMarco.h>
#include <RenderSystem/MaterialType.h>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Render
{

using ShaderBytecodeTable = std::unordered_map<ShaderVariantKey, std::vector<uint8_t>>;

// 외부에서 컴파일하거나 로드한 셰이더 바이트코드를 ShaderVariantKey별로 소유·조회한다.
// 기존 클래스 이름은 유지하되 HLSL 원본 로딩, 컴파일, 파일 저장 및 PSO 생성은 담당하지 않는다.
// 상위 시스템이 초기화 시 테이블을 공급하고, RenderMaterialResolver가 필요한 변형을 조회한다.
class RENDER_FRONTEND_API ShaderSourceLibrary
{
  public:
    static ShaderSourceLibrary *GetInstance();
    ShaderSourceLibrary();
    ~ShaderSourceLibrary();

    // 테이블 전체를 교체한다. 호출자는 std::move로 소유권을 넘기거나 복사본을 전달할 수 있다.
    // 경로·매크로 순서의 정규화와 컴파일 성공 여부 확인은 공급자가 수행한다.
    void SetShaderBytecodeTable(ShaderBytecodeTable table);

    // 미등록 키는 nullptr. 반환 버퍼는 라이브러리가 소유하며 테이블 교체·소멸 전까지만 유효하다.
    // PSO 생성 등 조회 결과를 사용하는 동안에는 테이블 교체를 병행하지 않는다.
    const std::vector<uint8_t> *GetShaderBytecode(const ShaderVariantKey &key) const;

    std::vector<ShaderVariantKey> GetShaderKeyList() const;

  private:
    ShaderBytecodeTable mShaderBytecodeTable;
};

} // namespace Render
