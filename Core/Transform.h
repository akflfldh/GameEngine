#pragma once

#include <Core/CoreDllExport.h>
#include <CoreMath/CoreMath.h>
#include <ReflectSystem/ReflectionMacro.h>

#include "Transform.generated.h"

class Arch;

class SceneComponent;
namespace Core
{

// Rotation 단위는 Degree

struct CORE_API_LIB REFLECT_STRUCT(EngineClass) Transform
{
    GENERATED_BODY(Transform)
  public:
    Transform();
    ~Transform();

    // --- 로컬 프로퍼티 Getters ---
    const CoreMath::Vector3 &GetScaleLocal() const;
    const CoreMath::Quaternion &GetQuaternionLocal() const;
    const CoreMath::Vector3 GetRotationLocal() const; // 오일러 각 반환
    const CoreMath::Vector3 &GetPositionLocal() const;

    // --- 로컬 프로퍼티 Setters ---
    void SetScaleLocal(const CoreMath::Vector3 &scale);
    void SetQuaternionLocal(const CoreMath::Quaternion &quaternion);
    void SetRotationLocal(const CoreMath::Vector3 &rotation); // 오일러 각 설정 Degree
    void SetPositionLocal(const CoreMath::Vector3 &position);

    // --Local Add
    void AddQuaternionLocal(const CoreMath::Quaternion &quaternion);
    void AddRotationLocal(const CoreMath::Vector3 &rotation);

    // --- 월드 프로퍼티 Getters (캐시된 데이터 접근) ---
    const CoreMath::Vector3 &GetScaleWorld() const;
    const CoreMath::Quaternion &GetQuaternionWorld() const;
    const CoreMath::Vector3 GetRotationWorld() const; // 오일러 각 반환
    const CoreMath::Vector3 &GetPositionWorld() const;

    // --- 행렬 Getters (캐시된 데이터 접근) ---
    const CoreMath::Matrix4X4 &GetTransformLocal() const;
    const CoreMath::Matrix4X4 &GetTransformWorld() const;

    // --- 캐시 데이터 Setters (주로 SceneComponent가 사용) ---
    void SetTransformLocal(const CoreMath::Matrix4X4 &matrix) const;
    void SetTransformWorld(const CoreMath::Matrix4X4 &matrix) const;
    void SetScaleWorld(const CoreMath::Vector3 &scale) const;
    void SetQuaternionWorld(const CoreMath::Quaternion &quaternion) const;
    void SetRotationWorld(const CoreMath::Vector3 &rotation) const; // 오일러 각 설정 Degree
    void SetPositionWorld(const CoreMath::Vector3 &position) const;

    // --- 상태 관리(State Management) ---
    bool IsDirty() const;
    void ClearDirtyFlag() const;

    void Serialize(Arch &arch);

    void MarkDirty();

    CoreMath::Vector3 GetForwardWorld() const;
    CoreMath::Vector3 GetUpWorld() const;
    CoreMath::Vector3 GetRightWorld() const;

  private:
    // 로컬 프로퍼티 (원본 데이터)
    // glm::vec3 mScaleLocal;
    // glm::quat mQuaternionLocal;
    // glm::vec3 mPositionLocal;

    REFLECT_PROPERTY()
    CoreMath::Vector3 mScaleLocal;
    REFLECT_PROPERTY()
    CoreMath::Quaternion mQuaternionLocal;
    REFLECT_PROPERTY()
    CoreMath::Vector3 mPositionLocal;

    // 월드 프로퍼티 (계산 결과 캐싱)
    // mutable glm::vec3 mScaleWorld;
    // mutable glm::quat mQuaternionWorld;
    // mutable glm::vec3 mPositionWorld;

    mutable CoreMath::Vector3 mScaleWorld;
    mutable CoreMath::Quaternion mQuaternionWorld;
    mutable CoreMath::Vector3 mPositionWorld;

    // 행렬 (계산 결과 캐싱)
    // mutable glm::mat4 mTransformLocal;
    // mutable glm::mat4 mTransformWorld;

    mutable CoreMath::Matrix4X4 mTransformLocal;
    mutable CoreMath::Matrix4X4 mTransformWorld;

    // 내부 상태 변수
    mutable bool mDirtyLocal;
};

} // namespace Core