#include "GeometryGenerator.h"
#define _USE_MATH_DEFINES
#include <cmath>
CoreAsset::GeometryGenerator::GeometryGenerator() {}

void CoreAsset::GeometryGenerator::AddCubeMeshData(float width, float height, float depth,
                                                   std::vector<StaticVertex> &oVertices,
                                                   std::vector<uint32_t> &oIndices, const CoreMath::Vector3 &offsetPos)
{

    float halfWidth = width / 2;
    float halfHeight = height / 2;
    float halfDepth = depth / 2;

    MeshData meshData;
    std::vector<CoreAsset::MeshIndexType> indexVector = {
        // Front (+Z)
        0,
        2,
        1,
        0,
        3,
        2,

        // Back (-Z)
        4,
        5,
        6,
        4,
        6,
        7,

        // Left (-X)
        8,
        9,
        10,
        8,
        10,
        11,

        // Right (+X)
        12,
        13,
        14,
        12,
        14,
        15,

        // Top (+Y)
        16,
        17,
        18,
        16,
        18,
        19,

        // Bottom (-Y)
        20,
        22,
        21,
        20,
        23,
        22,
    };
    std::vector<CoreAsset::StaticVertex> vertexVector = {
        // Front (+Z)
        {{-halfWidth, -halfHeight, +halfDepth}, {0.0f, 1.0f}, {0, 0, +1}},
        {{-halfWidth, +halfHeight, +halfDepth}, {0.0f, 0.0f}, {0, 0, +1}},
        {{+halfWidth, +halfHeight, +halfDepth}, {1.0f, 0.0f}, {0, 0, +1}},
        {{+halfWidth, -halfHeight, +halfDepth}, {1.0f, 1.0f}, {0, 0, +1}},

        // Back (-Z)
        {{-halfWidth, -halfHeight, -halfDepth}, {0.0f, 1.0f}, {0, 0, -1}},
        {{-halfWidth, +halfHeight, -halfDepth}, {0.0f, 0.0f}, {0, 0, -1}},
        {{+halfWidth, +halfHeight, -halfDepth}, {1.0f, 0.0f}, {0, 0, -1}},
        {{+halfWidth, -halfHeight, -halfDepth}, {1.0f, 1.0f}, {0, 0, -1}},

        // Left (-X)
        {{-halfWidth, -halfHeight, +halfDepth}, {0.0f, 1.0f}, {-1, 0, 0}},
        {{-halfWidth, +halfHeight, +halfDepth}, {0.0f, 0.0f}, {-1, 0, 0}},
        {{-halfWidth, +halfHeight, -halfDepth}, {1.0f, 0.0f}, {-1, 0, 0}},
        {{-halfWidth, -halfHeight, -halfDepth}, {1.0f, 1.0f}, {-1, 0, 0}},

        // Right (+X)
        {{+halfWidth, -halfHeight, -halfDepth}, {0.0f, 1.0f}, {+1, 0, 0}},
        {{+halfWidth, +halfHeight, -halfDepth}, {0.0f, 0.0f}, {+1, 0, 0}},
        {{+halfWidth, +halfHeight, +halfDepth}, {1.0f, 0.0f}, {+1, 0, 0}},
        {{+halfWidth, -halfHeight, +halfDepth}, {1.0f, 1.0f}, {+1, 0, 0}},

        // Top (+Y)
        {{-halfWidth, +halfHeight, -halfDepth}, {0.0f, 1.0f}, {0, +1, 0}},
        {{-halfWidth, +halfHeight, +halfDepth}, {0.0f, 0.0f}, {0, +1, 0}},
        {{+halfWidth, +halfHeight, +halfDepth}, {1.0f, 0.0f}, {0, +1, 0}},
        {{+halfWidth, +halfHeight, -halfDepth}, {1.0f, 1.0f}, {0, +1, 0}},

        // Bottom (-Y)
        {{-halfWidth, -halfHeight, -halfDepth}, {0.0f, 1.0f}, {0, -1, 0}},
        {{-halfWidth, -halfHeight, +halfDepth}, {0.0f, 0.0f}, {0, -1, 0}},
        {{+halfWidth, -halfHeight, +halfDepth}, {1.0f, 0.0f}, {0, -1, 0}},
        {{+halfWidth, -halfHeight, -halfDepth}, {1.0f, 1.0f}, {0, -1, 0}},
    };

    for (auto &vertex : vertexVector)
    {
        vertex.mPos += offsetPos;
    }
    CaculateTangents(vertexVector, indexVector);

    // index offset 적용
    for (auto &index : indexVector)
    {
        index += oVertices.size();
    }

    oVertices.insert(oVertices.end(), vertexVector.begin(), vertexVector.end());
    oIndices.insert(oIndices.end(), indexVector.begin(), indexVector.end());
}

