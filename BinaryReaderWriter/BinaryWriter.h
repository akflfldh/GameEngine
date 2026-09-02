#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace QuadPF
{
class PhysicalFileSystem;
}

namespace QuadRW
{

class BinaryWriter
{
  public:
    BinaryWriter();
    ~BinaryWriter();

    // 맨처음사용 또는 close후 호출
    void StartWrite();

    // buffer기록한것을 파일에 출력한다.
    bool Close(const std::filesystem::path &filePath);
    bool Close(); // 기록안하는버전

    template <typename T> void Write(T data);
    void Write(const std::string &str);

    // size값도 기록한다.
    void Write(const void *data, size_t size);
    // size값을 기록하지않는다.
    void WriteRaw(const void *data, size_t size);

    // size값을 기록하질 지 안할지 결정할수있다.
    // void Write(void * data , size_t size, bool writeSizeFlag)

    // write pointer 기능도 추가해서 원하는위치에 write할수있게 해야한다.
    // current  write pointer위치를 옮기지는않는다.
    void WriteRaw(const void *data, size_t size, size_t writePointer);

    uint64_t GetCurrentWritePointer() const;

    // 호출이후, 새로운 write하기전까지만 유효
    uint8_t *GetBufferPointer();
    size_t GetBufferSize() const;

  private:
    std::vector<uint8_t> mBuffer;
    QuadPF::PhysicalFileSystem *mPhysicalFileSystem;

    uint64_t mWritePointer;
};

} // namespace QuadRW
