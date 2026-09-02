#pragma once
#include <CoreMath/CoreMathDllMacro.h>
#include <ReflectSystem/ReflectionMacro.h>
#include <cmath>
#include <stdint.h>

class Arch;

namespace CoreMath
{
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.28318530717958647692f;
constexpr float HALF_PI = 1.57079632679489661923f;

struct Vector2;
struct Vector3;
struct Vector4;

struct COREMATH_API REFLECT_STRUCT(EngineClass) Vector2
{
  public:
    Vector2() = default;
    ~Vector2() = default;

    Vector2(const Vector2 &rhs) = default;

    Vector2(float x, float y) : X(x), Y(y) {}

    Vector2 &operator=(const Vector2 &rhs) = default;

    float &operator[](uint32_t index)
    {

        if (index >= 2)
            abort();

        float *pMem = (float *)(&(this->X));
        return pMem[index];
    }

    bool operator==(const Vector2 &vector) const
    {
        if ((X == vector.X) && (Y == vector.Y))
            return true;

        return false;
    }
    bool operator!=(const Vector2 &vector) const
    {

        return !this->operator==(vector);
    }

    Vector2 operator+(const Vector2 &rhs) const
    {
        return Vector2{X + rhs.X, Y + rhs.Y};
    }
    Vector2 operator-(const Vector2 &rhs) const
    {
        return Vector2{X - rhs.X, Y - rhs.Y};
    }
    Vector2 operator*(float scalar) const
    {
        return Vector2{X * scalar, Y * scalar};
    }
    Vector2 operator/(float scalar) const
    {
        return Vector2{X / scalar, Y / scalar};
    }

    Vector2 &operator+=(const Vector2 &rhs)
    {
        X += rhs.X;
        Y += rhs.Y;
        return *this;
    }
    Vector2 &operator-=(const Vector2 &rhs)
    {
        X -= rhs.X;
        Y -= rhs.Y;
        return *this;
    }
    Vector2 &operator*=(float scalar)
    {
        X *= scalar;
        Y *= scalar;
        return *this;
    }
    Vector2 &operator/=(float scalar)
    {
        X /= scalar;
        Y /= scalar;
        return *this;
    }

    void Set(float x, float y, float z)
    {
        X = x;
        Y = y;
    }

    float Length() const
    {

        return std::sqrt(LengthSquared());
    }
    float LengthSquared() const
    {
        return X * X + Y * Y;
    }
    void Normalize()
    {
        float lenSq = LengthSquared();

        if (lenSq > 0.0000001f)
        {
            float invLen = 1.0f / std::sqrt(lenSq);
            (*this) *= invLen;
        }
    }
    Vector2 GetNormalize() const
    {

        Vector2 ret(*this);
        ret.Normalize();
        return ret;
    }

    float Dot(const Vector2 &vector) const
    {
        return (X * vector.X) + (Y + vector.Y);
    }

    bool Equal(const Vector2 &vector)
    {
        if (X == vector.X && Y == vector.Y)
            return true;

        return false;
    }
    bool NearEqual(const Vector2 &vector, float epsilon = 0.00001f)
    {
        return std::abs(X - vector.X) <= epsilon && std::abs(Y - vector.Y) <= epsilon;
    }

    REFLECT_PROPERTY()
    float X = 0.0f;
    REFLECT_PROPERTY()
    float Y = 0.0f;

    static const Vector2 Zero;
    static const Vector2 One;
    static const Vector2 UnitX;
    static const Vector2 UnitY;

  private:
};

struct COREMATH_API REFLECT_STRUCT(EngineClass) Vector3
{
  public:
    Vector3() = default;
    ~Vector3() = default;

    Vector3(const Vector3 &rhs) = default;

    Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}
    explicit Vector3(const Vector4 &vec4);

    Vector3 &operator=(const Vector3 &rhs) = default;

    float &operator[](uint32_t index)
    {

        if (index >= 3)
            abort();

        float *pMem = (float *)(&(this->X));
        return pMem[index];
    }

    float operator[](uint32_t index) const
    {
        if (index >= 3)
            abort();

        float *pMem = (float *)(&(this->X));

        return pMem[index];
    }

