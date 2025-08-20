#include "CoreMath/CoreMath.h"

namespace CoreMath
{

#ifdef D3DX

#include <DirectXMath.h>

#define CCASTVECTOR2(pVector2) reinterpret_cast<const DirectX::XMFLOAT2 *>(pVector2) // 상수 Vector2 캐스팅
#define CASTVECTOR2(pVector2) reinterpret_cast<DirectX::XMFLOAT2 *>(pVector2)        // 비상수 Vector2 캐스팅

#define CCASTVECTOR3(pVector3) reinterpret_cast<const DirectX::XMFLOAT3 *>(pVector3) // 상수 Vector3 캐스팅
#define CASTVECTOR3(pVector3) reinterpret_cast<DirectX::XMFLOAT3 *>(pVector3)        // 비상수 Vector3 캐스팅

#define CCASTVECTOR4(pVector4) reinterpret_cast<const DirectX::XMFLOAT4 *>(pVector4) // 상수 Vector4 캐스팅
#define CASTVECTOR4(pVector4) reinterpret_cast<DirectX::XMFLOAT4 *>(pVector4)        // 비상수 Vector4 캐스팅

#define CCASTMATRIX4X4(pMatrix4X4) reinterpret_cast<const DirectX::XMFLOAT4X4 *>(pMatrix4X4) // 상수 Vector4 캐스팅
#define CASTMATRIX4X4(pMatrix4X4) reinterpret_cast<DirectX::XMFLOAT4X4 *>(pMatrix4X4)        // 비상수 Vector4 캐스팅

const Vector2 Vector2::Zero = Vector2{0.0f, 0.0f};
const Vector2 Vector2::One = Vector2{1.0f, 1.0f};
const Vector2 Vector2::UintX = Vector2{1.0f, 0.0f};
const Vector2 Vector2::UintY = Vector2{0.0f, 1.0f};

const Vector3 Vector3::Zero = Vector3{0.0f, 0.0f, 0.0f};
const Vector3 Vector3::One = Vector3{1.0f, 1.0f, 1.0f};
const Vector3 Vector3::UintX = Vector3{1.0f, 0.0f, 0.0f};
const Vector3 Vector3::UintY = Vector3{0.0f, 1.0f, 0.0f};
const Vector3 Vector3::UintZ = Vector3{0.0f, 0.0f, 1.0f};

const Vector4 Vector4::Zero = Vector4{0.0f, 0.0f, 0.0f, 0.0f};
const Vector4 Vector4::One = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
const Vector4 Vector4::UintX = Vector4{1.0f, 0.0f, 0.0f, 0.0f};
const Vector4 Vector4::UintY = Vector4{0.0f, 1.0f, 0.0f, 0.0f};
const Vector4 Vector4::UintZ = Vector4{0.0f, 0.0f, 1.0f, 0.0f};
const Vector4 Vector4::UintW = Vector4{0.0f, 0.0f, 0.0f, 1.0f};

const Matrix4X4 Matrix4X4::Zero =
    Matrix4X4{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
const Matrix4X4 Matrix4X4::Identity = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

// Vector2::operator!=
bool Vector2::operator!=(const Vector2 &vector) const
{
    return !this->operator==(vector);
}

// Vector2::operator+
Vector2 Vector2::operator+(const Vector2 &rhs) const
{
    DirectX::XMFLOAT2 vf2;
    DirectX::XMStoreFloat2(&vf2, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(CCASTVECTOR2(this)),
                                                      DirectX::XMLoadFloat2(CCASTVECTOR2(&rhs))));
    return Vector2(vf2.x, vf2.y);
}

// Vector2::operator-
Vector2 Vector2::operator-(const Vector2 &rhs) const
{
    DirectX::XMFLOAT2 vf2;
    DirectX::XMStoreFloat2(&vf2, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(CCASTVECTOR2(this)),
                                                           DirectX::XMLoadFloat2(CCASTVECTOR2(&rhs))));
    return Vector2(vf2.x, vf2.y);
}

