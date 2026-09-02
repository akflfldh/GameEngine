#pragma once

#include <CoreAsset/AssetType.h>
#include <CoreMath/CoreMath.h>
#include <EditorDirector/IInspector.h>

/*
맵의 ambient light,sky sphere등등의 ui를 관리하는 클래스

*/

namespace UI
{
class UICanvas;
}
class Map;
class UIScrollBox;
class UIReflectVector3Panel;
class UIReflectFloatPanel;
class UIReflectTextureSlotPanel;

class MapSettingUIController : public IInspector
{
  public:
    static MapSettingUIController *GetInstance();
    MapSettingUIController();
    ~MapSettingUIController();

    void Initialize(UI::UICanvas *canvas, float panelWidth, float panelHeight, const CoreMath::Vector2 &panelPos);

    virtual void BeginUI() override;

    void SetTargetMap(Map *map);

    virtual void ActivateInspector() override;
    virtual void DeActivateInspector() override;
    virtual void SetInspectorContext(const InspectorContext &inspectorContext) override;

  private:
#pragma region Rebuild

    void RebuildUI(Map *map);

#pragma endregion

    CoreMath::Vector3 GetAmbientColor() const;
    void SetAmbientColor(const CoreMath::Vector3 &value);

    float GetAmbientIntensity() const;
    void SetAmbientIntensity(float value);

    CoreAsset::AssetID GetSkySphereTexture() const;
    void SetSkySphereTexture(CoreAsset::AssetID id);

  private:
    UI::UICanvas *mCanvas = nullptr;
    Map *mTargetMap = nullptr;

    UIScrollBox *mScrollBox = nullptr;
    float mScrollBoxWidth = 0.0f;
    float mScrollBoxHeight = 0.0f;
    CoreMath::Vector2 mScrollBoxPos;

    UIReflectVector3Panel *mAmbientColorPanel = nullptr;
    UIReflectFloatPanel *mAmbientIntensityPanel = nullptr;
    UIReflectTextureSlotPanel *mSkySphereTextureSlotPanel = nullptr;
};
