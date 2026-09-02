#pragma once
#include <sstream>
#include <string>
#include <vector>
namespace QuadPF
{
class PhysicalFileSystem;
}

namespace QuadRW
{
// 지금 BinaryReader와 거의 동일하다 -> 텍스트만을위한 기능을 추가할필요가있을수있다.

class TextReader
{
  public:
    TextReader();
    ~TextReader();

    // 일단 맨처음은 파일을 모두읽는게 끝

    bool StartRead(const char *filePath);
    bool StartRead(const std::string &filPath);

    template <typename T> void Read(T &value);
    // size값을 지정하여 읽는다.
    // oBuffer는 size만큼의 크기가 보장되어야한다.
    // 실제 읽은 바이트수를 리턴한다.
    size_t Read(void *oBuffer, size_t size);

    // 내부버퍼를 비운다.
    void FlushBuffer();

    // 읽은 파일의 사이즈(byte)를 얻는다
    uint64_t GetFileSize() const;

    bool SetReadPointer(uint64_t pointerPos);
    uint64_t GetReadPointer() const;

    bool IsEOF() const;

  private:
    QuadPF::PhysicalFileSystem *mPhysicalFileSystem;
    std::vector<uint8_t> mBuffer;
    uint64_t mReadPointer;
    std::stringstream mStringStream;
};

} // namespace QuadRW