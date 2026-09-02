#pragma once

#include <CoreAsset/CoreAssetDLLMacro.h>
#include <ReflectSystem/ReflectionSystem.h>

class CORE_ASSET_API AssetPtrDescriptor : public TypeDescriptor
{
  public:
    virtual void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo = nullptr);
    // void Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo);
};