// Vector2::operator* (스칼라 곱셈)
Vector2 Vector2::operator*(float scalar) const
{
    DirectX::XMFLOAT2 vf2;
    DirectX::XMStoreFloat2(&vf2, DirectX::XMVectorScale(DirectX::XMLoadFloat2(CCASTVECTOR2(this)), scalar));
    return Vector2(vf2.x, vf2.y);
}

// Vector2::operator/ (스칼라 나눗셈)
Vector2 Vector2::operator/(float scalar) const
{
    if (scalar == 0.0f)
    {
        return Vector2(0.0f, 0.0f); // 0벡터 반환
    }
    return operator*(1.0f / scalar);
}

// Vector2::operator+=
Vector2 &Vector2::operator+=(const Vector2 &rhs)
{
    DirectX::XMStoreFloat2(CASTVECTOR2(this), DirectX::XMVectorAdd(DirectX::XMLoadFloat2(CCASTVECTOR2(this)),
                                                                   DirectX::XMLoadFloat2(CCASTVECTOR2(&rhs))));
    return *this;
}

// Vector2::operator-=
Vector2 &Vector2::operator-=(const Vector2 &rhs)
{
    DirectX::XMStoreFloat2(CASTVECTOR2(this), DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(CCASTVECTOR2(this)),
                                                                        DirectX::XMLoadFloat2(CCASTVECTOR2(&rhs))));
    return *this;
}

// Vector2::operator*= (스칼라 곱셈 할당)
Vector2 &Vector2::operator*=(float scalar)
{
    DirectX::XMStoreFloat2(CASTVECTOR2(this),
                           DirectX::XMVectorScale(DirectX::XMLoadFloat2(CCASTVECTOR2(this)), scalar));
    return *this;
}

// Vector2::operator/= (스칼라 나눗셈 할당)
Vector2 &Vector2::operator/=(float scalar)
{
    if (scalar == 0.0f)
    {
        X = 0.0f;
        Y = 0.0f;
        return *this;
    }
    return this->operator*=(1.0f / scalar);
}

// Vector2::Length()
float Vector2::Length() const
{
    return DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMLoadFloat2(CCASTVECTOR2(this))));
}

// Vector2::LengthSquared()
float Vector2::LengthSquared() const
{
    return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(DirectX::XMLoadFloat2(CCASTVECTOR2(this))));
}

// Vector2::Normalize() (void 버전)
void Vector2::Normalize()
{
    DirectX::XMStoreFloat2(CASTVECTOR2(this), DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(CCASTVECTOR2(this))));
}

// Vector2::Normalize() (const 버전)
Vector2 Vector2::Normalize() const
{
    Vector2 temp = *this;
    temp.Normalize();
    return temp;
}

// Vector2::Dot()
float Vector2::Dot(const Vector2 &vector) const
{
    return DirectX::XMVectorGetX(
        DirectX::XMVector2Dot(DirectX::XMLoadFloat2(CCASTVECTOR2(this)), DirectX::XMLoadFloat2(CCASTVECTOR2(&vector))));
}

// Vector2::Equal() (Vector4와 동일하게 오버로드된 operator==가 부동소수점 비교를 한다고 가정)
bool Vector2::Equal(const Vector2 &vector)
{
    return operator==(vector);
}

// Vector2::NearEqual() (부동소수점 오차 허용 비교)
bool Vector2::NearEqual(const Vector2 &vector, float epsilon)
{
    DirectX::XMVECTOR vector1 = DirectX::XMLoadFloat2(CCASTVECTOR2(this));
    DirectX::XMVECTOR vector2 = DirectX::XMLoadFloat2(CCASTVECTOR2(&vector));
    DirectX::XMVECTOR epsilonVec = DirectX::XMVectorReplicate(epsilon);

    return DirectX::XMVector2NearEqual(vector1, vector2, epsilonVec);
}