    bool operator==(const Vector3 &vector) const
    {

        if ((X == vector.X) && (Y == vector.Y) && (Z == vector.Z))
            return true;

        return false;
    }
    bool operator!=(const Vector3 &vector) const
    {
        return !(this->operator==(vector));
    }

    Vector3 operator+(const Vector3 &rhs) const
    {
        return Vector3{X + rhs.X, Y + rhs.Y, Z + rhs.Z};
    }
    Vector3 operator-(const Vector3 &rhs) const
    {
        return Vector3{X - rhs.X, Y - rhs.Y, Z - rhs.Z};
    }
    Vector3 operator*(const Vector3 &rhs) const
    {
        return Vector3{X * rhs.X, Y * rhs.Y, Z * rhs.Z};
    }

    Vector3 operator*(float scalar) const
    {
        return Vector3{X * scalar, Y * scalar, Z * scalar};
    }
    Vector3 operator/(float scalar) const
    {
        return Vector3{X / scalar, Y / scalar, Z / scalar};
    }

    Vector3 operator/(const Vector3 &rhs) const
    {
        return Vector3{X / rhs.X, Y / rhs.Y, Z / rhs.Z};
    }

    Vector3 &operator+=(const Vector3 &rhs)
    {
        X += rhs.X;
        Y += rhs.Y;
        Z += rhs.Z;
        return *this;
    }
    Vector3 &operator-=(const Vector3 &rhs)
    {

        X -= rhs.X;
        Y -= rhs.Y;
        Z -= rhs.Z;
        return *this;
    }

    Vector3 &operator*=(float scalar)
    {

        X *= scalar;
        Y *= scalar;
        Z *= scalar;
        return *this;
    }

    Vector3 &operator/=(float scalar)
    {

        X /= scalar;
        Y /= scalar;
        Z /= scalar;
        return *this;
    }

    void Set(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    float Length() const
    {
        return std::sqrt(LengthSquared());
    }
    float LengthSquared() const
    {
        return X * X + Y * Y + Z * Z;
    }
    void Normalize()
    {
        float lenSq = LengthSquared();

        if (lenSq > 0.00001f)
        {
            float invLen = 1.0f / std::sqrt(lenSq);
            X *= invLen;
            Y *= invLen;
            Z *= invLen;
        }
    }
    Vector3 GetNormalize() const
    {
        Vector3 ret(*this);
        ret.Normalize();
        return ret;
    }
    float Dot(const Vector3 &vector) const
    {
        return X * vector.X + Y * vector.Y + Z * vector.Z;
    }

    Vector3 Cross(const Vector3 &vector) const
    {
        return Vector3{(Y * vector.Z - Z * vector.Y), -(X * vector.Z - Z * vector.X), (X * vector.Y - Y * vector.X)};
    }

    bool Equal(const Vector3 &vector) const
    {
        return (X == vector.X) && (Y == vector.Y) && (Z == vector.Z);
    }
    bool NearEqual(const Vector3 &vector, float epsilon = 0.00001f) const
    {
        return std::abs(X - vector.X) <= epsilon && std::abs(Y - vector.Y) <= epsilon &&
               std::abs(Z - vector.Z) <= epsilon;
    }

    void Clear()
    {
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
    }

    REFLECT_PROPERTY()
    float X = 0.0f;
    REFLECT_PROPERTY()
    float Y = 0.0f;
    REFLECT_PROPERTY()
    float Z = 0.0f;

    const static Vector3 Zero;
    const static Vector3 One;
    const static Vector3 UintX;
    const static Vector3 UintY;
    const static Vector3 UintZ;

  private:
};

inline float Dot(const Vector3 &vec1, const Vector3 &vec2)
{
    return vec1.X * vec2.X + vec1.Y * vec2.Y + vec1.Z * vec2.Z;
}

inline CoreMath::Vector3 operator*(float scalar, const Vector3 &vec)
{
    return vec * scalar;
}

struct alignas(16) COREMATH_API REFLECT_STRUCT(EngineClass) Vector4
{
  public:
    Vector4() = default;
    ~Vector4() = default;

    Vector4(const Vector4 &rhs) = default;

    Vector4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}
    Vector4(const Vector3 &rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z), W(1.0f) {}
    Vector4(const Vector3 &rhs, float w) : X(rhs.X), Y(rhs.Y), Z(rhs.Z), W(w) {}

