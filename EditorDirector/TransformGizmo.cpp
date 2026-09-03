#include "TransformGizmo.h"
#include "CameraComponentVisualizer.h"
#include <Core/Entity.h>
#include <Core/IHittable.h>
#include <Core/Map.h>
#include <CoreMath/Geometry.h>
#include <EditorDirector/GizmoMeshComponent.h>

// TODO
/*
마우스로 pos축을 움직일때 기즈모의 사이즈가 변하지않아 hit문제가 발생함
다시 카메라를 조금이라도 움직이면 scale이 다시 계산되어 정상크기로돌아오고 hit도 올바르게변하는 현상임

*/

Quad::TransformGizmo::TransformGizmo() : mMode(EGizmoMode::eTranslation) {}

Quad::TransformGizmo::~TransformGizmo() {}

void Quad::TransformGizmo::Initialize(Map *editorMap)
{
    mGizmoRoot = editorMap->CreateEntity<Entity>("GizmoRootEntity");

    mPosScaleAxisRoot = editorMap->CreateEntity<Entity>("GizmoPosScaleRootEntity");
    mPosScaleAxisRoot->SetParent(mGizmoRoot);
    mRotationAxisRoot = editorMap->CreateEntity<Entity>("GizmoRotationRootEntity");
    mRotationAxisRoot->SetParent(mGizmoRoot);
    mScaleAxisRoot = editorMap->CreateEntity<Entity>("GizmoScaleRootEntity");
    mScaleAxisRoot->SetParent(mGizmoRoot);

#pragma region PosAxis

    // AxisX
    mAxisX = editorMap->CreateEntity<Entity>("GizmoAxisX");
    auto AxisXStaticMeshCom =
        static_cast<Quad::GizmoMeshComponent *>(mAxisX->CreateComponent<Quad::GizmoMeshComponent>("AxisStaticMeshCom"));
    if (AxisXStaticMeshCom)
    {
        AxisXStaticMeshCom->SetMesh("Engine/Arrow");
        AxisXStaticMeshCom->SetParent(mAxisX->GetRootComponent());
        AxisXStaticMeshCom->SetColor({1, 0, 0, 1});
    }
    mAxisX->SetParent(mPosScaleAxisRoot);
    mAxisX->SetRotationLocal(0, 0, -90);

    // AxisY
    mAxisY = editorMap->CreateEntity<Entity>("GizmoAxisY");
    auto AxisYStaticMeshCom =
        static_cast<Quad::GizmoMeshComponent *>(mAxisY->CreateComponent<Quad::GizmoMeshComponent>("AxisStaticMeshCom"));
    if (AxisYStaticMeshCom)
    {
        AxisYStaticMeshCom->SetMesh("Engine/Arrow");
        AxisYStaticMeshCom->SetParent(mAxisY->GetRootComponent());
        AxisYStaticMeshCom->SetColor({0, 1, 0, 1});
    }
    mAxisY->SetParent(mPosScaleAxisRoot);
    //   mAxisY->SetRotationLocal(0, 0, 0);

    // AxisZ
    mAxisZ = editorMap->CreateEntity<Entity>("GizmoAxisZ");
    auto AxisZStaticMeshCom =
        static_cast<Quad::GizmoMeshComponent *>(mAxisZ->CreateComponent<Quad::GizmoMeshComponent>("AxisStaticMeshCom"));
    if (AxisZStaticMeshCom)
    {
        AxisZStaticMeshCom->SetMesh("Engine/Arrow");
        AxisZStaticMeshCom->SetParent(mAxisZ->GetRootComponent());
        AxisZStaticMeshCom->SetColor({0, 0, 1, 1});
    }
    mAxisZ->SetParent(mPosScaleAxisRoot);
    mAxisZ->SetRotationLocal(90, 0, 0);

#pragma endregion

#pragma region Scale Axis

    mScaleAxisX = editorMap->CreateEntity<Entity>("GizmoScaleAxisX");
    auto scaleAxisXStaticMeshCom = static_cast<Quad::GizmoMeshComponent *>(
        mScaleAxisX->CreateComponent<Quad::GizmoMeshComponent>("AxisStaticMeshCom"));
    if (scaleAxisXStaticMeshCom)
    {
        scaleAxisXStaticMeshCom->SetMesh("Engine/ArrowCube");
        scaleAxisXStaticMeshCom->SetParent(mScaleAxisX->GetRootComponent());
        scaleAxisXStaticMeshCom->SetColor({1, 0, 0, 1});
    }
    mScaleAxisX->SetParent(mScaleAxisRoot);
    mScaleAxisX->SetRotationLocal(0, 0, -90);

    // AxisY
    mScaleAxisY = editorMap->CreateEntity<Entity>("GizmoScaleAxisY");
    auto scaleAxisYStaticMeshCom = static_cast<Quad::GizmoMeshComponent *>(
        mScaleAxisY->CreateComponent<Quad::GizmoMeshComponent>("AxisStaticMeshCom"));
    if (scaleAxisYStaticMeshCom)
    {
        scaleAxisYStaticMeshCom->SetMesh("Engine/ArrowCube");
        scaleAxisYStaticMeshCom->SetParent(mScaleAxisY->GetRootComponent());
        scaleAxisYStaticMeshCom->SetColor({0, 1, 0, 1});
    }
    mScaleAxisY->SetParent(mScaleAxisRoot);
    //   mAxisY->SetRotationLocal(0, 0, 0);

    // AxisZ
    mScaleAxisZ = editorMap->CreateEntity<Entity>("GizmoScaleAxisZ");
    auto scaleAxisZStaticMeshCom = static_cast<Quad::GizmoMeshComponent *>(
        mScaleAxisZ->CreateComponent<Quad::GizmoMeshComponent>("AxisStaticMeshCom"));
    if (scaleAxisZStaticMeshCom)
    {
        scaleAxisZStaticMeshCom->SetMesh("Engine/ArrowCube");
        scaleAxisZStaticMeshCom->SetParent(mScaleAxisZ->GetRootComponent());
        scaleAxisZStaticMeshCom->SetColor({0, 0, 1, 1});
    }
    mScaleAxisZ->SetParent(mScaleAxisRoot);
    mScaleAxisZ->SetRotationLocal(90, 0, 0);

#pragma endregion

    mRotationAxisX = editorMap->CreateEntity<Entity>("RotationAxisX");
    auto AxisRotationMeshCom = static_cast<Quad::GizmoMeshComponent *>(
        mRotationAxisX->CreateComponent<Quad::GizmoMeshComponent>("AxisStaticMeshCom"));
    if (AxisRotationMeshCom)
    {
        AxisRotationMeshCom->SetMesh("Engine/Quad");
        AxisRotationMeshCom->SetParent(mRotationAxisX->GetRootComponent());
        AxisRotationMeshCom->SetColor({1, 0, 0, 1});
    }
    mRotationAxisX->SetParent(mRotationAxisRoot);
    mRotationAxisX->SetRotationLocal(0, 0, 90);

    mRotationAxisY = editorMap->CreateEntity<Entity>("RotationAxisY");
    AxisRotationMeshCom = static_cast<Quad::GizmoMeshComponent *>(
        mRotationAxisY->CreateComponent<Quad::GizmoMeshComponent>("AxisStaticMeshCom"));
    if (AxisRotationMeshCom)
    {
        AxisRotationMeshCom->SetMesh("Engine/Quad");
        AxisRotationMeshCom->SetParent(mRotationAxisY->GetRootComponent());
        AxisRotationMeshCom->SetColor({0, 1, 0, 1});
    }
    mRotationAxisY->SetParent(mRotationAxisRoot);
    mRotationAxisY->SetRotationLocal(0, 0, 0);

    mRotationAxisZ = editorMap->CreateEntity<Entity>("RotationAxisZ");
    AxisRotationMeshCom = static_cast<Quad::GizmoMeshComponent *>(
        mRotationAxisZ->CreateComponent<Quad::GizmoMeshComponent>("AxisStaticMeshCom"));
    if (AxisRotationMeshCom)
    {
        AxisRotationMeshCom->SetMesh("Engine/Quad");
        AxisRotationMeshCom->SetParent(mRotationAxisZ->GetRootComponent());
        AxisRotationMeshCom->SetColor({0, 0, 1, 1});
    }
    mRotationAxisZ->SetParent(mRotationAxisRoot);
    mRotationAxisZ->SetRotationLocal(-90, 0, 0);

    mGizmoRoot->SetActive(false);
    mPosScaleAxisRoot->SetActive(false);
    mRotationAxisRoot->SetActive(false);
}