void CoreAsset::GeometryGenerator::CaculateTangent(const CoreMath::Vector3 &v1Pos, const CoreMath::Vector3 &v2Pos,
                                                   const CoreMath::Vector3 &v3Pos, const CoreMath::Vector2 &v1Tex,
                                                   const CoreMath::Vector2 &v2Tex, const CoreMath::Vector2 &v3Tex,
                                                   CoreMath::Vector4 &outV1Tangent, CoreMath::Vector4 &outV2Tangent,
                                                   CoreMath::Vector4 &outV3Tangent, CoreMath::Vector3 &outV1BiTangent,
                                                   CoreMath::Vector3 &outV2BiTangent, CoreMath::Vector3 &outV3BiTangent)
{

    CoreMath::Vector3 q1 = v2Pos - v1Pos;
    CoreMath::Vector3 q2 = v3Pos - v1Pos;

    float u21 = v2Tex.X - v1Tex.X;
    float v21 = v2Tex.Y - v1Tex.Y;

    float u31 = v3Tex.X - v1Tex.X;
    float v31 = v3Tex.Y - v1Tex.Y;

    float denominator = u21 * v31 - v21 * u31;

    if (std::abs(denominator) < 0.000001f)
        return;

    CoreMath::Vector4 tangent;
    tangent.X = v31 * q1.X - v21 * q2.X;
    tangent.Y = v31 * q1.Y - v21 * q2.Y;
    tangent.Z = v31 * q1.Z - v21 * q2.Z;

    tangent *= (1.0f / denominator);

    tangent.W = 0.0f;

    outV1Tangent += tangent;
    outV2Tangent += tangent;
    outV3Tangent += tangent;

    CoreMath::Vector3 bitangent;
    bitangent.X = u21 * q2.X - u31 * q1.X;
    bitangent.Y = u21 * q2.Y - u31 * q1.Y;
    bitangent.Z = u21 * q2.Z - u31 * q1.Z;

    bitangent *= (1.0f / denominator);
    outV1BiTangent += bitangent;
    outV2BiTangent += bitangent;
    outV3BiTangent += bitangent;
}

void CoreAsset::GeometryGenerator::FinishCaculateTangent(const CoreMath::Vector3 &normal,
                                                         const CoreMath::Vector3 &biTangent,
                                                         CoreMath::Vector4 &outTangent)
{

    CoreMath::Vector3 tangent = {outTangent.X, outTangent.Y, outTangent.Z};
    tangent = (tangent - CoreMath::Dot(normal, tangent) * normal).GetNormalize();

    outTangent.W = CoreMath::Dot(normal.Cross(tangent), biTangent) < 0.0f ? -1.0f : 1.0f;
    outTangent.X = tangent.X;
    outTangent.Y = tangent.Y;
    outTangent.Z = tangent.Z;
}

void CoreAsset::GeometryGenerator::CaculateVertexNoraml(std::vector<StaticVertex> &vertexVec,
                                                        const std::vector<MeshIndexType> &indexVec)
{

    for (size_t i = 0; i < indexVec.size(); i += 3)
    {

        MeshIndexType index1 = indexVec[i];
        MeshIndexType index2 = indexVec[i + 1];
        MeshIndexType index3 = indexVec[i + 2];

        CoreMath::Vector3 e1 = vertexVec[index2].mPos - vertexVec[index1].mPos;
        CoreMath::Vector3 e2 = vertexVec[index3].mPos - vertexVec[index1].mPos;

        CoreMath::Vector3 faceNormal = e1.Cross(e2).GetNormalize();

        vertexVec[index1].mNormal += faceNormal;
        vertexVec[index2].mNormal += faceNormal;
        vertexVec[index3].mNormal += faceNormal;
    }

    for (auto &vertex : vertexVec)
    {
        vertex.mNormal.Normalize();
    }
}

