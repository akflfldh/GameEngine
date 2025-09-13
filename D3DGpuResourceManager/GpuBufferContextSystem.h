#pragma once

#include <CommonHeader/GpuTypes.h>
#include <D3DGpuResourceManager/GRMPtr.h>
#include <D3DGpuResourceManager/GpuResourceDllMacro.h>
#include <stdint.h>
#include <string>
namespace GRM
{

using pDataMemory = void *;
enum EBufferShaderResourceUploadType
{
    eObject = 0,
    ePass = 1
};

struct GpuBufferContext
{
    uint32_t mID;
    GRM::GRMPtr mGpuBuffer;
    // type (constant(object,pass) , structured(object) )

    // 여기서 pass인경우에는 에디터는 object파라미터에 nullptr을 전달할것이고, frameContext에만 유효한 파라미터를
    // 전달할것이다.
    //  buffer타입이 object인경우에는 object파라미터와 frameContext파라미터 둘다 유효하다.
    pDataMemory (*createBufferData)(void *object, void *frameContext) = nullptr;

    GRM::BufferDesc mBufferDesc; // 버퍼설명 //버퍼를 다시 생성할떄 사용한다.

    EBufferShaderResourceUploadType mUploadType; // 업로드타입, 0이면 pass 기본 10, 1이면 object 기본 100
};

class GPURESOURCE_MANAGER_API GpuBufferContextSystem
{
  public:
    static GpuBufferContextSystem *GetInstance();
    virtual ~GpuBufferContextSystem() = 0;

    //.shader.buffer파일을 읽어서 버퍼를 shader들이 사용할 버퍼를 생성한다.
    virtual void LoadShaderBufferFile(const std::string &filePath) = 0;

    // 이메서드는 에디터와, 유저 DLL의 전역함수가 호출한다.
    // 이미 등록되어있는 버퍼에대해  버퍼를 채울 데이터메모리를 반환하는 콜백 등록을 수행한다.
    virtual bool RegisterGpuBufferCallback(uint32_t id,
                                           pDataMemory (*createBufferData)(void *object, void *frameContext)) = 0;

    // 이메서드는 shaderImporter들이호출하여 버퍼를 생성하고 등록한다.
    //	virtual bool CreateGpuBufferContext(uint32_t id, const GRM::BufferDesc& bufferDesc) = 0;
    // 이 메서드는 renderItem Builder들이 호출할것이다.
    virtual GpuBufferContext *GetGpuBufferContext(uint32_t id) const = 0;

  protected:
    GpuBufferContextSystem();

  private:
};
} // namespace GRM
