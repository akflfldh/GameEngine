#pragma once

#include <string>
#include <vector>
namespace QuadPF
{
class PhysicalFileSystem;
}

namespace QuadRW
{
class BinaryReader
{
  public:
    BinaryReader();
    ~BinaryReader();

    // 내부버퍼를 비우고 ,새로운파일을 읽는다.
    bool StartRead(const std::string &filePath);

    template <typename T> bool Read(T &oData);
    bool Read(std::string &oStr);

    // size값도 기록한다.

    // data는 내부버퍼에대한 포인터를 가리킨다. Size만큼 복사해서 사용할것
    // 사이즈를 기록하여 저장한 경우에대응되는 읽기 함수
    // bool Read(void** oData, size_t & oSize);

    // size값을 지정하여 읽는다.
    // oBuffer는 size만큼의 크기가 보장되어야한다.
    bool ReadRaw(void *oBuffer, size_t size);

    //기록된 size값 만큼읽는다. 크기가 부족하면 실패
    bool Read(void *oBuffer, size_t bufferSize);

    bool SetReadPointer(uint64_t pointerPos);
    uint64_t GetReadPointer() const;

    // 내부버퍼를 비운다.
    void FlushBuffer();

    // 읽은 파일의 사이즈(byte)를 얻는다
    uint64_t GetFileSize() const;

  private:
    QuadPF::PhysicalFileSystem *mPhysicalFileSystem;
    std::vector<uint8_t> mBuffer;
    uint64_t mReadPointer;
};

} // namespace QuadRW
