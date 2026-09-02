#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "CoreMath/CoreMath.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
namespace CoreMath
{
const Vector2 Vector2::Zero = Vector2{0.0f, 0.0f};
const Vector2 Vector2::One = Vector2{1.0f, 1.0f};
const Vector2 Vector2::UnitX = Vector2{1.0f, 0.0f};
const Vector2 Vector2::UnitY = Vector2{0.0f, 1.0f};

const Vector3 Vector3::Zero = Vector3{0.0f, 0.0f, 0.0f};
const Vector3 Vector3::One = Vector3{1.0f, 1.0f, 1.0f};
const Vector3 Vector3::UintX = Vector3{1.0f, 0.0f, 0.0f};
const Vector3 Vector3::UintY = Vector3{0.0f, 1.0f, 0.0f};
const Vector3 Vector3::UintZ = Vector3{0.0f, 0.0f, 1.0f};

Vector3::Vector3(const Vector4 &vec4) : X(vec4.X), Y(vec4.Y), Z(vec4.Z) {}

const Vector4 Vector4::Zero = Vector4{0.0f, 0.0f, 0.0f, 0.0f};
const Vector4 Vector4::One = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
const Vector4 Vector4::UnitX = Vector4{1.0f, 0.0f, 0.0f, 0.0f};
const Vector4 Vector4::UnitY = Vector4{0.0f, 1.0f, 0.0f, 0.0f};
const Vector4 Vector4::UnitZ = Vector4{0.0f, 0.0f, 1.0f, 0.0f};
const Vector4 Vector4::UnitW = Vector4{0.0f, 0.0f, 0.0f, 1.0f};

Quaternion::Quaternion(const Vector3 &rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z), W(0.0f) {}

Quaternion Quaternion::operator*(const Quaternion &rhs) const
{

    glm::quat q1 = glm::make_quat(&X);
    glm::quat q2 = glm::make_quat(&rhs.X);

    glm::quat ret = q1 * q2;

    Quaternion result;
    memcpy(&result.X, glm::value_ptr(ret), sizeof(float) * 4);
    return result;
}

Quaternion Quaternion::operator*(float value) const
{
    return Quaternion(X * value, Y * value, Z * value, W * value);
}

Quaternion &Quaternion::operator+=(const Quaternion &rhs)
{

    glm::quat q1 = glm::make_quat(&X);
    glm::quat q2 = glm::make_quat(&rhs.X);

    glm::quat ret = q1 + q2;

    memcpy(&X, glm::value_ptr(ret), sizeof(float) * 4);
    return *this;
}

Vector3 CoreMath::Quaternion::ToEulerAngles() const
{

    glm::quat qa = glm::make_quat(&X);

    const glm::vec3 vec1 = glm::degrees(glm::eulerAngles(qa));

    Vector3 result;
    memcpy(&result.X, glm::value_ptr(vec1), sizeof(float) * 3);

    return result;
}

CoreMath::Vector3 Quaternion::RotateVector(const Vector3 &v) const
{

    glm::quat qa = glm::make_quat(&X);

    glm::vec3 v3 = glm::make_vec3(&v.X);

    glm::vec3 nv3 = qa * v3;

    Vector3 result;
    memcpy(&result.X, glm::value_ptr(nv3), sizeof(float) * 3);
    return result;
}

CoreMath::Vector3 Quaternion::InverseRotateVector(const Vector3 &v) const
{

    CoreMath::Quaternion invqa = GetConjugate();
    return invqa.RotateVector(v);
}

Quaternion CoreMath::Quaternion::MakeFromEuler(const Vector3 &euler)
{

    glm::vec3 vec1 = glm::make_vec3(&euler.X);

    vec1 = glm::radians(vec1);

    float pitch = glm::radians(euler.X);
    float yaw = glm::radians(euler.Y);
    float roll = glm::radians(euler.Z);

    glm::quat q =                                 // 세번째
        glm::angleAxis(yaw, glm::vec3(0, 1, 0)) * // 첫번째
        glm::angleAxis(pitch, glm::vec3(1, 0, 0)) * glm::angleAxis(roll, glm::vec3(0, 0, 1));

    Quaternion result;
    memcpy(&result.X, glm::value_ptr(q), sizeof(float) * 4);
    return result;
}

void CoreMath::Quaternion::ApplyAngularFromVector(const Vector3 &v)
{

    Quaternion delta(v.X, v.Y, v.Z, 0.0f);
    *this += 0.5f * delta * *this;
    // Normalize();
}

Matrix4X4::Matrix4X4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20,
                     float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
    // 1열
    mat[0][0] = m00;
    mat[0][1] = m10;
    mat[0][2] = m20;
    mat[0][3] = m30;