// Vector2::operator== (부동소수점 비교 고려)
bool Vector2::operator==(const Vector2 &vector) const
{
    return DirectX::XMVector2Equal(DirectX::XMLoadFloat2(CCASTVECTOR2(this)),
                                   DirectX::XMLoadFloat2(CCASTVECTOR2(&vector)));
}

// Vector3::operator!=
bool Vector3::operator!=(const Vector3 &vector) const
{
    return !this->operator==(vector);
}

// Vector3::operator+
Vector3 Vector3::operator+(const Vector3 &rhs) const
{
    DirectX::XMFLOAT3 vf3;
    DirectX::XMStoreFloat3(&vf3, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(CCASTVECTOR3(this)),
                                                      DirectX::XMLoadFloat3(CCASTVECTOR3(&rhs))));
    return Vector3(vf3.x, vf3.y, vf3.z);
}

// Vector3::operator-
Vector3 Vector3::operator-(const Vector3 &rhs) const
{
    DirectX::XMFLOAT3 vf3;
    DirectX::XMStoreFloat3(&vf3, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(CCASTVECTOR3(this)),
                                                           DirectX::XMLoadFloat3(CCASTVECTOR3(&rhs))));
    return Vector3(vf3.x, vf3.y, vf3.z);
}

// Vector3::operator* (스칼라 곱셈)
Vector3 Vector3::operator*(float scalar) const
{
    DirectX::XMFLOAT3 vf3;
    DirectX::XMStoreFloat3(&vf3, DirectX::XMVectorScale(DirectX::XMLoadFloat3(CCASTVECTOR3(this)), scalar));
    return Vector3(vf3.x, vf3.y, vf3.z);
}

// Vector3::operator/ (스칼라 나눗셈)
Vector3 Vector3::operator/(float scalar) const
{
    if (scalar == 0.0f)
    {
        // 0으로 나누는 경우 처리: 0벡터 반환
        return Vector3(0.0f, 0.0f, 0.0f);
    }
    return operator*(1.0f / scalar);
}

// Vector3::operator+=
Vector3 &Vector3::operator+=(const Vector3 &rhs)
{
    DirectX::XMStoreFloat3(CASTVECTOR3(this), DirectX::XMVectorAdd(DirectX::XMLoadFloat3(CCASTVECTOR3(this)),
                                                                   DirectX::XMLoadFloat3(CCASTVECTOR3(&rhs))));
    return *this;
}

// Vector3::operator-=
Vector3 &Vector3::operator-=(const Vector3 &rhs)
{
    DirectX::XMStoreFloat3(CASTVECTOR3(this), DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(CCASTVECTOR3(this)),
                                                                        DirectX::XMLoadFloat3(CCASTVECTOR3(&rhs))));
    return *this;
}

// Vector3::operator*= (스칼라 곱셈 할당)
Vector3 &Vector3::operator*=(float scalar)
{
    DirectX::XMStoreFloat3(CASTVECTOR3(this),
                           DirectX::XMVectorScale(DirectX::XMLoadFloat3(CCASTVECTOR3(this)), scalar));
    return *this;
}

// Vector3::operator/= (스칼라 나눗셈 할당)
Vector3 &Vector3::operator/=(float scalar)
{
    if (scalar == 0.0f)
    {
        // 0으로 나누는 경우 처리: 0으로 설정
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
        return *this;
    }
    return this->operator*=(1.0f / scalar);
}

// Vector3::Length()
float Vector3::Length() const
{
    return DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(CCASTVECTOR3(this))));
}

// Vector3::LengthSquared()
float Vector3::LengthSquared() const
{
    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(CCASTVECTOR3(this))));
}

// Vector3::Normalize() (void 버전)
void Vector3::Normalize()
{
    DirectX::XMStoreFloat3(CASTVECTOR3(this), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(CCASTVECTOR3(this))));
}

// Vector3::Normalize() (const 버전)
Vector3 Vector3::Normalize() const
{
    Vector3 temp = *this; // 현재 객체의 복사본
    temp.Normalize();     // 복사본 정규화 (void Normalize() 호출)
    return temp;          // 정규화된 복사본 반환
}