CoreAsset::GeometryGenerator::~GeometryGenerator() {}

CoreAsset::MeshData CoreAsset::GeometryGenerator::CreateCube(float width, float height, float depth)
{
    MeshData meshData;
    // float halfWidth = width / 2;
    // float halfHeight = height / 2;
    // float halfDepth = depth / 2;

    // std::vector<CoreAsset::MeshIndexType> indexVector = {
    //     // Front (+Z)
    //     0,
    //     2,
    //     1,
    //     0,
    //     3,
    //     2,

    //    // Back (-Z)
    //    4,
    //    5,
    //    6,
    //    4,
    //    6,
    //    7,

    //    // Left (-X)
    //    8,
    //    9,
    //    10,
    //    8,
    //    10,
    //    11,

    //    // Right (+X)
    //    12,
    //    13,
    //    14,
    //    12,
    //    14,
    //    15,

    //    // Top (+Y)
    //    16,
    //    17,
    //    18,
    //    16,
    //    18,
    //    19,

    //    // Bottom (-Y)
    //    20,
    //    22,
    //    21,
    //    20,
    //    23,
    //    22,
    //};
    // std::vector<CoreAsset::StaticVertex> vertexVector = {
    //    // Front (+Z)
    //    {{-halfWidth, -halfHeight, +halfDepth}, {0.0f, 1.0f}, {0, 0, +1}},
    //    {{-halfWidth, +halfHeight, +halfDepth}, {0.0f, 0.0f}, {0, 0, +1}},
    //    {{+halfWidth, +halfHeight, +halfDepth}, {1.0f, 0.0f}, {0, 0, +1}},
    //    {{+halfWidth, -halfHeight, +halfDepth}, {1.0f, 1.0f}, {0, 0, +1}},

    //    // Back (-Z)
    //    {{-halfWidth, -halfHeight, -halfDepth}, {0.0f, 1.0f}, {0, 0, -1}},
    //    {{-halfWidth, +halfHeight, -halfDepth}, {0.0f, 0.0f}, {0, 0, -1}},
    //    {{+halfWidth, +halfHeight, -halfDepth}, {1.0f, 0.0f}, {0, 0, -1}},
    //    {{+halfWidth, -halfHeight, -halfDepth}, {1.0f, 1.0f}, {0, 0, -1}},

    //    // Left (-X)
    //    {{-halfWidth, -halfHeight, +halfDepth}, {0.0f, 1.0f}, {-1, 0, 0}},
    //    {{-halfWidth, +halfHeight, +halfDepth}, {0.0f, 0.0f}, {-1, 0, 0}},
    //    {{-halfWidth, +halfHeight, -halfDepth}, {1.0f, 0.0f}, {-1, 0, 0}},
    //    {{-halfWidth, -halfHeight, -halfDepth}, {1.0f, 1.0f}, {-1, 0, 0}},

    //    // Right (+X)
    //    {{+halfWidth, -halfHeight, -halfDepth}, {0.0f, 1.0f}, {+1, 0, 0}},
    //    {{+halfWidth, +halfHeight, -halfDepth}, {0.0f, 0.0f}, {+1, 0, 0}},
    //    {{+halfWidth, +halfHeight, +halfDepth}, {1.0f, 0.0f}, {+1, 0, 0}},
    //    {{+halfWidth, -halfHeight, +halfDepth}, {1.0f, 1.0f}, {+1, 0, 0}},

    //    // Top (+Y)
    //    {{-halfWidth, +halfHeight, -halfDepth}, {0.0f, 1.0f}, {0, +1, 0}},
    //    {{-halfWidth, +halfHeight, +halfDepth}, {0.0f, 0.0f}, {0, +1, 0}},
    //    {{+halfWidth, +halfHeight, +halfDepth}, {1.0f, 0.0f}, {0, +1, 0}},
    //    {{+halfWidth, +halfHeight, -halfDepth}, {1.0f, 1.0f}, {0, +1, 0}},

    //    // Bottom (-Y)
    //    {{-halfWidth, -halfHeight, -halfDepth}, {0.0f, 1.0f}, {0, -1, 0}},
    //    {{-halfWidth, -halfHeight, +halfDepth}, {0.0f, 0.0f}, {0, -1, 0}},
    //    {{+halfWidth, -halfHeight, +halfDepth}, {1.0f, 0.0f}, {0, -1, 0}},
    //    {{+halfWidth, -halfHeight, -halfDepth}, {1.0f, 1.0f}, {0, -1, 0}},
    //};

    // std::vector<CoreMath::Vector3> biTangentList(vertexVector.size());

    // for (size_t i = 0; i < indexVector.size() / 3; ++i)
    //{
    //     CoreAsset::MeshIndexType index1 = indexVector[i * 3 + 0];
    //     CoreAsset::MeshIndexType index2 = indexVector[i * 3 + 1];
    //     CoreAsset::MeshIndexType index3 = indexVector[i * 3 + 2];

    //    // CaculateTangent(v1,v2,v3);
    //    CaculateTangent(vertexVector[index1].mPos, vertexVector[index2].mPos, vertexVector[index3].mPos,
    //                    vertexVector[index1].mTex, vertexVector[index2].mTex, vertexVector[index3].mTex,
    //                    vertexVector[index1].mTangent, vertexVector[index2].mTangent, vertexVector[index3].mTangent,
    //                    biTangentList[index1], biTangentList[index2], biTangentList[index3]);
    //}

    // for (size_t i = 0; i < vertexVector.size(); ++i)
    //{
    //     FinishCaculateTangent(vertexVector[i].mNormal, biTangentList[i], vertexVector[i].mTangent);
    // }

    AddCubeMeshData(width, height, depth, meshData.mVertices, meshData.mIndices);

    // meshData.mIndices = std::move(indexVector);
    //   meshData.mVertices = std::move(vertexVector);
    return meshData;
}