    // 2열
    mat[1][0] = m01;
    mat[1][1] = m11;
    mat[1][2] = m21;
    mat[1][3] = m31;

    // 3열
    mat[2][0] = m02;
    mat[2][1] = m12;
    mat[2][2] = m22;
    mat[2][3] = m32;

    // 4열
    mat[3][0] = m03;
    mat[3][1] = m13;
    mat[3][2] = m23;
    mat[3][3] = m33;
}

const Matrix4X4 Matrix4X4::Zero = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0, 0};
const Matrix4X4 Matrix4X4::Identity = {1.0f, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f};

Matrix4X4::Matrix4X4(const CoreMath::Vector4 &col1, const CoreMath::Vector4 &col2, const CoreMath::Vector4 &col3,
                     const CoreMath::Vector4 &col4)
{
    mat[0] = col1;
    mat[1] = col2;
    mat[2] = col3;
    mat[3] = col4;
}

Matrix4X4 Matrix4X4::operator+(const Matrix4X4 &rhs) const
{
    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);
    const glm::mat4 mat2 = glm::make_mat4(&rhs.mat[0].X);

    glm::mat4 result = mat1 + mat2;

    Matrix4X4 ret;
    memcpy(ret.mat, glm::value_ptr(result), sizeof(float) * 16);
    return ret;
}
Matrix4X4 Matrix4X4::operator-(const Matrix4X4 &rhs) const
{

    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);
    const glm::mat4 mat2 = glm::make_mat4(&rhs.mat[0].X);

    glm::mat4 result = mat1 - mat2;

    Matrix4X4 ret;
    memcpy(ret.mat, glm::value_ptr(result), sizeof(float) * 16);
    return ret;
}
Matrix4X4 Matrix4X4::operator*(const Matrix4X4 &rhs) const // 행렬 곱셈
{
    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);
    const glm::mat4 mat2 = glm::make_mat4(&rhs.mat[0].X);

    glm::mat4 result = mat1 * mat2;

    Matrix4X4 ret;
    memcpy(ret.mat, glm::value_ptr(result), sizeof(float) * 16);
    return ret;
}
Matrix4X4 Matrix4X4::operator*(float scalar) const // 스칼라 곱셈
{
    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    glm::mat4 result = mat1 * scalar;

    Matrix4X4 ret;
    memcpy(ret.mat, glm::value_ptr(result), sizeof(float) * 16);

    return ret;
}
Matrix4X4 Matrix4X4::operator/(float scalar) const // 스칼라 나눗셈 ,스칼라가 0이면 0이곱해진다
{
    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    glm::mat4 result = mat1 / scalar;

    Matrix4X4 ret;
    memcpy(ret.mat, glm::value_ptr(result), sizeof(float) * 16);
    return ret;
}

Vector4 Matrix4X4::operator*(const Vector4 &rhs) const // 행렬 * Vector4
{
    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    const glm::vec4 vec1 = glm::make_vec4(&rhs.X);

    glm::vec4 result = mat1 * vec1;

    Vector4 vec;
    memcpy(&vec, glm::value_ptr(result), sizeof(float) * 4);
    return vec;
}
Vector3 Matrix4X4::operator*(const Vector3 &rhs) const // 행렬 * Vector3 벡터의 w성분은 0으로 취급하여 곱한다.
{
    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    const glm::vec4 vec1 = {rhs.X, rhs.Y, rhs.Z, 0.0f};

    glm::vec4 result = mat1 * vec1;

    return Vector3{result.x, result.y, result.z};

} // namespace CoreMath

