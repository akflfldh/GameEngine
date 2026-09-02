#include "AssetPtrDescriptor.h"
#include <CoreAsset/AssetPtr.h>
#include <memory>
void AssetPtrDescriptor::Serialize(Arch &arch, void *data, Quad::PropertyInfo *propertyInfo)
{

    // 클래스정보 GET

    // ASSET ID  - > 정수

    // 저장 로드 .

    CoreAsset::AssetPtr *assetPtr = (CoreAsset::AssetPtr *)(data);

    std::string typeName = "AssetPtr";
    // propertyInfo->mType;
    arch.StartTable(typeName);

    if (arch.GetLoadingFlag())
    {

        std::string propertyName;
        std::string propertyType;
        uint32_t propertyValueSize = 0;
        arch.ReadPropertyHeader(propertyName, propertyType, propertyValueSize);

        if (propertyName == "AssetID" && propertyType == "AssetID")
        {
            CoreAsset::AssetID id = 0;
            arch << id;

            assetPtr->SetAsset(id);
        }
        arch.ReadPropertyHeader(propertyName, propertyType, propertyValueSize);
    }
    else
    {
        // 어차피 루프 한번

        std::string propertyName;
        std::string propertyType;
        uint32_t propertyValueSize = 0;

        arch.WritePropertyHeader("AssetID", "AssetID");
        CoreAsset::AssetID id = assetPtr->GetAssetID();
        arch << id;
        arch.EndProperty();

        arch.WritePropertyHeader("None", "None");
        arch.EndProperty();
    }

    arch.EndTable();
}

struct AssetPtrDescriptorRegister
{
    AssetPtrDescriptorRegister()
    {

        auto reflectSystem = Quad::ReflectionSystem::GetInstance();
        auto assetPtrDescriptor = std::make_unique<AssetPtrDescriptor>();

        reflectSystem->RegisterTypeDescriptor("AssetPtr", std::move(assetPtrDescriptor));
    }
};

static AssetPtrDescriptorRegister assetPtrDescriptorRegister;