CoreAsset::MeshData CoreAsset::GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height,
                                                                 uint32_t sliceCount, uint32_t stackCount)
{
    MeshData meshData;
    // index
    std::vector<CoreAsset::MeshIndexType> indexVector;
    std::vector<CoreAsset::StaticVertex> vertexVector;

    uint32_t ringCount = stackCount + 1;

    float stackHeight = height / stackCount;

    // botto부터 ring이 올라갈때마다 변해야하는 반지름  변화량
    float radiusStep = (topRadius - bottomRadius) / stackCount;

    float thetaStep = CoreMath::TWO_PI / sliceCount;

    for (uint32_t i = 0; i < ringCount; ++i)
    {

        float r = bottomRadius + radiusStep * i;
        float y = -0.5f * height + stackHeight * i;
        for (uint32_t j = 0; j <= sliceCount; ++j)
        {
            StaticVertex vertex;

            float c = cosf(thetaStep * j);
            float s = sinf(thetaStep * j);

            vertex.mPos = CoreMath::Vector3(r * c, y, r * s);
            vertex.mTex = CoreMath::Vector2((float)j / sliceCount, 1.0f - (float)i / stackCount);

            vertexVector.push_back(vertex);
        }
    }

    uint32_t ringVertexCount = sliceCount + 1;
    for (uint32_t i = 0; i < stackCount; ++i)
    {
        uint32_t currRing = i * ringVertexCount;
        uint32_t nextRing = (i + 1) * ringVertexCount;
        for (uint32_t j = 0; j < sliceCount; ++j)
        {

            indexVector.push_back(currRing + j);
            indexVector.push_back(nextRing + j);
            indexVector.push_back(currRing + j + 1);

            indexVector.push_back(nextRing + j);
            indexVector.push_back(nextRing + j + 1);
            indexVector.push_back(currRing + j + 1);
        }
    }

    // bottom ring cetner
    if (bottomRadius > 0.0f)
    {
        uint32_t centerVertexIndex = vertexVector.size();

        StaticVertex bottomCenterVertex;
        bottomCenterVertex.mPos = {0, -0.5f * height, 0};
        bottomCenterVertex.mTex = {0.5f, 0.5f};

        vertexVector.push_back(bottomCenterVertex);

        float r = bottomRadius;
        for (int j = 0; j <= sliceCount; ++j)
        {
            StaticVertex vertex;

            float c = cosf(thetaStep * j);
            float s = sinf(thetaStep * j);

            vertex.mPos = CoreMath::Vector3(r * c, -0.5f * height, r * s);
            vertex.mTex = CoreMath::Vector2(c * 0.5f + 0.5f, s * 0.5f + 0.5f);

            vertexVector.push_back(vertex);
        }

        for (int j = 0; j < sliceCount; ++j)
        {
            indexVector.push_back(centerVertexIndex);
            indexVector.push_back(centerVertexIndex + j + 1);
            indexVector.push_back(centerVertexIndex + j + 2);
        }
    }
    // top ring center

    if (topRadius > 0.0f)
    {
        uint32_t centerVertexIndex = vertexVector.size();

        StaticVertex topCenterVertex;
        topCenterVertex.mPos = {0, 0.5f * height, 0};
        topCenterVertex.mTex = {0.5f, 0.5f};

        vertexVector.push_back(topCenterVertex);
        float r = bottomRadius;
        for (int j = 0; j <= sliceCount; ++j)
        {
            StaticVertex vertex;

            float c = cosf(thetaStep * j);
            float s = sinf(thetaStep * j);

            vertex.mPos = CoreMath::Vector3(r * c, 0.5f * height, r * s);
            vertex.mTex = CoreMath::Vector2(c * 0.5f + 0.5f, -s * 0.5f + 0.5f);

            vertexVector.push_back(vertex);
        }

        for (int j = 0; j < sliceCount; ++j)
        {
            indexVector.push_back(centerVertexIndex);
            indexVector.push_back(centerVertexIndex + j + 2);
            indexVector.push_back(centerVertexIndex + j + 1);
        }
    }

    CaculateVertexNoraml(vertexVector, indexVector);

    std::vector<CoreMath::Vector3> biTangentList(vertexVector.size());

    for (size_t i = 0; i < indexVector.size() / 3; ++i)
    {
        CoreAsset::MeshIndexType index1 = indexVector[i * 3 + 0];
        CoreAsset::MeshIndexType index2 = indexVector[i * 3 + 1];
        CoreAsset::MeshIndexType index3 = indexVector[i * 3 + 2];

        // CaculateTangent(v1,v2,v3);
        CaculateTangent(vertexVector[index1].mPos, vertexVector[index2].mPos, vertexVector[index3].mPos,
                        vertexVector[index1].mTex, vertexVector[index2].mTex, vertexVector[index3].mTex,
                        vertexVector[index1].mTangent, vertexVector[index2].mTangent, vertexVector[index3].mTangent,
                        biTangentList[index1], biTangentList[index2], biTangentList[index3]);
    }

    for (size_t i = 0; i < vertexVector.size(); ++i)
    {
        FinishCaculateTangent(vertexVector[i].mNormal, biTangentList[i], vertexVector[i].mTangent);
    }

    meshData.mVertices = std::move(vertexVector);
    meshData.mIndices = std::move(indexVector);
    return meshData;
}

