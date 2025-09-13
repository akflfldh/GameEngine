#pragma once

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
    bool Close(const std::string &path);

    template <typename T> void Write(T data);
    void Write(const std::string &str);

    // size값도 기록한다.
    void Write(const void *data, size_t size);
    // size값을 기록하지않는다.
    void WriteRaw(const void *data, size_t size);

    // size값을 기록하질 지 안할지 결정할수있다.
    // void Write(void * data , size_t size, bool writeSizeFlag)

  private:
    std::vector<uint8_t> mBuffer;
    QuadPF::PhysicalFileSystem *mPhysicalFileSystem;
};

} // namespace QuadRW