    Vector3 XYZ() const
    {
        return {X, Y, Z};
    }

    Vector4 &operator=(const Vector4 &rhs) = default;

    bool operator==(const Vector4 &vector) const;
    bool operator!=(const Vector4 &vector) const;

    float &operator[](uint32_t index)
    {

        if (index >= 4)
            abort();

        float *pMem = (float *)(&(this->X));
        return pMem[index];
    }

    const float &operator[](uint32_t index) const
    {
        return const_cast<Vector4 &>(*this).operator[](index);
    }

    Vector4 operator+(const Vector4 &rhs) const
    {
        return Vector4{X + rhs.X, Y + rhs.Y, Z + rhs.Z, W + rhs.W};
    }
    Vector4 operator-(const Vector4 &rhs) const
    {
        return Vector4{X - rhs.X, Y - rhs.Y, Z - rhs.Z, W - rhs.W};
    }
    Vector4 operator*(float scalar) const
    {
        return Vector4{X * scalar, Y * scalar, Z * scalar, W * scalar};
    }
    Vector4 operator/(float scalar) const
    {
        return Vector4{X / scalar, Y * scalar, Z * scalar, W * scalar};
    }

    Vector4 &operator+=(const Vector4 &rhs)
    {
        X += rhs.X;
        Y += rhs.Y;
        Z += rhs.Z;
        W += rhs.W;
        return *this;
    }
    Vector4 &operator-=(const Vector4 &rhs)
    {
        X -= rhs.X;
        Y -= rhs.Y;
        Z -= rhs.Z;
        W -= rhs.W;

        return *this;
    }
    Vector4 &operator*=(float scalar)
    {
        X *= scalar;
        Y *= scalar;
        Z *= scalar;
        W *= scalar;

        return *this;
    }
    Vector4 &operator/=(float scalar)
    {
        X /= scalar;
        Y /= scalar;
        Z /= scalar;
        W /= scalar;

        return *this;
    }

    void Set(float x, float y, float z, float w)
    {
        X = x;
        Y = y;
        Z = z;
        W = w;
    }

    float Length() const
    {
        return std::sqrt(LengthSquared());
    }
    float LengthSquared() const
    {
        return X * X + Y * Y + Z * Z + W * W;
    }
    void Normalize()
    {
        float lenSq = LengthSquared();

        if (lenSq > 0.000001f)
        {
            float invLen = 1.0f / std::sqrt(lenSq);
            X *= invLen;
            Y *= invLen;
            Z *= invLen;
            W *= invLen;
        }
    }
    Vector4 GetNormalize() const
    {

        Vector4 ret(*this);
        ret.Normalize();
        return ret;
    }
    float Dot(const Vector4 &vector) const
    {
        return (X * vector.X) + (Y * vector.Y) + (Z * vector.Z) + (W * vector.W);
    }
    bool Equal(const Vector4 &vector) const
    {

        return (X == vector.X) && (Z == vector.Z) && (Y == vector.Y) && (W == vector.W);
    }

    bool NearEqual(const Vector4 &vector, float epsilon = 0.00001f) const
    {

        return std::abs(X - vector.X) <= epsilon && std::abs(Y - vector.Y) <= epsilon &&
               std::abs(Z - vector.Z) <= epsilon && std::abs(W - vector.W) <= epsilon;
    }

    REFLECT_PROPERTY()
    float X = 0.0f;
    REFLECT_PROPERTY()
    float Y = 0.0f;
    REFLECT_PROPERTY()
    float Z = 0.0f;
    REFLECT_PROPERTY()
    float W = 0.0f;

    const static Vector4 Zero;
    const static Vector4 One;
    const static Vector4 UnitX;
    const static Vector4 UnitY;
    const static Vector4 UnitZ;
    const static Vector4 UnitW;

  private:
};

struct COREMATH_API REFLECT_STRUCT(EngineClass) Quaternion
{
  public:
    Quaternion() = default;
    ~Quaternion() = default;

    Quaternion(const Quaternion &rhs) = default;