CoreAsset::MeshData CoreAsset::GeometryGenerator::CreateArrow(float cylinderHeight)
{

    CoreAsset::MeshData cylinder = CreateCylinder(0.05, 0.05, cylinderHeight, 12, 6);

    for (auto &vertex : cylinder.mVertices)
    {
        vertex.mPos.Y += (cylinderHeight / 2);
    }

    float coneHeight = cylinderHeight / 5;
    CoreAsset::MeshData cone = CreateCylinder(0.12, 0, coneHeight, 12, 6);

    for (auto &vertex : cone.mVertices)
    {
        vertex.mPos.Y += cylinderHeight + coneHeight / 2;
    }

    uint32_t indexOffset = cylinder.mVertices.size();
    for (uint32_t &index : cone.mIndices)
    {
        index += indexOffset;
    }

    cylinder.mVertices.insert(cylinder.mVertices.end(), cone.mVertices.begin(), cone.mVertices.end());
    cylinder.mIndices.insert(cylinder.mIndices.end(), cone.mIndices.begin(), cone.mIndices.end());
    return cylinder;
}

CoreAsset::MeshData CoreAsset::GeometryGenerator::CreateArrowCube(float cylinderHeight)
{
    CoreAsset::MeshData cylinder = CreateCylinder(0.05, 0.05, cylinderHeight, 12, 6);

    for (auto &vertex : cylinder.mVertices)
    {
        vertex.mPos.Y += (cylinderHeight / 2);
    }

    float cubeSize = cylinderHeight / 5;
    CoreAsset::MeshData cube = CreateCube(0.12, 0.12, 0.12);

    for (auto &vertex : cube.mVertices)
    {
        vertex.mPos.Y += cylinderHeight + cubeSize / 2;
    }

    uint32_t indexOffset = cylinder.mVertices.size();
    for (uint32_t &index : cube.mIndices)
    {
        index += indexOffset;
    }

    cylinder.mVertices.insert(cylinder.mVertices.end(), cube.mVertices.begin(), cube.mVertices.end());
    cylinder.mIndices.insert(cylinder.mIndices.end(), cube.mIndices.begin(), cube.mIndices.end());
    return cylinder;
}