// Vector3::Dot()
float Vector3::Dot(const Vector3 &vector) const
{
    return DirectX::XMVectorGetX(
        DirectX::XMVector3Dot(DirectX::XMLoadFloat3(CCASTVECTOR3(this)), DirectX::XMLoadFloat3(CCASTVECTOR3(&vector))));
}

bool Vector3::Equal(const Vector3 &vector)
{
    return this->operator==(vector);
}

bool Vector3::NearEqual(const Vector3 &vector, float epsilon)
{
    DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(CCASTVECTOR3(this));
    DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(CCASTVECTOR3(&vector));

    return DirectX::XMVector3NearEqual(v1, v2, DirectX::XMVectorReplicate(epsilon));
}

// Vector3::Cross() (외적) - Vector3에만 있는 특수 연산
Vector3 Vector3::Cross(const Vector3 &rhs) const
{
    DirectX::XMFLOAT3 vf3;
    DirectX::XMStoreFloat3(&vf3, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(CCASTVECTOR3(this)),
                                                         DirectX::XMLoadFloat3(CCASTVECTOR3(&rhs))));
    return Vector3(vf3.x, vf3.y, vf3.z);
}

// Vector3::operator== (부동소수점 비교 고려)
bool Vector3::operator==(const Vector3 &vector) const
{
    DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(CCASTVECTOR3(this));
    DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(CCASTVECTOR3(&vector));

    return DirectX::XMVector3Equal(v1, v2);
}

bool Vector4::operator!=(const Vector4 &vector) const
{
    return !this->operator==(vector);
}

Vector4 Vector4::operator+(const Vector4 &rhs) const
{
    DirectX::XMFLOAT4 vf4;

    DirectX::XMStoreFloat4(&vf4, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(CCASTVECTOR4(this)),
                                                      DirectX::XMLoadFloat4(CCASTVECTOR4(&rhs))));

    return {vf4.x, vf4.y, vf4.z, vf4.w};
}

Vector4 Vector4::operator-(const Vector4 &rhs) const
{

    DirectX::XMFLOAT4 vf4;

    DirectX::XMStoreFloat4(&vf4, DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(CCASTVECTOR4(this)),
                                                           DirectX::XMLoadFloat4(CCASTVECTOR4(&rhs))));

    return {vf4.x, vf4.y, vf4.z, vf4.w};
}

Vector4 Vector4::operator*(float scalar) const
{
    DirectX::XMFLOAT4 vf4;

    DirectX::XMStoreFloat4(&vf4, DirectX::XMVectorScale(DirectX::XMLoadFloat4(CCASTVECTOR4(this)), scalar));

    return {vf4.x, vf4.y, vf4.z, vf4.w};
}

Vector4 Vector4::operator/(float scalar) const
{
    if (scalar == 0.0f)
    {
        // 0으로 나누는 경우 처리: 0벡터 반환
        return Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    return operator*(1.0f / scalar);
}

Vector4 &Vector4::operator+=(const Vector4 &rhs)
{
    DirectX::XMStoreFloat4(CASTVECTOR4(this), DirectX::XMVectorAdd(DirectX::XMLoadFloat4(CCASTVECTOR4(this)),
                                                                   DirectX::XMLoadFloat4(CCASTVECTOR4(&rhs))));

    return *this;
}

Vector4 &Vector4::operator-=(const Vector4 &rhs)
{
    DirectX::XMStoreFloat4(CASTVECTOR4(this), DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(CCASTVECTOR4(this)),
                                                                        DirectX::XMLoadFloat4(CCASTVECTOR4(&rhs))));

    return *this;
}

Vector4 &Vector4::operator*=(float scalar)
{
    DirectX::XMStoreFloat4(CASTVECTOR4(this),
                           DirectX::XMVectorScale(DirectX::XMLoadFloat4(CCASTVECTOR4(this)), scalar));

    return *this;
}