Matrix4X4 &Matrix4X4::operator+=(const Matrix4X4 &rhs)
{

    mat[0] += rhs.mat[0];
    mat[1] += rhs.mat[1];
    mat[2] += rhs.mat[2];
    mat[3] += rhs.mat[3];
    return *this;
}
Matrix4X4 &Matrix4X4::operator-=(const Matrix4X4 &rhs)
{

    mat[0] -= rhs.mat[0];
    mat[1] -= rhs.mat[1];
    mat[2] -= rhs.mat[2];
    mat[3] -= rhs.mat[3];
    return *this;
}

Matrix4X4 &Matrix4X4::operator*=(const Matrix4X4 &rhs)
{
    *this = (*this) * rhs;
    return *this;
}

Matrix4X4 &Matrix4X4::operator*=(float scalar)
{

    *this = (*this) * scalar;

    return *this;
}
Matrix4X4 &Matrix4X4::operator/=(float scalar)
{

    *this = (*this) / scalar;
    return *this;
}

Vector3 Matrix4X4::TransformDirection(const Vector3 &rhs) const
{

    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    const glm::vec4 vec1 = {rhs.X, rhs.Y, rhs.Z, 0.0f};

    glm::vec4 result = mat1 * vec1;

    return Vector3{result.x, result.y, result.z};
}
Vector3 Matrix4X4::TransformPoint(const Vector3 &rhs) const
{

    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    const glm::vec4 vec1 = {rhs.X, rhs.Y, rhs.Z, 1.0f};

    glm::vec4 result = mat1 * vec1;

    return Vector3{result.x, result.y, result.z};
}

void Matrix4X4::TransformDirectionRef(Vector3 &rhs) const
{

    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    const glm::vec4 vec1 = {rhs.X, rhs.Y, rhs.Z, 0.0f};

    glm::vec4 result = mat1 * vec1;

    memcpy(&rhs, glm::value_ptr(result), sizeof(float) * 3);

    return;
}

void Matrix4X4::TransformPointRef(Vector3 &rhs) const
{

    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    const glm::vec4 vec1 = {rhs.X, rhs.Y, rhs.Z, 1.0f};

    glm::vec4 result = mat1 * vec1;

    memcpy(&rhs, glm::value_ptr(result), sizeof(float) * 3);
}

Matrix4X4 Matrix4X4::GetTransposed() const // 전치 행렬을 반환
{

    glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    mat1 = glm::transpose(mat1);

    Matrix4X4 ret;
    memcpy(ret.mat, glm::value_ptr(mat1), sizeof(float) * 16);

    return ret;
}
Matrix4X4 Matrix4X4::GetInversed() const // 역행렬 반환
{

    glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    mat1 = glm::inverse(mat1);

    Matrix4X4 ret;
    memcpy(ret.mat, glm::value_ptr(mat1), sizeof(float) * 16);

    return ret;
}
float Matrix4X4::Determinant() const // 행렬식
{
    const glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    return glm::determinant(mat1);
}

void Matrix4X4::Transpose() // 자신을  전치시킨다.
{
    glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    mat1 = glm::transpose(mat1);

    memcpy(mat, glm::value_ptr(mat1), sizeof(float) * 16);
}

Vector4 Matrix4X4::GetRow(int rowIndex) const
{
    return Vector4{mat[0][rowIndex], mat[1][rowIndex], mat[2][rowIndex], mat[3][rowIndex]};
}
Vector4 Matrix4X4::GetColumn(int columnIndex) const
{

    return mat[columnIndex];
}

void Matrix4X4::SetColComponet(int index, const CoreMath::Vector4 &vec)
{

    if (index >= 4)
        return;

    mat[index] = vec;
}
void Matrix4X4::SetColComponents(const CoreMath::Vector4 &col1, const CoreMath::Vector4 &col2,
                                 const CoreMath::Vector4 &col3, const CoreMath::Vector4 &col4)
{
    mat[0] = col1;
    mat[1] = col2;
    mat[2] = col3;
    mat[3] = col4;
}

