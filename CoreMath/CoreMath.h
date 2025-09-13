#pragma once

namespace CoreMath
{
class Vector2
{
  public:
    Vector2() = default;
    ~Vector2() = default;

    Vector2(const Vector2 &rhs) : X(rhs.X), Y(rhs.Y) {}

    Vector2(float x, float y) : X(x), Y(y) {}

    Vector2 &operator=(const Vector2 &rhs) = default;

    bool operator==(const Vector2 &vector) const;
    bool operator!=(const Vector2 &vector) const;

    Vector2 operator+(const Vector2 &rhs) const;
    Vector2 operator-(const Vector2 &rhs) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;

    Vector2 &operator+=(const Vector2 &rhs);
    Vector2 &operator-=(const Vector2 &rhs);
    Vector2 &operator*=(float scalar);
    Vector2 &operator/=(float scalar);

    void Set(float x, float y, float z)
    {
        X = x;
        Y = y;
    }

    float Length() const;
    float LengthSquared() const;
    void Normalize();
    Vector2 Normalize() const;
    float Dot(const Vector2 &vector) const;

    bool Equal(const Vector2 &vector);
    bool NearEqual(const Vector2 &vector, float epsilon = 0.00001f);

    float X = 0.0f;
    float Y = 0.0f;

    const static Vector2 Zero;
    const static Vector2 One;
    const static Vector2 UintX;
    const static Vector2 UintY;

  private:
};

class Vector3
{
  public:
    Vector3() = default;
    ~Vector3() = default;

    Vector3(const Vector3 &rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z) {}

    Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

    Vector3 &operator=(const Vector3 &rhs) = default;

    bool operator==(const Vector3 &vector) const;
    bool operator!=(const Vector3 &vector) const;

    Vector3 operator+(const Vector3 &rhs) const;
    Vector3 operator-(const Vector3 &rhs) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    Vector3 &operator+=(const Vector3 &rhs);
    Vector3 &operator-=(const Vector3 &rhs);
    Vector3 &operator*=(float scalar);
    Vector3 &operator/=(float scalar);

    void Set(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    float Length() const;
    float LengthSquared() const;
    void Normalize();
    Vector3 Normalize() const;
    float Dot(const Vector3 &vector) const;

    Vector3 Cross(const Vector3 &vector) const;

    bool Equal(const Vector3 &vector);
    bool NearEqual(const Vector3 &vector, float epsilon = 0.00001f);

    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    const static Vector3 Zero;
    const static Vector3 One;
    const static Vector3 UintX;
    const static Vector3 UintY;
    const static Vector3 UintZ;

  private:
};

class Vector4
{
  public:
    Vector4() = default;
    ~Vector4() = default;

    Vector4(const Vector4 &rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z), W(rhs.W) {}

    Vector4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

    Vector4 &operator=(const Vector4 &rhs) = default;

    bool operator==(const Vector4 &vector) const;
    bool operator!=(const Vector4 &vector) const;

    Vector4 operator+(const Vector4 &rhs) const;
    Vector4 operator-(const Vector4 &rhs) const;
    Vector4 operator*(float scalar) const;
    Vector4 operator/(float scalar) const;

    Vector4 &operator+=(const Vector4 &rhs);
    Vector4 &operator-=(const Vector4 &rhs);
    Vector4 &operator*=(float scalar);
    Vector4 &operator/=(float scalar);

    void Set(float x, float y, float z, float w)
    {
        X = x;
        Y = y;
        Z = z;
        W = w;
    }

    float Length() const;
    float LengthSquared() const;
    void Normalize();
    Vector4 Normalize() const;
    float Dot(const Vector4 &vector) const;
    bool Equal(const Vector4 &vector);
    bool NearEqual(const Vector4 &vector, float epsilon = 0.00001f);

    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    float W = 0.0f;

    const static Vector4 Zero;
    const static Vector4 One;
    const static Vector4 UintX;
    const static Vector4 UintY;
    const static Vector4 UintZ;
    const static Vector4 UintW;

