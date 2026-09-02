#include "MapSettingUIController.h"
#include <Core/Map.h>
#include <UIReflectFloatPanel.h>
#include <UIReflectTextureSlotPanel.h>
#include <UIReflectVector3Panel.h>
#include <UIScrollBox.h>
#include <UiSystem/UICanvas.h>

MapSettingUIController *MapSettingUIController::GetInstance()
{

    static MapSettingUIController instance;
    return &instance;
}

MapSettingUIController::MapSettingUIController() {}

MapSettingUIController::~MapSettingUIController() {}

void MapSettingUIController::Initialize(UI::UICanvas *canvas, float width, float height,
                                        const CoreMath::Vector2 &panelPos)
{

    mCanvas = canvas;
    mScrollBoxWidth = width;
    mScrollBoxHeight = height;

    mScrollBoxPos = panelPos;
}

void MapSettingUIController::BeginUI()
{

    if (mCanvas == nullptr)
        return;

    // scrollBox
    mScrollBox = mCanvas->CreateUIElement<UIScrollBox>("ScrollBox");

    if (mScrollBox == nullptr)
        return;

#pragma region Ambient

    mScrollBox->SetSize(mScrollBoxWidth, mScrollBoxHeight);
    mScrollBox->SetBackgrounColor(0.4f, 0.4f, 0.4f);
    mScrollBox->SetPositionLocal(mScrollBoxPos.X, mScrollBoxPos.Y);
    mScrollBox->SetLayout(EUIScrollLayout::eVertical);

    mAmbientColorPanel = mCanvas->CreateUIElement<UIReflectVector3Panel>("AmbientColorPanel");
    mAmbientColorPanel->SetTagText("주변광 색상");
    mAmbientColorPanel->SetColor(0.36f, 0.36f, 0.36f);

    mAmbientColorPanel->BindVector3([this]() { return GetAmbientColor(); },
                                    [this](const CoreMath::Vector3 &value) { SetAmbientColor(value); });

    mScrollBox->AddItem(mAmbientColorPanel);

    mAmbientIntensityPanel = mCanvas->CreateUIElement<UIReflectFloatPanel>("AmbientIntensityPanel");

    mAmbientIntensityPanel->SetTagText("주변광 강도");
    mAmbientIntensityPanel->SetColor(0.36f, 0.36f, 0.36f);
    mAmbientIntensityPanel->BindFloat([this]() { return GetAmbientIntensity(); },
                                      [this](float value) { SetAmbientIntensity(value); });

    mScrollBox->AddItem(mAmbientIntensityPanel);

#pragma endregion

#pragma region skySphere

    mSkySphereTextureSlotPanel = mCanvas->CreateUIElement<UIReflectTextureSlotPanel>("SkySphereTextureSlotPanel");
    mSkySphereTextureSlotPanel->SetTagText("스카이 구");
    mSkySphereTextureSlotPanel->BindTexture([this]() { return GetSkySphereTexture(); },
                                            [this](CoreAsset::AssetID id) { SetSkySphereTexture(id); });
    mSkySphereTextureSlotPanel->SetWidth(mScrollBox->GetWidth());
    mSkySphereTextureSlotPanel->SetBackgroundColor(0.37, 0.37, 0.37);
    mScrollBox->AddItem(mSkySphereTextureSlotPanel);

#pragma endregion

    DeActivateInspector();
}

void MapSettingUIController::SetTargetMap(Map *map)
{

    mTargetMap = map;
    RebuildUI(mTargetMap);
}

void MapSettingUIController::ActivateInspector()
{

    if (mScrollBox)
    {
        mScrollBox->SetActiveFlag(true);
    }
}

void MapSettingUIController::DeActivateInspector()
{

    if (mScrollBox)
    {
        mScrollBox->SetActiveFlag(false);
    }
}

void MapSettingUIController::SetInspectorContext(const InspectorContext &inspectorContext)
{

    SetTargetMap(inspectorContext.mTargetMap);
}

void MapSettingUIController::RebuildUI(Map *map)
{

    if (map == nullptr)
    {
        // clear
        return;
    }

    mAmbientColorPanel->RefreshFromSource();
    mAmbientIntensityPanel->RefreshFromSource();
}

CoreMath::Vector3 MapSettingUIController::GetAmbientColor() const
{

    if (mTargetMap == nullptr)
        return {1, 1, 1};

    return mTargetMap->GetAmbientLightSettings().mColor;
}

void MapSettingUIController::SetAmbientColor(const CoreMath::Vector3 &value)
{

    if (mTargetMap == nullptr)
        return;

    mTargetMap->SetAmbientLightColor(value);
}

float MapSettingUIController::GetAmbientIntensity() const
{
    if (mTargetMap == nullptr)
        return 0.0f;

    return mTargetMap->GetAmbientLightSettings().mIntensity;
}

void MapSettingUIController::SetAmbientIntensity(float value)
{

    if (mTargetMap == nullptr)
        ;

    mTargetMap->SetAmbientLightIntensity(value);
}

CoreAsset::AssetID MapSettingUIController::GetSkySphereTexture() const
{

    if (mTargetMap == nullptr)
        return NoneAssetID;

    return mTargetMap->GetSkySphereSettings().mTexID;
}

void MapSettingUIController::SetSkySphereTexture(CoreAsset::AssetID id)
{

    if (mTargetMap == nullptr)
    {

        return;
    }

    mTargetMap->SetSkySphereTextureID(id);
}
