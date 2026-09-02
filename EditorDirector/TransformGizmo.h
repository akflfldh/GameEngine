#pragma once

#include <CoreMath/Geometry.h>
namespace CoreMath
{
struct Ray;
}

class Entity;
class Map;
class CameraComponent;
class Component;
class SceneComponent;
namespace Core
{

struct HitResult;
struct InputData;

} // namespace Core

namespace Quad
{

enum class EGizmoMode
{
    eTranslation = 0,
    eScale,
    eRotation
};

class TransformGizmo
{
  public:
    TransformGizmo();
    ~TransformGizmo();

    void Initialize(Map *editorMap);
    void SetTargetObject(Entity *entity);
    void SetTargetComponent(Component *com);

    void Update(CameraComponent *cameraComponent);

    void SetActive(bool state);
    bool GetActiveState() const;
    bool RayHit(const CoreMath::Ray &ray, Core::HitResult &oHitResult) const;

    void SetSelectState(bool state, const CoreMath::Ray &ray);
    bool GetSelectState() const;

    void OnMouseMove(const Core::InputData &inputData, const CoreMath::Ray &ray);

    void UpdateGizmoSize(const CoreMath::Vector3 &targetPosWorld);

    void ChangeMode();

    void SetMode(EGizmoMode mode);

    void SetSelectStateFalse();

    // 컴포넌트,오브젝트 조작여부
    void SetComponentControlState(bool state);
    bool GetComponentControlState() const;

  private:
    void SetVirtualPlane(const CoreMath::Ray &ray);
    void TranslationTarget(const CoreMath::Vector3 &shift);
    void ScaleTarget(const CoreMath::Vector3 &scale);
    void RotateTarget(const CoreMath::Vector3 &rotate);

  private:
    EGizmoMode mMode;
    // 조작할 대상 (선택된 게임 오브젝트)
    Entity *mTargetObject = nullptr;

    SceneComponent *mTargetCom = nullptr;

    Entity *mGizmoRoot = nullptr;        // 최상위 루트엔티티
    Entity *mPosScaleAxisRoot = nullptr; // 위치 루트 엔티티
    Entity *mScaleAxisRoot = nullptr;    // 스케일 루트 엔티티
    Entity *mRotationAxisRoot = nullptr; // 회전 루트 엔티티

    Entity *mAxisX = nullptr; // 빨간 화살표
    Entity *mAxisY = nullptr; // 초록 화살표
    Entity *mAxisZ = nullptr; // 파란 화살표

    Entity *mScaleAxisX = nullptr; // 빨간
    Entity *mScaleAxisY = nullptr; // 초록
    Entity *mScaleAxisZ = nullptr; // 파란

    Entity *mRotationAxisX = nullptr;
    Entity *mRotationAxisY = nullptr;
    Entity *mRotationAxisZ = nullptr;

    bool mSelectState = false;
    mutable int mHittedAxisIndex = -1;
    bool mVirtualPlaneActive = false;

    CoreMath::Vector3 mVirtualPlaneNormal;
    CoreMath::Vector3 mVirtualPlanePoint;
    CoreMath::Vector3 mSelectStartPos;
    CoreMath::Vector3 mCurrAxis;

    // com 단위 , object단위 조작인지 여부
    bool mComponentControlState = false;
};

} // namespace Quad