Matrix4X4 Matrix4X4::MakeTranslation(Vector3 position) // 이동 행렬
{
    const glm::vec3 vec1 = glm::make_vec3(&position.X);

    glm::mat4x4 ret = glm::translate(glm::identity<glm::mat4x4>(), vec1);

    Matrix4X4 result;
    memcpy(result.mat, glm::value_ptr(ret), sizeof(float) * 16);

    return result;
}

Matrix4X4 Matrix4X4::MakeTranslation(float x, float y, float z) // 이동 행렬
{
    return MakeTranslation({x, y, z});
}
Matrix4X4 Matrix4X4::MakeScale(Vector3 scale) // 크기 행렬
{
    const glm::vec3 vec1 = glm::make_vec3(&scale.X);

    glm::mat4x4 ret = glm::scale(glm::identity<glm::mat4x4>(), vec1);

    Matrix4X4 result;
    memcpy(result.mat, glm::value_ptr(ret), sizeof(float) * 16);

    return result;
}
Matrix4X4 Matrix4X4::MakeScale(float x, float y, float z) // 크기 행렬
{
    return MakeScale({x, y, z});
}

Matrix4X4 Matrix4X4::MakeScale(float value) // 크기 행렬
{
    return MakeScale({value, value, value});
}

Matrix4X4 Matrix4X4::MakeRotationRadX(float x) // X축 회전 (라디안)
{

    glm::mat4x4 ret = glm::rotate(glm::identity<glm::mat4x4>(), x, glm::vec3{1, 0, 0});

    Matrix4X4 result;
    memcpy(result.mat, glm::value_ptr(ret), sizeof(float) * 16);

    return result;
}
Matrix4X4 Matrix4X4::MakeRotationRadY(float y) // Y축 회전 (라디안)
{

    glm::mat4 ret = glm::rotate(glm::mat4(1.0f), y, glm::vec3{0, 1, 0});
    Matrix4X4 result;
    memcpy(result.mat, glm::value_ptr(ret), sizeof(float) * 16);
    return result;
}

Matrix4X4 Matrix4X4::MakeRotationRadZ(float z) // Z축 회전 (라디안)
{

    glm::mat4 ret = glm::rotate(glm::mat4(1.0f), z, glm::vec3{0, 0, 1});
    Matrix4X4 result;
    memcpy(result.mat, glm::value_ptr(ret), sizeof(float) * 16);
    return result;
}

Matrix4X4 Matrix4X4::MakeRotationDegreeX(float x) // X축 회전 (도)
{

    return MakeRotationRadX(glm::radians(x));
}

Matrix4X4 Matrix4X4::MakeRotationDegreeY(float y) // Y축 회전 (도)
{
    return MakeRotationRadY(glm::radians(y));
}

Matrix4X4 Matrix4X4::MakeRotationDegreeZ(float z) // Z축 회전 (도)
{
    return MakeRotationRadZ(glm::radians(z));
}

Matrix4X4 Matrix4X4::MakeRotationYawPitchRollRad(float yaw, float pitch, float roll)
{
    glm::quat q = glm::angleAxis(yaw, glm::vec3(0, 0, 1)) * glm::angleAxis(pitch, glm::vec3(1, 0, 0)) *
                  glm::angleAxis(roll, glm::vec3(0, 1, 0));

    glm::mat4 mat = glm::mat4_cast(q);

    Matrix4X4 result;
    memcpy(result.mat, glm::value_ptr(mat), sizeof(float) * 16);
    return result;
}
Matrix4X4 Matrix4X4::MakeTransform(Vector3 position, Quaternion quaternion, Vector3 scale)
{

    const glm::vec3 translationVec = glm::make_vec3(&position.X);
    glm::mat4x4 translationMat = glm::translate(glm::identity<glm::mat4x4>(), translationVec);

    const glm::quat rotationQuat = glm::make_quat(&quaternion.X);
    glm::mat4x4 quatMat = glm::mat4_cast(rotationQuat);

    const glm::vec3 scaleVec = glm::make_vec3(&scale.X);
    glm::mat4x4 scaleMat = glm::scale(glm::identity<glm::mat4x4>(), scaleVec);

    glm::mat4x4 result = translationMat * quatMat * scaleMat;

    Matrix4X4 ret;

    memcpy(ret.mat, glm::value_ptr(result), sizeof(float) * 16);

    return ret;
}

