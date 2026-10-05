#pragma once

#include <Core/CoreType.h>
#include <CoreMath/CoreMath.h>
#include <cstring>
#include <stdint.h>
#include <string>
#include <vector>

namespace Core
{

using PlayerNetID = uint64_t;

enum class EPacketType : uint8_t
{
    eAckPlayerConnect = 0,
    eNewPlayerConnect,
    ePlayerAction,
    ePlayerState
};

struct PacketHeader
{
    EPacketType mPacketType;
    PacketHeader() {};
    virtual ~PacketHeader() {};

    virtual void Serialize(std::vector<uint8_t> &oBuffer) // obuffer가 이미 사이즈가 충분하다고 가정
    {

        uint64_t index = 0;

        memcpy(oBuffer.data() + index, &mPacketType, sizeof(mPacketType));
    }

    void DeSerialize(uint8_t *buffer)
    {

        uint64_t index = 0;

        memcpy(&mPacketType, buffer + index, sizeof(mPacketType));
    }

    uint64_t GetHeaderSize() const
    {
        return sizeof(mPacketType);
    }
};

struct AcknowledgePlayerConnectPacket : public PacketHeader
{
    AcknowledgePlayerConnectPacket()
    {

        mPacketType = EPacketType::eAckPlayerConnect;
    }

    PlayerNetID mPlayerID = 0;

    void Serialize(std::vector<uint8_t> &oBuffer) override
    {

        uint64_t size = GetHeaderSize() + GetPacketDataSize();
        oBuffer.resize(size);

        PacketHeader::Serialize(oBuffer);

        uint64_t index = GetHeaderSize();
        memcpy(oBuffer.data() + index, &mPlayerID, sizeof(mPlayerID));
    }

    void DeSerialize(uint8_t *buffer)
    {

        uint64_t index = 0;

        memcpy(&mPlayerID, buffer + index, sizeof(mPlayerID));
    }
    // header제외
    uint64_t GetPacketDataSize()
    {

        return sizeof(mPlayerID);
    }
};

struct NewPlayerConnectPacket : public PacketHeader
{
    NewPlayerConnectPacket()
    {
        mPacketType = EPacketType::eNewPlayerConnect;
    };

    PlayerNetID mPlayerID = 0;
    void Serialize(std::vector<uint8_t> &oBuffer) override
    {
        uint64_t size = GetHeaderSize() + GetPacketDataSize();
        oBuffer.resize(size);

        PacketHeader::Serialize(oBuffer);

        uint64_t index = GetHeaderSize();
        memcpy(oBuffer.data() + index, &mPlayerID, sizeof(mPlayerID));
    }

    void DeSerialize(uint8_t *buffer)
    {

        uint64_t index = 0;

        memcpy(&mPlayerID, buffer + index, sizeof(mPlayerID));
    }

    // header제외
    uint64_t GetPacketDataSize()
    {

        return sizeof(mPlayerID);
    }
};

// 플레이어의 Action 이름과 값을 전달하는 패킷이며, 콜백이나 오브젝트 포인터는 소유하지 않는다.
// Action 값의 의미는 수신 측 게임 로직이 해석하고, 이 타입은 직렬화 형식만 책임진다.
struct PlayerActionPacket : public PacketHeader
{
    PlayerActionPacket()
    {
        mPacketType = EPacketType::ePlayerAction;
    };

    std::string mActionName;
    EActionValueType mActionValueType = EActionValueType::eVector2;
    // 값 타입에 관계없이 네 성분을 모두 전송한다. Bool 등에서 사용할 성분은 Action 처리 측이 결정한다.
    CoreMath::Vector4 mActionValue;

    void Serialize(std::vector<uint8_t> &oBuffer) override
    {
        oBuffer.resize(GetHeaderSize() + GetPacketDataSize());
        PacketHeader::Serialize(oBuffer);

        // 기존 패킷과 동일한 네이티브 바이트 형식이다. std::string 객체 대신 길이와 내용만 저장한다.
        // 본문 순서는 이름 길이(uint64_t), 이름 바이트(종료 문자 없음), 값 타입, X/Y/Z/W이다.
        uint64_t index = GetHeaderSize();
        uint64_t actionNameSize = mActionName.size();
        memcpy(oBuffer.data() + index, &actionNameSize, sizeof(actionNameSize));
        index += sizeof(actionNameSize);

        if (actionNameSize != 0)
            memcpy(oBuffer.data() + index, mActionName.data(), actionNameSize);
        index += actionNameSize;

        memcpy(oBuffer.data() + index, &mActionValueType, sizeof(mActionValueType));
        index += sizeof(mActionValueType);

        // Vector4의 정렬이나 구조체 padding에 의존하지 않고 float 성분만 순서대로 전송한다.
        const float actionValues[4] = {mActionValue.X, mActionValue.Y, mActionValue.Z, mActionValue.W};
        memcpy(oBuffer.data() + index, actionValues, sizeof(actionValues));
    }

    bool DeSerialize(const uint8_t *buffer, uint64_t bufferSize)
    {
        // 다른 패킷의 DeSerialize와 마찬가지로 buffer는 헤더 다음의 본문을 가리킨다.
        // 가변 길이 이름은 송신 측 길이만 믿고 읽지 않는다. 실패 시 기존 필드 값은 유지한다.
        const uint64_t fixedDataSize = sizeof(uint64_t) + sizeof(mActionValueType) + sizeof(float) * 4;
        if (buffer == nullptr || bufferSize < fixedDataSize)
            return false;

        uint64_t index = 0;
        uint64_t actionNameSize = 0;
        memcpy(&actionNameSize, buffer + index, sizeof(actionNameSize));
        index += sizeof(actionNameSize);

        // 길이 덧셈의 overflow를 피하고, 고정 필드와 이름이 본문 전체 크기에 정확히 맞는지 검사한다.
        if (actionNameSize != bufferSize - fixedDataSize)
            return false;

        mActionName.assign(reinterpret_cast<const char *>(buffer + index), actionNameSize);
        index += actionNameSize;

        memcpy(&mActionValueType, buffer + index, sizeof(mActionValueType));
        index += sizeof(mActionValueType);

        float actionValues[4];
        memcpy(actionValues, buffer + index, sizeof(actionValues));
        mActionValue = {actionValues[0], actionValues[1], actionValues[2], actionValues[3]};

        return true;
    }