Vector4 &Vector4::operator/=(float scalar)
{
    if (scalar == 0.0f)
    {
        // 0으로 나누는 경우 처리: 0으로 설정
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
        W = 0.0f;
        return *this;
    }
    return this->operator*=(1.0f / scalar);
}

float Vector4::Length() const
{
    return DirectX::XMVectorGetX(DirectX::XMVector4Length(DirectX::XMLoadFloat4(CCASTVECTOR4(this))));
}

float Vector4::LengthSquared() const
{
    return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(DirectX::XMLoadFloat4(CCASTVECTOR4(this))));
}

void Vector4::Normalize()
{
    DirectX::XMStoreFloat4(CASTVECTOR4(this), DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(CCASTVECTOR4(this))));
}

Vector4 Vector4::Normalize() const
{
    Vector4 vector4 = *this;
    vector4.Normalize();
    return vector4;
}

float Vector4::Dot(const Vector4 &vector) const
{
    return DirectX::XMVectorGetX(
        DirectX::XMVector4Dot(DirectX::XMLoadFloat4(CCASTVECTOR4(this)), DirectX::XMLoadFloat4(CCASTVECTOR4(&vector))));
}

bool Vector4::Equal(const Vector4 &vector)
{
    return operator==(vector);
}

bool Vector4::NearEqual(const Vector4 &vector, float epsilon)
{
    DirectX::XMVECTOR vector1 = DirectX::XMLoadFloat4(CCASTVECTOR4(this));
    DirectX::XMVECTOR vector2 = DirectX::XMLoadFloat4(CCASTVECTOR4(&vector));

    return DirectX::XMVector4NearEqual(vector1, vector2, DirectX::XMVectorReplicate(epsilon));
}

bool Vector4::operator==(const Vector4 &vector) const
{
    return DirectX::XMVector4Equal(DirectX::XMLoadFloat4(CCASTVECTOR4(this)),
                                   DirectX::XMLoadFloat4(CCASTVECTOR4(&vector)));
}

Quaternion Quaternion::operator*(const Quaternion &rhs) const
{

    Quaternion quaternion;
    DirectX::XMStoreFloat4(CASTVECTOR4(&quaternion),
                           DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(CCASTVECTOR4(this)),
                                                         DirectX::XMLoadFloat4(CCASTVECTOR4(&rhs))));

    return quaternion;
}

Matrix4X4::Matrix4X4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20,
                     float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{

    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;

    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;

    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;

    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}

Matrix4X4 Matrix4X4::operator+(const Matrix4X4 &rhs) const
{

    DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));
    DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(&rhs));

    Matrix4X4 result;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&result), m1 + m2);

    return result;
}

Matrix4X4 Matrix4X4::operator-(const Matrix4X4 &rhs) const
{
    DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));
    DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(&rhs));

    Matrix4X4 result;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&result), m1 - m2);

    return result;
}

Matrix4X4 Matrix4X4::operator*(const Matrix4X4 &rhs) const
{
    DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));
    DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(&rhs));

    Matrix4X4 result;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&result), m1 * m2);

    return result;
}

Matrix4X4 Matrix4X4::operator*(float scalar) const
{
    DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));

    DirectX::XMVECTOR r1 = DirectX::XMVectorScale(m1.r[0], scalar);
    DirectX::XMVECTOR r2 = DirectX::XMVectorScale(m1.r[1], scalar);
    DirectX::XMVECTOR r3 = DirectX::XMVectorScale(m1.r[2], scalar);
    DirectX::XMVECTOR r4 = DirectX::XMVectorScale(m1.r[3], scalar);

    DirectX::XMMATRIX m2(r1, r2, r3, r4);

    Matrix4X4 result;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&result), m2);

    return result;
}

Matrix4X4 Matrix4X4::operator/(float scalar) const
{
    if (scalar != 0.0f)
        return this->operator*(1.0f / scalar);
    else
        return this->operator*(0.0f);
}

Matrix4X4 &Matrix4X4::operator+=(const Matrix4X4 &rhs)
{

    DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));
    DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(&rhs));

    m1 += m2;

    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(this), m1);

    return *this;
}