    Quaternion(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

    Quaternion(const Vector3 &rhs);

    Quaternion &operator=(const Quaternion &rhs) = default;

    Quaternion operator*(const Quaternion &rhs) const;

    Quaternion operator*(float value) const;

    Quaternion &operator+=(const Quaternion &rhs);

    Vector3 ToEulerAngles() const;
    void Normalize()
    {
        float lenSq = X * X + Y * Y + Z * Z + W * W;
        if (lenSq > 0.000001f)
        {

            float invLen = 1.0f / std::sqrt(lenSq);
            X *= invLen;
            Y *= invLen;
            Z *= invLen;
            W *= invLen;
        }
    }

    Quaternion GetConjugate() const
    {
        return Quaternion{-X, -Y, -Z, W};
    }

    Quaternion GetInversed() const
    {

        float sqLen = X * X + Y * Y + Z * Z + W * W;
        if (sqLen > 0.0000001f)
        {
            return Quaternion{0, 0, 0, 1};
        }

        float invLen = 1.0f / std::sqrt(sqLen);
        return Quaternion{-X * invLen, -Y * invLen, -Z * invLen, W * invLen};
    }

    CoreMath::Vector3 RotateVector(const Vector3 &v) const;
    CoreMath::Vector3 InverseRotateVector(const Vector3 &v) const;

    static Quaternion MakeFromEuler(const Vector3 &euler);

    void ApplyAngularFromVector(const Vector3 &v);

    REFLECT_PROPERTY()
    float X;
    REFLECT_PROPERTY()
    float Y;
    REFLECT_PROPERTY()
    float Z;
    REFLECT_PROPERTY()
    float W;

  private:
};

struct COREMATH_API REFLECT_STRUCT(EngineClass) Matrix4X4
{
  public:
    Matrix4X4() = default;
    ~Matrix4X4() = default;

    Matrix4X4(const Matrix4X4 &) = default;
    Matrix4X4 &operator=(const Matrix4X4 &) = default;

    Matrix4X4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20,
              float m21, float m22, float m23, float m30, float m31, float m32, float m33);

    Matrix4X4(const CoreMath::Vector4 &col1, const CoreMath::Vector4 &col2, const CoreMath::Vector4 &col3,
              const CoreMath::Vector4 &col4);

