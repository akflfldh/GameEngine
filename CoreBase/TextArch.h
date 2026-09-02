#pragma once

#include <BinaryReaderWriter/TextReader.h>
#include <BinaryReaderWriter/TextWriter.h>
#include <CoreBase/Arch.h>
#include <CoreBase/CoreBaseDllMacro.h>
#include <CoreBase/FString.h>

class COREBASE_API TextArch : public Arch
{
    using Super = Arch;

  public:
    TextArch(bool loadingFlag = true);
    virtual ~TextArch();

    virtual void Start() override;
    virtual void End() override;
    void SetFile(const char *file);

    virtual Arch &operator<<(bool &value) override;

    // Characters (필요에 따라 wchar_t 등 추가 가능)
    virtual Arch &operator<<(char &value) override;

    // Signed Integers (플랫폼 독립적인 고정 크기 정수 사용을 권장)
    virtual Arch &operator<<(int8_t &value) override;
    virtual Arch &operator<<(int16_t &value) override;
    virtual Arch &operator<<(int32_t &value) override;
    virtual Arch &operator<<(int64_t &value) override;
    virtual Arch &operator<<(long &value) override;
    virtual Arch &operator<<(unsigned long &value) override;
    // Unsigned Integers
    virtual Arch &operator<<(uint8_t &value) override;
    virtual Arch &operator<<(uint16_t &value) override;
    virtual Arch &operator<<(uint32_t &value) override;
    virtual Arch &operator<<(uint64_t &value) override;

    virtual Arch &operator<<(float &value) override;
    virtual Arch &operator<<(double &value) override;

    virtual Arch &operator<<(FString &fstring) override;
    virtual Arch &operator<<(std::string &str) override;

#pragma region Table
    virtual void StartTable(std::string &tableName) override;
    virtual void EndTable() override;
    virtual void ReadPropertyHeader(std::string &propertyName, std::string &propertyType,
                                    uint32_t &propertyValueSize) override;
    virtual void WritePropertyHeader(const std::string &propertyName, const std::string &propertyType) override;

    // 반드시 헤더를 먼저읽고 호출
    virtual void SkipProperty(uint32_t propertyValueSize) override;
    // 앞 4바이트는 value크기를 담는 공간으로 예약
    virtual bool StartProperty(const std::string &propertyName, const std::string &propertyType) override;
    // EndProperty호출시 offset을 계산하여 value의 크기를 기록한다.( 파생된 구현부에서 지켜야하는 약속 )
    virtual void EndProperty() override;
    virtual uint32_t GetPropertySize() const override;

    // 다음 시작테이블의 이름을 엿본다.
    virtual void peekTableName(std::string &oTableName) override;

#pragma endregion

  protected:
    // 크기는 기록하지않기에 따로 기록해야할것
    virtual void Serialize(const void *data, size_t size) override;
    virtual void DeSerialize(void *data, size_t size) override;
    virtual bool IsAvaliableSerialize() const override;

  private:
    QuadRW::TextReader mReader;
    QuadRW::TextWriter mWriter;

    FString mFilePath;
};