bool Matrix4X4::Decompose(Vector3 &oPosition, Quaternion &oQuaternion, Vector3 &oScale) const
{

    glm::mat4 mat1 = glm::make_mat4(&mat[0].X);

    glm::vec3 scale;
    glm::quat quaternion;
    glm::vec3 position;
    glm::vec3 skew;
    glm::vec4 perspective;

    bool ret = glm::decompose(mat1, scale, quaternion, position, skew, perspective);

    if (ret)
    {
        memcpy(&oPosition.X, glm::value_ptr(position), sizeof(float) * 3);
        memcpy(&oQuaternion.X, glm::value_ptr(quaternion), sizeof(float) * 4);
        memcpy(&oScale.X, glm::value_ptr(scale), sizeof(float) * 3);
    }
    return ret;
}

void Matrix4X4::SetSkewSymmetric(const CoreMath::Vector3 &vector)
{

    mat[0].X = 0;
    mat[0].Y = vector.Z;
    mat[0].Z = -vector.Y;
    mat[0].W = 0;

    mat[1].X = -vector.Z;
    mat[1].Y = 0;
    mat[1].Z = vector.X;
    mat[1].W = 0;

    mat[2].X = vector.Y;
    mat[2].Y = -vector.X;
    mat[2].Z = 0;
    mat[2].W = 0;

    mat[3] = {0, 0, 0, 1};
}

Matrix4X4 Matrix4X4::MakeLookAtLH(const Vector3 &eye, const Vector3 &target, const Vector3 &up)
{

    glm::vec3 e = glm::make_vec3(&eye.X);
    glm::vec3 t = glm::make_vec3(&target.X);
    glm::vec3 u = glm::make_vec3(&up.X);

    glm::mat4 ret = glm::lookAtLH(e, t, glm::normalize(u));

    Matrix4X4 result;
    memcpy(result.mat, glm::value_ptr(ret), sizeof(float) * 16);
    return result;
}
Matrix4X4 Matrix4X4::MakePerspectiveFovLH(float fovAngleY, float aspectRatio, float nearZ, float farZ) // 왼손 원근
{
    float angleInRadian = glm::radians(fovAngleY);

    glm::mat4 mat1 = glm::perspectiveFovLH_ZO(angleInRadian, aspectRatio, 1.0f, nearZ, farZ);

    Matrix4X4 result;
    memcpy(result.mat, glm::value_ptr(mat1), sizeof(float) * 16);
    return result;
}
// static Matrix4x4 MakePerspectiveFovRH(float fovAngleY, float aspectRatio, float nearZ, float farZ); // 오른손
// 원근
Matrix4X4 Matrix4X4::MakeOrthographicLH(float left, float right, float bottom, float top, float nearZ,
                                        float farZ) // 왼손 직교
{

    glm::mat4 mat1 = glm::orthoLH_ZO(left, right, bottom, top, nearZ, farZ);

    Matrix4X4 result;
    memcpy(result.mat, glm::value_ptr(mat1), sizeof(float) * 16);
    return result;
}
} // namespace CoreMath

// namespace CoreMath

Arch &operator<<(Arch &arch, CoreMath::Vector2 &vector2)
{
    arch << vector2.X;
    arch << vector2.Y;
    return arch;
}

Arch &operator<<(Arch &arch, CoreMath::Vector3 &vector3)
{
    arch << vector3.X;
    arch << vector3.Y;
    arch << vector3.Z;

    return arch;
}
Arch &operator<<(Arch &arch, CoreMath::Vector4 &vector4)
{
    arch << vector4.X;
    arch << vector4.Y;
    arch << vector4.Z;
    arch << vector4.W;
    return arch;
}