CoreAsset::MeshData CoreAsset::GeometryGenerator::CreateQuad(float radius, float sliceCount)
{

    if (sliceCount < 3)
        sliceCount = 3;

    MeshData meshData;

    std::vector<StaticVertex> vertexVec;

    float r = radius;
    float sliceStep = CoreMath::HALF_PI / sliceCount;

    StaticVertex centerVertex;
    centerVertex.mPos = {0, 0, 0};
    centerVertex.mTex = {0.5f, 0.5f};
    centerVertex.mNormal = {0, 1, 0};

    vertexVec.push_back(centerVertex);

    for (int i = 0; i <= sliceCount; ++i)
    {
        StaticVertex vertex;
        float theta = sliceStep * i;

        float c = cosf(theta);
        float s = sinf(theta);

        vertex.mPos = {c * r, 0, s * r};
        vertex.mNormal = {1, 0, 0};
        vertex.mTex = {c + 0.5f, -s + 0.5f};

        vertexVec.push_back(vertex);
    }

    std::vector<MeshIndexType> indexVec;

    for (int i = 0; i < sliceCount; ++i)
    {
        indexVec.push_back(0);
        indexVec.push_back(i + 2);
        indexVec.push_back(i + 1);
    }

    meshData.mVertices = std::move(vertexVec);
    meshData.mIndices = std::move(indexVec);

    return meshData;
}

