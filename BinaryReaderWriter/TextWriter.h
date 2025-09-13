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

class TextWriter
{
  public:
    TextWriter();
    ~TextWriter();

    void StartWrite();

    template <typename T> void Write(T value);

    void Write(const std::string &value);

    bool Close(const char *file);

  private:
    std::stringstream mStringStream;
    std::vector<uint8_t> mBuffer;
    QuadPF::PhysicalFileSystem *mPhysicalFileSystem;
};

} // namespace QuadRW