void Quad::TransformGizmo::SetTargetObject(Entity *entity)
{

    mTargetObject = entity;
    if (mTargetObject)
        mGizmoRoot->SetPositionWorld(mTargetObject->GetPositionWorld());
    else
    {
    }
}

void Quad::TransformGizmo::SetTargetComponent(Component *com)
{

    mTargetCom = dynamic_cast<SceneComponent *>(com);
    if (mTargetCom)
    {
        mGizmoRoot->SetPositionWorld(mTargetCom->GetPositionWorld());
    }
}

void Quad::TransformGizmo::Update(CameraComponent *cameraComponent) {}

void Quad::TransformGizmo::SetActive(bool state)
{

    // mGizmoRoot->
    mGizmoRoot->SetActive(state);

    if (!state)
    {
        SetSelectStateFalse();
        mTargetObject = nullptr;
        mTargetCom = nullptr;
    }
}

bool Quad::TransformGizmo::GetActiveState() const
{

    return mGizmoRoot->GetActive();
}

bool Quad::TransformGizmo::RayHit(const CoreMath::Ray &ray, Core::HitResult &oHitResult) const
{

    bool bRet = false;

    switch (mMode)
    {
    case EGizmoMode::eTranslation:
    {

        oHitResult.mDistance = FLT_MAX;
        Core::HitResult xHitResult;
        bool bHit = mAxisX->RayHit(ray, xHitResult);

        if (bHit)
        {
            oHitResult = xHitResult;
            mHittedAxisIndex = 0;
            bRet = true;
        }
        Core::HitResult yHitResult;
        bHit = mAxisY->RayHit(ray, yHitResult);

        if (bHit)
        {
            if (yHitResult.mDistance < oHitResult.mDistance)
            {
                mHittedAxisIndex = 1;
                oHitResult = yHitResult;
            }
            bRet = true;
        }
        Core::HitResult zHitResult;
        bHit = mAxisZ->RayHit(ray, zHitResult);

        if (bHit)
        {
            if (zHitResult.mDistance < oHitResult.mDistance)
            {
                mHittedAxisIndex = 2;
                oHitResult = zHitResult;
            }
            bRet = true;
        }

        if (bRet)
        {
            return true;
        }
    }
    break;
    case EGizmoMode::eScale:
    {
        oHitResult.mDistance = FLT_MAX;
        Core::HitResult xHitResult;
        bool bHit = mScaleAxisX->RayHit(ray, xHitResult);

        if (bHit)
        {
            oHitResult = xHitResult;
            mHittedAxisIndex = 0;
            bRet = true;
        }
        Core::HitResult yHitResult;
        bHit = mScaleAxisY->RayHit(ray, yHitResult);

        if (bHit)
        {
            if (yHitResult.mDistance < oHitResult.mDistance)
            {
                mHittedAxisIndex = 1;
                oHitResult = yHitResult;
            }
            bRet = true;
        }
        Core::HitResult zHitResult;
        bHit = mScaleAxisZ->RayHit(ray, zHitResult);

        if (bHit)
        {
            if (zHitResult.mDistance < oHitResult.mDistance)
            {
                mHittedAxisIndex = 2;
                oHitResult = zHitResult;
            }
            bRet = true;
        }

        if (bRet)
        {
            return true;
        }
    }
    break;
    case EGizmoMode::eRotation:
    {
        oHitResult.mDistance = FLT_MAX;
        Core::HitResult xHitResult;
        bool bHit = mRotationAxisX->RayHit(ray, xHitResult);

        if (bHit)
        {
            oHitResult = xHitResult;
            mHittedAxisIndex = 0;
            bRet = true;
        }
        Core::HitResult yHitResult;
        bHit = mRotationAxisY->RayHit(ray, yHitResult);

        if (bHit)
        {
            if (yHitResult.mDistance < oHitResult.mDistance)
            {
                mHittedAxisIndex = 1;
                oHitResult = yHitResult;
            }
            bRet = true;
        }
        Core::HitResult zHitResult;
        bHit = mRotationAxisZ->RayHit(ray, zHitResult);

        if (bHit)
        {
            if (zHitResult.mDistance < oHitResult.mDistance)
            {
                mHittedAxisIndex = 2;
                oHitResult = zHitResult;
            }
            bRet = true;
        }

        if (bRet)
        {
            return true;
        }
    }
    break;
    }

    return false;
}