CoreAsset::MeshData CoreAsset::GeometryGenerator::CreateSphere(float radius, int sliceCount, int stackCount)
{

    if (stackCount < 4)
        stackCount = 4;

    if (sliceCount < 3)
        sliceCount = 3;

    std::vector<StaticVertex> vertices;
    std::vector<uint32_t> indices;

    int bodyStackCount = stackCount - 2;
    int ringCount = stackCount - 1;

    int bodyRingCount = ringCount - 2;

    float thetaStep = 360.0f / sliceCount * CoreMath::PI / 180.0f;
    float stackHeightStep = 2.0f * radius / stackCount;

    int ringVertexCount = sliceCount + 1;

    // top ring
    StaticVertex topVertex;
    topVertex.mPos = {0, radius, 0};
    topVertex.mNormal = {0, 1, 0};
    //    topVertex.mTex;
    for (int i = 0; i < sliceCount; ++i)
    {
        topVertex.mTex.X = static_cast<float>(i) / sliceCount;
        topVertex.mTex.Y = 0.0f;

        vertices.push_back(topVertex);
    }

    float topRingHeight = radius - stackHeightStep;
    //   float absRingHeightLength = std::abs(topRingHeight);
    float ringRadius = std::sqrtf(radius * radius - topRingHeight * topRingHeight);

    uint32_t startVertexIndexOffset = vertices.size();
    for (int j = 0; j < sliceCount + 1; ++j)
    {

        StaticVertex vertex;
        float theta = thetaStep * j;

        float x = cosf(theta) * ringRadius;
        float z = sinf(theta) * ringRadius;

        vertex.mPos = {x, topRingHeight, z};
        vertex.mTex.X = static_cast<float>(j) / sliceCount;
        vertex.mTex.Y = 1.0f / stackCount;

        vertices.push_back(vertex);
    }

    for (int i = 0; i < sliceCount; ++i)
    {
        // 0 2 1
        uint32_t currVertexIndex = startVertexIndexOffset + i;
        uint32_t currPoleVertexIndex = currVertexIndex - sliceCount;

        indices.push_back(currPoleVertexIndex);
        indices.push_back(currVertexIndex + 1);
        indices.push_back(currVertexIndex);
    }

    // body ring

    // top ring부터 아래로
    for (int i = 0; i < bodyRingCount; ++i)
    {
        int stackIndex = i + 2;
        float ringHeight = radius - stackIndex * stackHeightStep;

        float absRingHeightLength = std::abs(ringHeight);
        float ringRadius = std::sqrtf(radius * radius - absRingHeightLength * absRingHeightLength);

        uint32_t startVertexIndexOffset = vertices.size();
        // ring
        for (int j = 0; j < sliceCount + 1; ++j)
        {

            StaticVertex vertex;

            float theta = thetaStep * j;

            float x = cosf(theta) * ringRadius;
            float z = sinf(theta) * ringRadius;

            vertex.mPos = {x, ringHeight, z};
            vertex.mTex.X = static_cast<float>(j) / sliceCount;
            vertex.mTex.Y = static_cast<float>(stackIndex) / stackCount;

            vertices.push_back(vertex);
        }

        for (int j = 0; j < sliceCount; ++j)
        {
            uint32_t currVertexIndex = j + startVertexIndexOffset;

            indices.push_back(currVertexIndex);
            indices.push_back(currVertexIndex - ringVertexCount);
            indices.push_back(currVertexIndex + 1);

            indices.push_back(currVertexIndex - ringVertexCount);
            indices.push_back(currVertexIndex - ringVertexCount + 1);
            indices.push_back(currVertexIndex + 1);
        }
    }

    // bottom ring

    float bottomRingHeight = -radius + stackHeightStep;

    ringRadius = std::sqrtf(radius * radius - bottomRingHeight * bottomRingHeight);
    startVertexIndexOffset = vertices.size();
    for (int j = 0; j < sliceCount + 1; ++j)
    {

        StaticVertex vertex;
        float theta = thetaStep * j;

        float x = cosf(theta) * ringRadius;
        float z = sinf(theta) * ringRadius;

        vertex.mPos = {x, bottomRingHeight, z};
        vertex.mTex.X = static_cast<float>(j) / sliceCount;
        vertex.mTex.Y = static_cast<float>(stackCount - 1) / stackCount;

        vertices.push_back(vertex);
    }

    for (int i = 0; i < sliceCount; ++i)
    {
        uint32_t currVertexIndex = i + startVertexIndexOffset;

        indices.push_back(currVertexIndex);
        indices.push_back(currVertexIndex - ringVertexCount);
        indices.push_back(currVertexIndex + 1);

        indices.push_back(currVertexIndex - ringVertexCount);
        indices.push_back(currVertexIndex - ringVertexCount + 1);
        indices.push_back(currVertexIndex + 1);
    }

    // bottom stack
    startVertexIndexOffset = vertices.size();
    StaticVertex bottomVertex;
    bottomVertex.mPos = {0, -radius, 0};
    bottomVertex.mNormal = {0, -1, 0};

    //    topVertex.mTex;
    for (int i = 0; i < sliceCount; ++i)
    {
        bottomVertex.mTex.X = static_cast<float>(i) / sliceCount;
        bottomVertex.mTex.Y = 1.0F;
        vertices.push_back(bottomVertex);
    }

    for (int i = 0; i < sliceCount; ++i)
    {
        uint32_t currPoleVertexIndex = startVertexIndexOffset + i;
        // 0 2 1
        indices.push_back(currPoleVertexIndex);
        indices.push_back(currPoleVertexIndex - ringVertexCount);
        indices.push_back(currPoleVertexIndex - ringVertexCount + 1);
    }

    CaculateVertexNoraml(vertices, indices);

    std::vector<CoreMath::Vector3> biTangentList(vertices.size());

    for (size_t i = 0; i < indices.size() / 3; ++i)
    {
        CoreAsset::MeshIndexType index1 = indices[i * 3 + 0];
        CoreAsset::MeshIndexType index2 = indices[i * 3 + 1];
        CoreAsset::MeshIndexType index3 = indices[i * 3 + 2];

        // CaculateTangent(v1,v2,v3);
        CaculateTangent(vertices[index1].mPos, vertices[index2].mPos, vertices[index3].mPos, vertices[index1].mTex,
                        vertices[index2].mTex, vertices[index3].mTex, vertices[index1].mTangent,
                        vertices[index2].mTangent, vertices[index3].mTangent, biTangentList[index1],
                        biTangentList[index2], biTangentList[index3]);
    }

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        FinishCaculateTangent(vertices[i].mNormal, biTangentList[i], vertices[i].mTangent);
    }

    MeshData meshData;
    meshData.mVertices = std::move(vertices);
    meshData.mIndices = std::move(indices);

    return meshData;
}

