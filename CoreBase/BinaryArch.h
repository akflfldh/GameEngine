#pragma once
#include <BinaryReaderWriter/BinaryReader.h>
#include <BinaryReaderWriter/BinaryWriter.h>
#include <CoreBase/Arch.h>
#include <CoreBase/CoreBaseDllMacro.h>
#include <CoreBase/FString.h>

class COREBASE_API BinaryArch : public Arch
{
    using Super = Arch;

  public:
    BinaryArch(bool isLoadingFlag);
    virtual ~BinaryArch();

    void Start() override;
    void End() override;
    void SetFile(const char *file);

    virtual Arch &operator<<(bool &value) override;

    // Characters (필요에 따라 wchar_t 등 추가 가능)
    virtual Arch &operator<<(char &value) override;

    // Signed Integers (플랫폼 독립적인 고정 크기 정수 사용을 권장)
    virtual Arch &operator<<(int8_t &value) override;
    virtual Arch &operator<<(int16_t &value) override;
    virtual Arch &operator<<(int32_t &value) override;
    virtual Arch &operator<<(int64_t &value) override;

    // Unsigned Integers
    virtual Arch &operator<<(uint8_t &value) override;
    virtual Arch &operator<<(uint16_t &value) override;
    virtual Arch &operator<<(uint32_t &value) override;
    virtual Arch &operator<<(uint64_t &value) override;

    // size_t (아키텍처에 따라 크기가 변하므로 파일 포맷에 직접 사용할 경우 주의) -> 8바이트로고정되었다고 생각하자 일단

    // Floating Point
    virtual Arch &operator<<(float &value) override;
    virtual Arch &operator<<(double &value) override;

    virtual Arch &operator<<(FString &fstring) override;

  protected:
    // 크기는 기록하지않기에 따로 기록해야할것
    virtual void Serialize(const void *data, size_t size) override;
    virtual void DeSerialize(void *data, size_t size) override;

  private:
    QuadRW::BinaryReader mReader;
    QuadRW::BinaryWriter mWriter;

    FString mFilePath;
};