Matrix4X4 &Matrix4X4::operator-=(const Matrix4X4 &rhs)
{

    DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));
    DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(&rhs));

    m1 -= m2;

    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(this), m1);

    return *this;

    // TODO: 여기에 return 문을 삽입합니다.
}

Matrix4X4 &Matrix4X4::operator*=(const Matrix4X4 &rhs)
{
    DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));
    DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(&rhs));

    m1 *= m2;

    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(this), m1);

    return *this;
    // TODO: 여기에 return 문을 삽입합니다.
}

Matrix4X4 &Matrix4X4::operator*=(float scalar)
{

    DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));

    DirectX::XMVECTOR r1 = DirectX::XMVectorScale(m1.r[0], scalar);
    DirectX::XMVECTOR r2 = DirectX::XMVectorScale(m1.r[1], scalar);
    DirectX::XMVECTOR r3 = DirectX::XMVectorScale(m1.r[2], scalar);
    DirectX::XMVECTOR r4 = DirectX::XMVectorScale(m1.r[3], scalar);

    DirectX::XMMATRIX m2(r1, r2, r3, r4);

    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(this), m2);

    return *this;
}

Matrix4X4 &Matrix4X4::operator/=(float scalar)
{

    if (scalar != 0.0f)
        this->operator*=(1.0f / scalar);
    else
        this->operator*=(0.0f);

    return *this;
}

Vector4 Matrix4X4::operator*(const Vector4 &rhs) const
{

    DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));

    DirectX::XMVECTOR vector = DirectX::XMLoadFloat4(CCASTVECTOR4(&rhs));

    Vector4 v4;
    DirectX::XMStoreFloat4(CASTVECTOR4(&v4), DirectX::XMVector4Transform(vector, matrix));

    return v4;
}

Vector3 Matrix4X4::operator*(const Vector3 &rhs) const
{
    return TransformDirection(rhs);
}

Vector3 Matrix4X4::TransformDirection(const Vector3 &rhs) const
{
    DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));

    DirectX::XMVECTOR vector = DirectX::XMLoadFloat3(CCASTVECTOR3(&rhs));

    Vector3 v3;
    DirectX::XMStoreFloat3(CASTVECTOR3(&v3), DirectX::XMVector3Transform(vector, matrix));

    return v3;
}

Vector3 Matrix4X4::TransformPoint(const Vector3 &rhs) const
{

    DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this));

    DirectX::XMVECTOR vector = DirectX::XMLoadFloat3(CCASTVECTOR3(&rhs));

    Vector3 v3;
    DirectX::XMStoreFloat3(CASTVECTOR3(&v3), DirectX::XMVector3TransformCoord(vector, matrix));

    return v3;
}

Matrix4X4 Matrix4X4::Transpose() const
{

    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix),
                             DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this))));

    return matrix;
}

Matrix4X4 Matrix4X4::Inverse() const
{
    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix),
                             DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this))));

    return matrix;
}

float Matrix4X4::Determinant() const
{
    DirectX::XMVECTOR determinant;
    DirectX::XMMatrixInverse(&determinant, DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this)));

    return DirectX::XMVectorGetX(determinant);
}

void Matrix4X4::Transpose()
{

    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(this),
                             DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(this))));
}

Vector4 Matrix4X4::GetRow(int rowIndex) const
{
    return Vector4(m[rowIndex][0], m[rowIndex][1], m[rowIndex][2], m[rowIndex][3]);
}

Vector4 Matrix4X4::GetColumn(int columnIndex) const
{
    return Vector4(m[0][columnIndex], m[1][columnIndex], m[2][columnIndex], m[3][columnIndex]);
}

Matrix4X4 Matrix4X4::MakeTranslation(Vector3 position)
{

    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix),
                             DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(CCASTVECTOR3(&position))));

    return matrix;
}

Matrix4X4 Matrix4X4::MakeTranslation(float x, float y, float z)
{
    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix), DirectX::XMMatrixTranslation(x, y, z));

    return matrix;
}