CoreAsset::MeshData CoreAsset::GeometryGenerator::CreateEditorCamera()
{

    MeshData meshData;

    SubMeshData subMeshData1, subMeshData2;

    subMeshData1.mIndexOffset = 0;
    subMeshData1.mVertexOffset = 0;

    AddCubeMeshData(1, 1, 2, meshData.mVertices, meshData.mIndices);
    subMeshData1.mIndexNum = meshData.mIndices.size();

    subMeshData2.mIndexOffset = subMeshData1.mIndexNum;
    subMeshData2.mVertexOffset = 0;
    AddCubeMeshData(0.5, 0.5, 0.5, meshData.mVertices, meshData.mIndices, {0, 0, 1.25f});
    subMeshData2.mIndexNum = meshData.mIndices.size() - subMeshData1.mIndexNum;

    meshData.mSubMeshDataList.push_back(subMeshData1);
    meshData.mSubMeshDataList.push_back(subMeshData2);

    return meshData;
}

CoreAsset::MeshData CoreAsset::GeometryGenerator::CreateSkySphere(float radius, int sliceCount, int stackCount)
{
    std::vector<StaticVertex> vertices;
    std::vector<MeshIndexType> indices;

    MeshData meshData;

    for (int i = 0; i <= stackCount; ++i)
    {
        float v = static_cast<float>(i) / stackCount;
        float phi = v * CoreMath::PI;

        float y = std::cos(phi) * radius;

        float ringRadius = std::sin(phi) * radius;
        for (int j = 0; j <= sliceCount; ++j)
        {
            StaticVertex vertex;

            float u = static_cast<float>(j) / sliceCount;
            float theta = u * CoreMath::TWO_PI;

            float x = ringRadius * cos(theta);
            float z = ringRadius * sin(theta);

            vertex.mPos = {x, y, z};
            vertex.mTex = {u, v};

            vertices.push_back(vertex);
        }
    }

    int ringVertexCount = sliceCount + 1;
    for (int i = 0; i < stackCount; ++i)
    {
        for (int j = 0; j < sliceCount; ++j)
        {
            int a = i * ringVertexCount + j;
            int b = (i + 1) * ringVertexCount + j;
            int c = (i + 1) * ringVertexCount + j + 1;
            int d = (i)*ringVertexCount  + j + 1;

            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(b);

            indices.push_back(a);
            indices.push_back(d);
            indices.push_back(c);
        }
    }

    meshData.mVertices = std::move(vertices);
    meshData.mIndices = std::move(indices);

    meshData.mSubMeshDataList.resize(1);

    meshData.mSubMeshDataList[0].mIndexNum = meshData.mIndices.size();
    meshData.mSubMeshDataList[0].mVertexOffset = 0;
    meshData.mSubMeshDataList[0].mIndexOffset = 0;

    return meshData;
}

void CoreAsset::GeometryGenerator::CaculateTangents(std::vector<StaticVertex> &vertices,
                                                    const std::vector<uint32_t> &indices)
{
    std::vector<CoreMath::Vector3> biTangentList(vertices.size());

    for (size_t i = 0; i < indices.size() / 3; ++i)
    {
        CoreAsset::MeshIndexType index1 = indices[i * 3 + 0];
        CoreAsset::MeshIndexType index2 = indices[i * 3 + 1];
        CoreAsset::MeshIndexType index3 = indices[i * 3 + 2];

        // CaculateTangent(v1,v2,v3);
        CaculateTangent(vertices[index1].mPos, vertices[index2].mPos, vertices[index3].mPos, vertices[index1].mTex,
                        vertices[index2].mTex, vertices[index3].mTex, vertices[index1].mTangent,
                        vertices[index2].mTangent, vertices[index3].mTangent, biTangentList[index1],
                        biTangentList[index2], biTangentList[index3]);
    }

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        FinishCaculateTangent(vertices[i].mNormal, biTangentList[i], vertices[i].mTangent);
    }
}