  private:
};

class Quaternion
{
  public:
    Quaternion() = default;
    ~Quaternion() = default;

    Quaternion(const Quaternion &rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z), W(rhs.W) {}

    Quaternion(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

    Quaternion &operator=(const Quaternion &rhs)
    {

        X = rhs.X;
        Y = rhs.Y;
        Z = rhs.Z;
        W = rhs.W;
        return *this;
    };

    Quaternion operator*(const Quaternion &rhs) const;

    float X;
    float Y;
    float Z;
    float W;

  private:
};

class Matrix4X4
{
  public:
    Matrix4X4() = default;
    ~Matrix4X4() = default;

    Matrix4X4(const Matrix4X4 &) = default;
    Matrix4X4 &operator=(const Matrix4X4 &) = default;

    Matrix4X4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20,
              float m21, float m22, float m23, float m30, float m31, float m32, float m33);

    // bool operator==(const Matrix4X4& matrix) const;
    // bool operator!=(const Matrix4X4& matrix) const;

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

    Matrix4X4 Transpose() const; // 전치 행렬을 반환
    Matrix4X4 Inverse() const;   // 역행렬 반환
    float Determinant() const;   // 행렬식

    void Transpose(); // 자신을  전치시킨다.

    Vector4 GetRow(int rowIndex) const;
    Vector4 GetColumn(int columnIndex) const;

    float m[4][4]; // 행우선 ;

    const static Matrix4X4 Zero;
    const static Matrix4X4 Identity;

    static Matrix4X4 MakeTranslation(Vector3 position);          // 이동 행렬
    static Matrix4X4 MakeTranslation(float x, float y, float z); // 이동 행렬
    static Matrix4X4 MakeScale(Vector3 scale);                   // 크기 행렬
    static Matrix4X4 MakeScale(float x, float y, float z);       // 크기 행렬

    static Matrix4X4 MakeRotationRadX(float x); // X축 회전 (라디안)
    static Matrix4X4 MakeRotationRadY(float y); // Y축 회전 (라디안)
    static Matrix4X4 MakeRotationRadZ(float z); // Z축 회전 (라디안)

    static Matrix4X4 MakeRotationDegreeX(float x); // X축 회전 (도)
    static Matrix4X4 MakeRotationDegreeY(float y); // Y축 회전 (도)
    static Matrix4X4 MakeRotationDegreeZ(float z); // Z축 회전 (도)

    static Matrix4X4 MakeRotationYawPitchRollRad(float x, float y, float z);

    static Matrix4X4 MakeTransform(Vector3 position, Quaternion rotation, Vector3 scale);

    // static Matrix4X4 MakeRotationDegree(float x, float y, float z);
    //  static Matrix4x4 MakeRotation(const Quaternion& q); // 쿼터니언 회전 행렬

    static void MatrixDecompose(Vector3 &oPosition, Quaternion &oQuaternion, Vector3 &oScale, const Matrix4X4 &matrix);

    // 인터페이스만 만든상태
    // 뷰(View) 행렬 - 왼손/오른손 좌표계 고려
    static Matrix4X4 MakeLookAtLH(Vector3 eye, Vector3 target, Vector3 up); // 왼손 LookAt
    // static Matrix4x4 MakeLookAtRH(Vector3 eye, Vector3 target, Vector3 up); // 오른손 LookAt

    // 투영(Projection) 행렬 - 왼손/오른손 좌표계 고려
    static Matrix4X4 MakePerspectiveFovLH(float fovAngleY, float aspectRatio, float nearZ, float farZ); // 왼손 원근
    // static Matrix4x4 MakePerspectiveFovRH(float fovAngleY, float aspectRatio, float nearZ, float farZ); // 오른손
    // 원근
    static Matrix4X4 MakeOrthographicLH(float width, float height, float nearZ, float farZ); // 왼손 직교
    // static Matrix4x4 MakeOrthographicRH(float width, float height, float nearZ, float farZ); // 오른손 직교

  private:
};

} // namespace CoreMath
