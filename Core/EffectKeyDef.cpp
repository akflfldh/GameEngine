#include "EffectKeyDef.h"

const char* Quad::EffectKeyDef::toString(EPassKey passKey)
{
    switch (passKey)
    {
    case EPassKey::eType:return "TYPE";
    case EPassKey::eShader:return "SHADER";
    case EPassKey::eConfig:return "CONFIG";

    }

    return nullptr;
}

const char* Quad::EffectKeyDef::toString(EShaderKey shaderKey)
{
    switch (shaderKey)
    {
    case EShaderKey::eFile:return "File";
    case EShaderKey::eEntryPoint:return "EntryPoint";
    case EShaderKey::eVersion:return "Version";

    }


    return nullptr;
}

const char* Quad::EffectKeyDef::toString(EShaderNameKey shaderNameKey)
{
    switch (shaderNameKey)
    {
    case EShaderNameKey::eVS:return "VS";
    case EShaderNameKey::eGS:return "GS";
    case EShaderNameKey::ePS:return "PS";

    }

    return nullptr;
}
