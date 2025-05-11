#pragma once
#define FBXSDK_SHARED

#include"Predeclare.h"
#include"Asset/Mesh/MeshType.h"
#include<vector>

#include<DirectXMath.h>
#include"RawResourceType.h"
#include"fbxsdk.h"
#include"fbxsdk/fileio/fbxiosettings.h"
#include"ResourceImportCommonHeader.h"
#include"Utility/Singleton.h"

#include"Asset/Animation/BoneAnimationClip.h"
#include"Asset/Animation/AnimationClip.h"


namespace Quad
{

	class FbxParser:public Singleton<FbxParser>
	{
		enum class EAxisType
		{
			eMayZUp = 0,
			eOpenGL,
			eDirect
		};

		enum class ENodeType
		{
			eNull=0,
			eMesh,
			eBone
		};


		struct Node
		{
			ENodeType mNodeType;



		};


		struct ControlPoint
		{
			DirectX::XMFLOAT3 mPosition;
			float mBoneIndex[8] = { 0, };
			float mWeight[8] = { 0, };

			int mBoneIndexCount = 0;
		};


	public:
		FbxParser();
		~FbxParser();


		//void Initialize();
		void LoadFbxFile(const std::string & filePath, std::vector<RawMeshData*>& oRawMeshDataVector,
			std::vector<RawMaterialData>& oRawMaterialDataVector, std::vector<RawTextureData>& oRawTextureDataVector,
			std::unique_ptr<Skeleton>& oSkeleton,
			std::unique_ptr <AnimationClip>& oAnimationClip, LoadResultCommonData& loadResultCommonData);

	private:
		
		void LoadModel(FbxScene* scene, std::vector<RawMeshData*>& oRawMeshDataVector,
			std::vector<RawMaterialData>& oRawMaterialDataVector, std::vector<RawTextureData>& oRawTextureDataVector,
			Skeleton* skeleton, std::vector<BoneAnimationClip>* pBoneAnimationClipVector);
		
		void LoadControlPoints(FbxMesh* mesh ,std::vector<ControlPoint>& oControlPointVector, std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>& oRawBoundingBoxData);

		void ConstructSknningMeshDataAndAnimClip(FbxNode* node, std::vector<ControlPoint>& controlPointVector,
			 Skeleton* skeleton, std::vector<BoneAnimationClip>* pBoneAnimationClipVector, bool& oStaticMeshFlag);
		void BuildControlPointWeight(std::vector<ControlPoint>& controlPointVector, FbxCluster* cluster, int boneI);

		void BuildBoneAnimationClip(int boneID, Skeleton* skeleton, FbxNode* meshNode, FbxNode* boneNode,
			std::vector<BoneAnimationClip>* pBoneAnimationClipVector);


		void MergeSkinningMesh(std::vector<RawMeshData*>& rawMeshDataVector);



		void LoadMaterials(FbxMesh* mesh, std::vector<RawMaterialData>& oMaterialDataVector, std::vector< RawTextureData>& oTextureDataVector);
		void BuildStaticMesh(FbxMesh* mesh, const std::vector<ControlPoint>& controlPointVector,
			const std::vector<RawMaterialData>& materialVector, RawStaticMeshData* oRawMeshData);
		void BuildSkinningMesh(FbxMesh* mesh, const std::vector<ControlPoint>& controlPointVector,
			const std::vector<RawMaterialData>& materialVector, RawSkinningMeshData* oRawMeshData);

		void LoadBoneHierarchy(FbxNode* rootNode, Skeleton** oSkeleton);


		//anim methods

		void LoadAnimClip(FbxAnimLayer* animLayer, Skeleton& skeleton, AnimationClip& oAnimaitionClip);


		void AnimCurveNode(FbxAnimLayer* AnimLayer);



	private:
		//helper

		 DirectX::XMFLOAT4X4 ConvertFbxAMatrix(const FbxAMatrix& fbxAMatrix);

		 void ConvertToDirectAxisSystem(DirectX::XMFLOAT3& oPos);
		 void ConvertQauternionToDirectAxisSystem(DirectX::XMFLOAT3& oPos);

		 void ConvertMatrixToDirectAxisSystem(DirectX::XMFLOAT4X4  & matrix);

		 EAnimationTimeMode ConvertTimeMode(FbxTime::EMode timeMode) const;

		 void ConvertVertexWindingOrder(std::vector<MeshIndexType>& oIndexVector);


	private:
		FbxManager* mManager;
		FbxIOSettings* mIOSettings; 
		FbxImporter* mImporter;



		int mAnimationClipStartFrame = 0;
		int mAnimationClipEndFrame = 0;
		FbxTime::EMode mTimeMode;


		int mConvertY = 0;	
		int mConvertZ = 0;
		EAxisType mAxisType;




		LoadResultCommonData mLoadResultCommonData;
	};

}