#pragma once
#include <BinaryReaderWriter/BinaryReader.h>
#include <BinaryReaderWriter/BinaryWriter.h>
#include <CoreBase/Arch.h>
#include <CoreBase/CoreBaseDllMacro.h>
#include <CoreBase/FString.h>
#include <filesystem>
#include <stack>
#include <unordered_map>
#include <vector>

class COREBASE_API BinaryArch : public Arch
{
    using Super = Arch;
    using ArchPropertyMap = std::unordered_map<std::string, ArchProperty>;

  public:
    BinaryArch(bool isLoadingFlag);
    virtual ~BinaryArch();

    void Start() override;

    // load flag 에서 유효 , 복사수행
    void Start(uint8_t *buffer, size_t size);

    void End() override;
    void SetFile(const std::filesystem::path &filepath);

    // load

    // write
    uint8_t *GetBufferFromMemory();
    size_t GetBufferSize() const;

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

    // size_t (아키텍처에 따라 크기가 변하므로 파일 포맷에 직접 사용할 경우 주의) -> 8바이트로고정되었다고 생각하자 일단

    // Floating Point
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

    template <typename T> BinaryArch &operator<<(const std::vector<T> &vec);

  protected:
    // 크기는 기록하지않기에 따로 기록해야할것
    virtual void Serialize(const void *data, size_t size) override;
    virtual void DeSerialize(void *data, size_t size) override;

  private:
    QuadRW::BinaryReader mReader;
    QuadRW::BinaryWriter mWriter;

    std::filesystem::path mFilePath;

    std::stack<uint64_t> mRWPointerPosForTableStack;
    std::stack<ArchPropertyMap> mArchPropertyMapStack; // table구조 load시 재귀적인 테이블 구축,복구에 사용

    uint32_t mCurrPropertyValueSize;
};

template <typename T> BinaryArch &BinaryArch ::operator<<(const std::vector<T> &vec)
{

    // 1 .개수
    size_t size = vec.size();

    // 2 .element
    for (auto &element : vec)
    {
        *this << element;
    }

    return *this;
}