    bool operator==(const Matrix4X4 &matrix) const
    {
        for (int i = 0; i < 4; ++i)
        {
            if (!mat[i].NearEqual(matrix.mat[i]))
            {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const Matrix4X4 &matrix) const
    {
        return !(this->operator==(matrix));
    }

    Matrix4X4 operator+(const Matrix4X4 &rhs) const;
    Matrix4X4 operator-(const Matrix4X4 &rhs) const;
    Matrix4X4 operator*(const Matrix4X4 &rhs) const; // 행렬 곱셈
    Matrix4X4 operator*(float scalar) const;         // 스칼라 곱셈
    Matrix4X4 operator/(float scalar) const;         // 스칼라 나눗셈 ,스칼라가 0이면 0이곱해진다.

    // --- 복합 대입 연산자 (자신을 수정) ---
    Matrix4X4 &operator+=(const Matrix4X4 &rhs);
    Matrix4X4 &operator-=(const Matrix4X4 &rhs);
    Matrix4X4 &operator*=(const Matrix4X4 &rhs); // 행렬 곱셈 후 대입
    Matrix4X4 &operator*=(float scalar);         // 스칼라 곱셈
    Matrix4X4 &operator/=(float scalar);         // 스칼라 나눗셈 , 스칼라가 0이면 0이 곱해진다.

    Vector4 operator*(const Vector4 &rhs) const; // 행렬 * Vector4
    Vector3 operator*(const Vector3 &rhs) const; // 행렬 * Vector3 벡터의 w성분은 0으로 취급하여 곱한다.

    Vector3 TransformDirection(const Vector3 &rhs) const;
    Vector3 TransformPoint(const Vector3 &rhs) const;

    void TransformDirectionRef(Vector3 &rhs) const;
    void TransformPointRef(Vector3 &rhs) const;

    Matrix4X4 GetTransposed() const; // 전치 행렬을 반환
    Matrix4X4 GetInversed() const;   // 역행렬 반환
    float Determinant() const;       // 행렬식

    void Transpose(); // 자신을  전치시킨다.

    Vector4 GetRow(int rowIndex) const;
    Vector4 GetColumn(int columnIndex) const;

    void SetColComponet(int index, const CoreMath::Vector4 &vec);
    void SetColComponents(const CoreMath::Vector4 &col1, const CoreMath::Vector4 &col2, const CoreMath::Vector4 &col3,
                          const CoreMath::Vector4 &col4 = {0, 0, 0, 1});

    // float mat[16]; // 열우선
    Vector4 mat[4];

    const static Matrix4X4 Zero;
    const static Matrix4X4 Identity;

    static Matrix4X4 MakeTranslation(Vector3 position);          // 이동 행렬
    static Matrix4X4 MakeTranslation(float x, float y, float z); // 이동 행렬
    static Matrix4X4 MakeScale(Vector3 scale);                   // 크기 행렬
    static Matrix4X4 MakeScale(float x, float y, float z);       // 크기 행렬
    static Matrix4X4 MakeScale(float value);                     // 크기 행렬

    static Matrix4X4 MakeRotationRadX(float x); // X축 회전 (라디안)
    static Matrix4X4 MakeRotationRadY(float y); // Y축 회전 (라디안)
    static Matrix4X4 MakeRotationRadZ(float z); // Z축 회전 (라디안)

    static Matrix4X4 MakeRotationDegreeX(float x); // X축 회전 (도)
    static Matrix4X4 MakeRotationDegreeY(float y); // Y축 회전 (도)
    static Matrix4X4 MakeRotationDegreeZ(float z); // Z축 회전 (도)

    static Matrix4X4 MakeRotationYawPitchRollRad(float yaw, float pitch, float roll);

    static Matrix4X4 MakeTransform(Vector3 position, Quaternion rotation, Vector3 scale);

    // static Matrix4X4 MakeRotationDegree(float x, float y, float z);
    //  static Matrix4x4 MakeRotation(const Quaternion& q); // 쿼터니언 회전 행렬

    static void MatrixDecompose(Vector3 &oPosition, Quaternion &oQuaternion, Vector3 &oScale, const Matrix4X4 &matrix);

    bool Decompose(Vector3 &oPosition, Quaternion &oQuaternion, Vector3 &oScale) const;

    // 교대대칭행렬로 설정한다
    void SetSkewSymmetric(const CoreMath::Vector3 &vector);

    // 인터페이스만 만든상태
    // 뷰(View) 행렬 - 왼손/오른손 좌표계 고려
    static Matrix4X4 MakeLookAtLH(const Vector3 &eye, const Vector3 &target, const Vector3 &up); // 왼손 LookAt
    // static Matrix4x4 MakeLookAtRH(Vector3 eye, Vector3 target, Vector3 up); // 오른손 LookAt

    // 투영(Projection) 행렬 - 왼손/오른손 좌표계 고려
    static Matrix4X4 MakePerspectiveFovLH(float fovAngleY, float aspectRatio, float nearZ, float farZ); // 왼손 원근
    // static Matrix4x4 MakePerspectiveFovRH(float fovAngleY, float aspectRatio, float nearZ, float farZ); // 오른손
    // 원근
    static Matrix4X4 MakeOrthographicLH(float left, float right, float bottom, float top, float nearZ,
                                        float farZ); // 왼손 직교
    // static Matrix4x4 MakeOrthographicRH(float width, float height, float nearZ, float farZ); // 오른손 직교

  private:
};

inline Quaternion operator*(float value, const Quaternion &quaternion)
{
    return Quaternion(quaternion.X * value, quaternion.Y * value, quaternion.Z * value, quaternion.W * value);
}

inline float RadianToDegree(float angle)
{
    return angle / CoreMath::TWO_PI * 360.0f;
}

} // namespace CoreMath

COREMATH_API Arch &operator<<(Arch &arch, CoreMath::Vector2 &vector2);
COREMATH_API Arch &operator<<(Arch &arch, CoreMath::Vector3 &vector3);
COREMATH_API Arch &operator<<(Arch &arch, CoreMath::Vector4 &vector4);

inline Arch &operator<<(Arch &arch, CoreMath::Quaternion &quat)
{
    arch << quat.X;
    arch << quat.Y;
    arch << quat.Z;
    arch << quat.W;

    return arch;
}
