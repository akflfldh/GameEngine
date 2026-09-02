#pragma once

#include <CoreAsset/AssetType.h>
namespace CoreAsset
{
class Asset;
}
namespace UI
{
class UIElement;
}

class Object;
class Component;
enum class EDragDropType
{
    eNone = 0,
    eAssetTexture,
    eAssetMaterial,
    eAssetStaticMesh,
    eObject,
    eObjectItem,
    ePrefab,
    eComponent
};

struct DragPayload
{
    EDragDropType mType;
    CoreAsset::AssetID mAssetID = NoneAssetID;
    Object *mObject = nullptr;
    UI::UIElement *mUIElement = nullptr;
    Component *mComponent = nullptr;
    // AssetBrower에서 사용하는 ICon 이미지를위한 필드가 있어야할듯
};