    // 헤더를 제외한 본문 크기다. 이름 길이 필드와 네 개의 float 성분을 포함한다.
    uint64_t GetPacketDataSize() const
    {
        return sizeof(uint64_t) + mActionName.size() + sizeof(mActionValueType) + sizeof(float) * 4;
    }
};

struct AnimationPlaybackNetState
{
    CoreAsset::AssetID mClipID = NoneAssetID;
    float mCurrTime = 0.0f;
    bool mLoop = false;
    bool mPause = false;

    size_t GetSize() const
    {
        return sizeof(CoreAsset::AssetID) + sizeof(mCurrTime) + sizeof(mLoop) + sizeof(mPause);
    }

    void Serialize(uint8_t *buffer)
    {
        uint64_t index = 0;
        memcpy(buffer, &mClipID, sizeof(mClipID));
        index += sizeof(mClipID);
        memcpy(buffer + index, &mCurrTime, sizeof(mCurrTime));
        index += sizeof(mCurrTime);

        memcpy(buffer + index, &mLoop, sizeof(mLoop));
        index += sizeof(mLoop);
        memcpy(buffer + index, &mPause, sizeof(mPause));
    }

    void DeSerialize(uint8_t *buffer)
    {
        uint64_t index = 0;
        memcpy(&mClipID, buffer, sizeof(mClipID));
        index += sizeof(mClipID);

        memcpy(&mCurrTime, buffer + index, sizeof(mCurrTime));
        index += sizeof(mCurrTime);

        memcpy(&mLoop, buffer + index, sizeof(mLoop));
        index += sizeof(mLoop);

        memcpy(&mPause, buffer + index, sizeof(mPause));
        index += sizeof(mPause);
    }
};

// 호스트가 확정한 플레이어 캐릭터의 월드 Transform을 전달하는 스냅샷이다.
// PlayerID는 적용 대상을 식별하며, 캐릭터/컨트롤러 참조와 애니메이션 상태는 소유하지 않는다.
struct PlayerStatePacket : public PacketHeader
{
    PlayerStatePacket()
    {
        mPacketType = EPacketType::ePlayerState;
    }

    PlayerNetID mPlayerID = 0;
    CoreMath::Vector3 mPosition = {0, 0, 0};
    // GetRotationWorld()/SetRotationWorld()와 같은 Euler 각도(degree)이며 전방 벡터가 아니다.
    CoreMath::Vector3 mRotation = {0, 0, 0};
    CoreMath::Vector3 mScale = {1, 1, 1};

    bool mLightEnabled = false;

    AnimationPlaybackNetState mAnimationPlaybackState;

    void Serialize(std::vector<uint8_t> &oBuffer) override
    {
        oBuffer.resize(GetHeaderSize() + GetPacketDataSize());
        PacketHeader::Serialize(oBuffer);

        uint64_t index = GetHeaderSize();
        memcpy(oBuffer.data() + index, &mPlayerID, sizeof(mPlayerID));
        index += sizeof(mPlayerID);

        // 기존 패킷과 같은 네이티브 바이트 형식이다. 타입 padding에 의존하지 않고
        // 위치 XYZ, 회전 XYZ, 스케일 XYZ의 float 성분만 순서대로 전송한다.
        const float transformValues[9] = {mPosition.X, mPosition.Y, mPosition.Z, mRotation.X, mRotation.Y,
                                          mRotation.Z, mScale.X,    mScale.Y,    mScale.Z};
        memcpy(oBuffer.data() + index, transformValues, sizeof(transformValues));

        index += sizeof(transformValues);

        memcpy(oBuffer.data() + index, &mLightEnabled, sizeof(mLightEnabled));

        index += sizeof(mLightEnabled);
        mAnimationPlaybackState.Serialize(oBuffer.data() + index);
    }

    void DeSerialize(uint8_t *buffer)
    {
        // 고정 크기 패킷의 기존 호출 규칙을 따른다. 호출 측이 본문 크기를 확인한 뒤
        // 헤더 다음 주소를 전달하며, 이 메서드는 좌표계 변환 없이 저장된 값을 복원한다.
        uint64_t index = 0;
        memcpy(&mPlayerID, buffer + index, sizeof(mPlayerID));
        index += sizeof(mPlayerID);

        float transformValues[9];
        memcpy(transformValues, buffer + index, sizeof(transformValues));
        mPosition = {transformValues[0], transformValues[1], transformValues[2]};
        mRotation = {transformValues[3], transformValues[4], transformValues[5]};
        mScale = {transformValues[6], transformValues[7], transformValues[8]};

        index += sizeof(transformValues);

        memcpy(&mLightEnabled, buffer + index, sizeof(mLightEnabled));

        index += sizeof(mLightEnabled);

        mAnimationPlaybackState.DeSerialize(buffer + index);
    }

    // 헤더와 구조체 padding을 제외한 PlayerID + float 9개의 고정 본문 크기다.
    uint64_t GetPacketDataSize() const
    {
        return sizeof(mPlayerID) + sizeof(float) * 9 + sizeof(mLightEnabled) + mAnimationPlaybackState.GetSize();
    }
};

} // namespace Core
