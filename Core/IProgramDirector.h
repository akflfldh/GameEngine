#pragma once

#include "Core/CoreDllExport.h"

struct RuntimeServices;

namespace Quad
{
class CORE_API_LIB IProgramDirector
{

  public:
    virtual void Initialize() = 0;
    virtual void Initialize(const RuntimeServices &services)
    {
        Initialize();
    }

    virtual void Begin() = 0;
    virtual void PreUpdate(float deltaTime) = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void EndUpdate(float deltaTime) = 0;
    virtual void CleanUp() = 0;
    virtual void Draw() = 0;

    // draw후 프레임끝에 수행할 작업
    virtual void EndFrame() = 0;

    // 종료버튼 클릭후 수행작업
    virtual void EndSystem() = 0;

  private:
};

} // namespace Quad