Matrix4X4 Matrix4X4::MakeScale(Vector3 scale)
{
    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix),
                             DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(CCASTVECTOR3(&scale))));

    return matrix;
}

Matrix4X4 Matrix4X4::MakeScale(float x, float y, float z)
{
    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix), DirectX::XMMatrixScaling(x, y, z));

    return matrix;
}

Matrix4X4 Matrix4X4::MakeRotationRadX(float angleRad)
{
    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix), DirectX::XMMatrixRotationX(angleRad));

    return matrix;
}

Matrix4X4 Matrix4X4::MakeRotationRadY(float angleRad)
{
    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix), DirectX::XMMatrixRotationY(angleRad));

    return matrix;
}

Matrix4X4 Matrix4X4::MakeRotationRadZ(float angleRad)
{
    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix), DirectX::XMMatrixRotationZ(angleRad));

    return matrix;
}

Matrix4X4 Matrix4X4::MakeRotationDegreeX(float angle)
{
    return MakeRotationRadX(angle * DirectX::XM_PI / 180.0f);
}

Matrix4X4 Matrix4X4::MakeRotationDegreeY(float angle)
{
    return MakeRotationRadY(angle * DirectX::XM_PI / 180.0f);
}

Matrix4X4 Matrix4X4::MakeRotationDegreeZ(float angle)
{
    return MakeRotationRadZ(angle * DirectX::XM_PI / 180.0f);
}

Matrix4X4 Matrix4X4::MakeRotationYawPitchRollRad(float x, float y, float z)
{
    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix), DirectX::XMMatrixRotationRollPitchYaw(x, y, z));

    return matrix;
}

Matrix4X4 Matrix4X4::MakeTransform(Vector3 position, Quaternion rotation, Vector3 scale)
{

    Matrix4X4 matrix;

    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix),
                             DirectX::XMMatrixAffineTransformation(DirectX::XMLoadFloat3(CCASTVECTOR3(&scale)),
                                                                   DirectX::XMVectorSet(0, 0, 0, 1.0f),
                                                                   DirectX::XMLoadFloat4(CCASTVECTOR4(&rotation)),
                                                                   DirectX::XMLoadFloat3(CCASTVECTOR3(&position))));

    return matrix;
}

void Matrix4X4::MatrixDecompose(Vector3 &oPosition, Quaternion &oQuaternion, Vector3 &oScale, const Matrix4X4 &matrix)
{

    DirectX::XMVECTOR positionVector;
    DirectX::XMVECTOR quaternion;
    DirectX::XMVECTOR scalingVector;
    DirectX::XMMatrixDecompose(&scalingVector, &quaternion, &positionVector,
                               DirectX::XMLoadFloat4x4(CCASTMATRIX4X4(&matrix)));

    DirectX::XMStoreFloat3(CASTVECTOR3(&oPosition), positionVector);
    DirectX::XMStoreFloat4(CASTVECTOR4(&oQuaternion), quaternion);
    DirectX::XMStoreFloat3(CASTVECTOR3(&oScale), scalingVector);
}

Matrix4X4 Matrix4X4::MakeLookAtLH(Vector3 eye, Vector3 target, Vector3 up)
{

    return Matrix4X4();
}

Matrix4X4 Matrix4X4::MakePerspectiveFovLH(float fovAngleY, float aspectRatio, float nearZ, float farZ)
{

    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(
        CASTMATRIX4X4(&matrix),
        DirectX::XMMatrixPerspectiveFovLH(fovAngleY / 180.0f * DirectX::XM_PI, aspectRatio, nearZ, farZ));

    return matrix;
}

Matrix4X4 Matrix4X4::MakeOrthographicLH(float width, float height, float nearZ, float farZ)
{
    Matrix4X4 matrix;
    DirectX::XMStoreFloat4x4(CASTMATRIX4X4(&matrix), DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ));

    return matrix;
}

#elif OPGL

#endif

} // namespace CoreMath