void Quad::TransformGizmo::SetSelectState(bool state, const CoreMath::Ray &ray)
{

    mSelectState = state;
    if (mSelectState == false)
    {
        mHittedAxisIndex = -1;
        mVirtualPlaneActive = false;
    }
    else
    {
        SetVirtualPlane(ray);
    }
}

bool Quad::TransformGizmo::GetSelectState() const
{
    return mSelectState;
}

void Quad::TransformGizmo::OnMouseMove(const Core::InputData &inputData, const CoreMath::Ray &ray)
{

    // TODO
    // Mode - scale, rotation , translation
    if ((mHittedAxisIndex == -1) || (mTargetObject == nullptr && mTargetCom == nullptr))
        return;

    float t = 0.0f;
    if (CoreMath::IntersectRayPlane(ray, mVirtualPlaneNormal, mVirtualPlanePoint, t))
    {

        CoreMath::Vector3 selectCurrentPos = ray.mOrigin + ray.mDirection * t;
        CoreMath::Vector3 mouseShiftWorldVec = (selectCurrentPos - mSelectStartPos);
        CoreMath::Vector3 shiftVec = CoreMath::Dot(mouseShiftWorldVec, mCurrAxis) * mCurrAxis;

        switch (mMode)
        {
        case EGizmoMode::eTranslation:
        {
            //  mTargetObject->TranslationWorld(shiftVec);
            TranslationTarget(shiftVec);

            //  mGizmoRoot->SetPositionWorld(mTargetObject->GetPositionWorld());
            UpdateGizmoSize(ray.mOrigin);
        }
        break;
        case EGizmoMode::eScale:
        {

            ScaleTarget(shiftVec);
        }
        break;
        case EGizmoMode::eRotation:
        {
            CoreMath::Vector3 center = mGizmoRoot->GetPositionWorld();
            CoreMath::Vector3 v1 = (mSelectStartPos - center).GetNormalize();
            CoreMath::Vector3 v2 = (selectCurrentPos - center).GetNormalize();

            float c = CoreMath::Dot(v1, v2);
            float s = CoreMath::Dot(mCurrAxis, v1.Cross(v2));

            float thetaRadian = atan2(s, c);
            float thetaDegree = CoreMath::RadianToDegree(thetaRadian);

            CoreMath::Vector3 rotation = thetaDegree * mCurrAxis;

            RotateTarget(rotation);
        }
        break;
        }
        mSelectStartPos = selectCurrentPos;
    }
}

