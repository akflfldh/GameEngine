#include "ShaderSourceLibrary.h"
#include <utility>

Render::ShaderSourceLibrary *Render::ShaderSourceLibrary::GetInstance()
{
    static ShaderSourceLibrary instance;

    return &instance;
}

Render::ShaderSourceLibrary::ShaderSourceLibrary() {}

Render::ShaderSourceLibrary::~ShaderSourceLibrary() {}

void Render::ShaderSourceLibrary::SetShaderBytecodeTable(ShaderBytecodeTable table)
{
    // 외부의 임시 테이블 수명에 의존하지 않도록 키와 바이트코드를 함께 소유한다.
    mShaderBytecodeTable = std::move(table);
}

const std::vector<uint8_t> *Render::ShaderSourceLibrary::GetShaderBytecode(const ShaderVariantKey &key) const
{
    auto it = mShaderBytecodeTable.find(key);
    return it != mShaderBytecodeTable.end() ? &it->second : nullptr;
}

std::vector<Render::ShaderVariantKey> Render::ShaderSourceLibrary::GetShaderKeyList() const
{
    std::vector<Render::ShaderVariantKey> keyList;

    for (const auto &entry : mShaderBytecodeTable)
    {
        keyList.push_back(entry.first);
    }

    return keyList;
}