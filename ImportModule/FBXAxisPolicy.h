#pragma once

#include <CoreMath/CoreMath.h>

namespace Import
{

enum class EFBXHandedness
{
    eLeftHanded,
    eRightHanded
};

enum class EFBXUpAxis
{
    eX,
    eY,
    eZ
};

/// FBX SDK scene에서 읽은 축 정책을 engine 좌표 변환 또는 명시적 거부 결과로 바꾼 값이다.
/// SDK 객체나 scene 수명을 소유하지 않으며 importer가 traversal 전에 소비하는 일회성 정책 결과다.
struct FBXAxisConversion
{
    bool mSupported = false;
    CoreMath::Matrix4X4 mBakeMatrix = CoreMath::Matrix4X4::Identity; ///< 지원 축의 engine-space bake 변환이다.
    const char *mFailureReason = nullptr; ///< 미지원 정책의 정적 오류 문자열이며 호출자가 소유하지 않는다.
};

inline FBXAxisConversion ResolveFBXAxisConversion(EFBXHandedness handedness, EFBXUpAxis upAxis, int upSign)
{
    if (handedness != EFBXHandedness::eRightHanded)
        return {false, CoreMath::Matrix4X4::Identity, "FBX V1은 Right-Handed 축계만 지원합니다."};
    if (upSign != 1)
        return {false, CoreMath::Matrix4X4::Identity, "FBX V1은 positive up axis만 지원합니다."};

    if (upAxis == EFBXUpAxis::eY)
    {
        // source RH Y-Up (x,y,z)를 engine LH Y-Up (x,y,-z)로 바꿔 handedness만 반전한다.
        return {true,
                CoreMath::Matrix4X4({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, -1, 0}, {0, 0, 0, 1}),
                nullptr};
    }
    if (upAxis == EFBXUpAxis::eZ)
    {
        // source RH Z-Up의 +Z를 engine +Y로, source +Y를 engine +Z로 옮기면서 handedness를 반전한다.
        return {true,
                CoreMath::Matrix4X4({1, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 1}),
                nullptr};
    }

    return {false, CoreMath::Matrix4X4::Identity, "FBX V1은 Y-Up 또는 Z-Up 축계만 지원합니다."};
}

} // namespace Import