void Quad::TransformGizmo::UpdateGizmoSize(const CoreMath::Vector3 &cameraPosWorld)
{
    float distance = (mGizmoRoot->GetPositionWorld() - cameraPosWorld).Length();
    float scale = distance / 5.0f;
    mGizmoRoot->SetScaleLocal({scale, scale, scale});
    // mPosScaleAxisRoot->SetScaleLocal({scale, scale, scale});
    // mRotationAxisRoot->SetScaleLocal({scale, scale, scale});
    // 기즈모의 사이즈는 카메라가 이동하면서 변경되는데 회전모드일경우 부호도 카메라와 기즈모의 위치에따라
    //     바뀐다.
    if (mMode == EGizmoMode::eRotation)
    {
        CoreMath::Vector3 gizmoPosWorld = mGizmoRoot->GetPositionWorld();
        CoreMath::Vector3 scaleAxisX = mRotationAxisX->GetScaleLocal();
        CoreMath::Vector3 scaleAxisY = mRotationAxisY->GetScaleLocal();
        CoreMath::Vector3 scaleAxisZ = mRotationAxisZ->GetScaleLocal();

        if (cameraPosWorld.Y > gizmoPosWorld.Y)
        {
            // x축기준회전사분원은 위에위치해있어야한다.
            scaleAxisX.X = std::abs(scaleAxisX.X);
            scaleAxisZ.Z = std::abs(scaleAxisZ.Z);
        }
        else
        {
            scaleAxisX.X = -1.0f * std::abs(scaleAxisX.X);
            scaleAxisZ.Z = -1.0f * std::abs(scaleAxisZ.Z);
        }

        if (cameraPosWorld.X > gizmoPosWorld.X)
        {
            scaleAxisZ.X = std::abs(scaleAxisZ.X);
            scaleAxisY.X = std::abs(scaleAxisY.X);
        }
        else
        {
            scaleAxisZ.X = -1.0f * std::abs(scaleAxisZ.X);
            scaleAxisY.X = -1.0f * std::abs(scaleAxisY.X);
        }

        if (cameraPosWorld.Z > gizmoPosWorld.Z)
        {
            scaleAxisX.Z = std::abs(scaleAxisX.Z);
            scaleAxisY.Z = std::abs(scaleAxisY.Z);
        }
        else
        {
            scaleAxisX.Z = -1.0f * std::abs(scaleAxisZ.Z);
            scaleAxisY.Z = -1.0f * std::abs(scaleAxisY.Z);
        }
        mRotationAxisX->SetScaleLocal(scaleAxisX);
        mRotationAxisY->SetScaleLocal(scaleAxisY);
        mRotationAxisZ->SetScaleLocal(scaleAxisZ);
    }
}

