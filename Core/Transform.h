#pragma once
#include <Core/CoreDllExport.h>
#include <CoreMath/CoreMath.h>
#include <ReflectSystem/ReflectionMacro.h>

namespace Core
{

class SceneComponent;

class CORE_API_LIB Transform
{
  public:
    Transform();
    ~Transform();

    // --- 로컬 프로퍼티 Getters ---
    // const& (상수 참조)로 반환하여 불필요한 데이터 복사를 방지하고 성능을 높입니다.
    // const 함수로 선언하여 이 함수가 객체의 상태를 변경하지 않음을 명시합니다.
    const CoreMath::Vector3 &GetScaleLocal() const;
    const CoreMath::Quaternion &GetQuaternionLocal() const;
    const CoreMath::Vector3 &GetPositionLocal() const;

    // --- 로컬 프로퍼티 Setters ---
    // 이 함수들을 통해 로컬 값이 변경되면, mDirtyLocal 플래그가 자동으로 설정됩니다.
    void SetScaleLocal(const CoreMath::Vector3 &scale);
    void SetQuaternionLocal(const CoreMath::Quaternion &quaternion);
    void SetPositionLocal(const CoreMath::Vector3 &position);

    // --- 월드 프로퍼티 Getters (캐시된 데이터 접근) ---
    const CoreMath::Vector3 &GetScaleWorld() const;
    const CoreMath::Quaternion &GetQuaternionWorld() const;
    const CoreMath::Vector3 &GetPositionWorld() const;

    // --- 행렬 Getters (캐시된 데이터 접근) ---
    const CoreMath::Matrix4X4 &GetTransformLocal() const;
    const CoreMath::Matrix4X4 &GetTransformWorld() const;

    // --- 캐시 데이터 Setters (주로 SceneComponent가 사용) ---
    // SceneComponent가 계산한 결과를 Transform 내부에 캐싱하기 위한 함수들입니다.
    void SetTransformLocal(const CoreMath::Matrix4X4 &matrix) const;
    void SetTransformWorld(const CoreMath::Matrix4X4 &matrix) const;
    void SetScaleWorld(const CoreMath::Vector3 &scale) const;
    void SetQuaternionWorld(const CoreMath::Quaternion &quaternion) const;
    void SetPositionWorld(const CoreMath::Vector3 &position) const;

    // --- 상태 관리(State Management) ---
    bool IsDirty() const;
    void ClearDirtyFlag() const;

  private:
    // 로컬 프로퍼티는 이 클래스가 소유한 원본 데이터입니다.
    CoreMath::Vector3 mScaleLocal;
    CoreMath::Quaternion mQuaternionLocal;
    CoreMath::Vector3 mPositionLocal;

    // 월드 프로퍼티는 SceneComponent에 의해 계산된 결과가 캐싱되는 곳입니다.
    mutable CoreMath::Vector3 mScaleWorld;
    mutable CoreMath::Quaternion mQuaternionWorld;
    mutable CoreMath::Vector3 mPositionWorld;

    // 행렬 또한 계산된 결과가 캐싱되는 곳입니다.
    mutable CoreMath::Matrix4X4 mTransformLocal;
    mutable CoreMath::Matrix4X4 mTransformWorld;

    // 내부 상태 변수
    mutable bool mDirtyLocal;
};

} // namespace Core