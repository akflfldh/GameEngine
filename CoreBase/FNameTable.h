#pragma once
#include <CoreBase/CoreBaseDllMacro.h>
#include <CoreBase/FString.h>
// 텍스트로 저장 이것도 binaryReaderWriter에서 제공하게하자.
// 인터페이스는?
// 파일 IO는 외부에서 처리하도록한다.
// FNameTable은 그냥 내부적으로 유지하는 테이블 데이터를 외부에 전달하는 메서드만 제공한다.

/*

    OpenFile("asd.asd");
    Store("asdas");
    Store("asdsadasD")이렇게 저장하면 끝.


    내부적으로 stringStream쓰면? 편하지
    즉 기본타입에대해서도 제공하게 템플릿제공하면좋아 근데 숨겨야하니 stringstream은내부적으로


*/
class FNameTableImpl;
class Arch;
class COREBASE_API FNameTable
{
  public:
    static FNameTable *GetInstance();
    ~FNameTable();

    size_t GetIndex(const char *name) const;
    FString GetName(size_t index) const;

    bool IsValid(size_t index) const;

    void Serialize(Arch &arch);

  protected:
    FNameTable();

  private:
    static FNameTableImpl *mImpl;
};