void Quad::TransformGizmo::SetMode(EGizmoMode mode)
{

    mMode = mode;

    mRotationAxisRoot->SetActive(false);
    mPosScaleAxisRoot->SetActive(false);
    mScaleAxisRoot->SetActive(false);

    switch (mode)
    {
    case EGizmoMode::eTranslation:
    {

        mPosScaleAxisRoot->SetActive(true);
    }
    break;
    case EGizmoMode::eScale:
    {

        mScaleAxisRoot->SetActive(true);
    }
    break;
    case EGizmoMode::eRotation:
    {

        mRotationAxisRoot->SetActive(true);
    }
    break;
    }
}

void Quad::TransformGizmo::SetSelectStateFalse()
{
    mSelectState = false;
    mHittedAxisIndex = -1;
}

void Quad::TransformGizmo::SetComponentControlState(bool state)
{

    mComponentControlState = state;
}

bool Quad::TransformGizmo::GetComponentControlState() const
{
    return mComponentControlState;
}

void Quad::TransformGizmo::SetVirtualPlane(const CoreMath::Ray &ray)
{

    if (mHittedAxisIndex == -1)
    {
        Core::HitResult hitresult;
        if (RayHit(ray, hitresult) == false)
            return;
    }

    // 가상의 평면을 설정하자
    if (mHittedAxisIndex == 0)
        mCurrAxis = {1, 0, 0};
    else if (mHittedAxisIndex == 1)
        mCurrAxis = {0, 1, 0};
    else if (mHittedAxisIndex == 2)
        mCurrAxis = {0, 0, 1};

    if (mMode == EGizmoMode::eRotation)
    {
        mVirtualPlaneNormal = mCurrAxis;
    }
    else
    {
        // (1,0,0)
        // v - (a*v)a
        mVirtualPlaneNormal = ray.mDirection - (CoreMath::Dot(mCurrAxis, ray.mDirection) * mCurrAxis);
        mVirtualPlaneNormal.Normalize();
    }
    mVirtualPlanePoint = mGizmoRoot->GetPositionWorld();

    float t = 0.0f;
    if (CoreMath::IntersectRayPlane(ray, mVirtualPlaneNormal, mGizmoRoot->GetPositionWorld(), t))
    {
        mSelectStartPos = ray.mOrigin + t * ray.mDirection;
    }
}

void Quad::TransformGizmo::TranslationTarget(const CoreMath::Vector3 &shift)
{

    if (mComponentControlState)
    {
        mTargetCom->TranslationLocal(shift);
        mGizmoRoot->SetPositionWorld(mTargetCom->GetPositionWorld());
    }
    else
    {
        mTargetObject->TranslationWorld(shift);
        mGizmoRoot->SetPositionWorld(mTargetObject->GetPositionWorld());
    }
}

void Quad::TransformGizmo::ScaleTarget(const CoreMath::Vector3 &scale)
{

    if (mComponentControlState)
    {
        CoreMath::Vector3 comScale = mTargetCom->GetScaleLocal();
        comScale += scale;

        mTargetCom->SetScaleLocal(comScale);
    }
    else
    {
        CoreMath::Vector3 objectScale = mTargetObject->GetScaleLocal();
        objectScale += scale;

        mTargetObject->SetScaleLocal(objectScale);
    }
}

void Quad::TransformGizmo::RotateTarget(const CoreMath::Vector3 &rotate)
{

    if (mComponentControlState)
    {

        mTargetCom->AddRotationLocal(rotate);
    }
    else
    {
        mTargetObject->AddRotationLocal(rotate);
    }
}
