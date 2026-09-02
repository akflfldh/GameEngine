#pragma once
#include <CoreBase/CoreBaseDllMacro.h>

class Arch;

// object, component, ui 등의 + Asset  최상위 클래스
class COREBASE_API BaseClass
{

  public:
    BaseClass();
    virtual ~BaseClass() = 0;

    virtual const char *GetRunTimeClassName() const;

    void MarkPropertyDirty();
    bool GetPropertyDirty() const;
    void ClearPropertyDirty();

    // 즉시동기화를위한 메서드인터페이스
    virtual void FlushPropertyDirty();

    virtual void Serialize(Arch &arch) = 0;

  private:
    bool mPropertyDirty = true;
};
