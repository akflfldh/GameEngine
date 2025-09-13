#include "TextWriter.h"
#include <PhysicalFileSystem/PhysicalFileSystem.h>
#include <iomanip>
#include <sstream>

namespace QuadRW
{

TextWriter::TextWriter()
{

    mPhysicalFileSystem = QuadPF::PhysicalFileSystem::GetInstance();

    mStringStream << std::setprecision(15) << std::boolalpha;
}

TextWriter::~TextWriter() {}

void TextWriter::StartWrite()
{
    //   mBuffer.clear();
    mStringStream.str({});
    mStringStream.clear();
}

bool TextWriter::Close(const char *file)
{

    std::string str = mStringStream.str().c_str();
    bool ret = mPhysicalFileSystem->WriteBufferToFile(file, (void *)str.c_str(), str.size());

    return ret;
}

template <typename T> void TextWriter::Write(T value)
{
    mStringStream << value;

    //  std::string str = ss.str();

    // mBuffer.insert(mBuffer.end(), reinterpret_cast<const uint8_t *>(str.data()),
    //     reinterpret_cast<const uint8_t *>(str.data() + str.size()));
}

void TextWriter::Write(const std::string &str)
{
    mStringStream << str;

    /*  mBuffer.insert(mBuffer.end(), reinterpret_cast<const uint8_t *>(str.data()),
                     reinterpret_cast<const uint8_t *>(str.data() + str.size()));*/
}

template void TextWriter::Write<char>(char);
template void TextWriter::Write<unsigned char>(unsigned char);
template void TextWriter::Write<short>(short);
template void TextWriter::Write<unsigned short>(unsigned short);
template void TextWriter::Write<int>(int);
template void TextWriter::Write<unsigned int>(unsigned int);
template void TextWriter::Write<long>(long);
template void TextWriter::Write<unsigned long>(unsigned long);
template void TextWriter::Write<long long>(long long);
template void TextWriter::Write<unsigned long long>(unsigned long long);
template void TextWriter::Write<float>(float);
template void TextWriter::Write<double>(double);
template void TextWriter::Write<bool>(bool);
template void TextWriter::Write<const char *>(const char *);
} // namespace QuadRW