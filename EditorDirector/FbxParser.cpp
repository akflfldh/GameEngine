#include "FbxParser.h"
#include"Utility.h"
#include"RawResourceType.h"
#include"Asset/Skeleton/Skeleton.h"
#include"Asset/Animation/Bone.h"
#include"Asset/Animation/AnimationClip.h"
#include"Asset/Animation/BoneAnimationClip.h"
#include"VertexHelper.h"


#include"ResourceManager/SkeletonManager.h"


#include"Core/BoneFactory.h"

Quad::FbxParser::FbxParser()
{

	mManager = FbxManager::Create();

	//일단 io셋팅은 디폴트로
	mIOSettings = FbxIOSettings::Create(mManager, IOSROOT);
	mManager->SetIOSettings(mIOSettings);
	mImporter = FbxImporter::Create(mManager, "");



}

Quad::FbxParser::~FbxParser()
{




}

void Quad::FbxParser::LoadFbxFile(const std::string& filePath,std::vector<RawMeshData*> & oRawMeshDataVector,
	std::vector<RawMaterialData>& oRawMaterialDataVector,std::vector<RawTextureData> & oRawTextureDataVector, std::unique_ptr<Skeleton>& oSkeleton,
	std::unique_ptr<AnimationClip>& oAnimationClip, LoadResultCommonData& loadResultCommonData)
{

	const std::string& convertedFilePath = filePath;// Utility::ConvertToString(filePath, true);
	const std::wstring filePathW = Utility::ConvertToWString(filePath,true);
	if (!mImporter->Initialize(convertedFilePath.c_str(), -1, mIOSettings))
	{

		const std::wstring & error=  Utility::ConvertToWString(mImporter->GetStatus().GetErrorString(),true);
		MessageBox(nullptr, error.c_str(), L"error", 0);
		return;
	}
	
	FbxScene* scene = FbxScene::Create(mManager, "");

	mImporter->Import(scene);
	//mImporter->Destroy();

	FbxGeometryConverter geometryConverter(mManager);
	geometryConverter.Triangulate(scene, true);

	//axis system convert 위한 작업
	FbxAxisSystem axisSystem = scene->GetGlobalSettings().GetAxisSystem();
	mAxisType = EAxisType::eDirect;
	if (axisSystem == FbxAxisSystem::MayaYUp || axisSystem == FbxAxisSystem::Motionbuilder ||axisSystem == FbxAxisSystem::OpenGL)
	{
		//z값에 -1곱
		mAxisType = EAxisType::eOpenGL;
		//FbxAxisSystem::OpenGL.ConvertScene(scene);
	}
	else if (axisSystem == FbxAxisSystem::MayaZUp || axisSystem == FbxAxisSystem::Max)
	{
		//y,z 변환
		mAxisType = EAxisType::eMayZUp;
		//FbxAxisSystem::Max.ConvertScene(scene);
	}



	//먼저 노드들과 , 계층구조구축
	Skeleton *skeleton = nullptr;
	LoadBoneHierarchy(scene->GetRootNode(),&skeleton);

	FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>();
	if (animStack)
	{
		FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>(0);
		const std::string& animClipName = animLayer->GetName();// Utility::ConvertToWString(animLayer->GetName(), true);
		AnimationClip * animationClip= new AnimationClip;
		animationClip->SetName(animClipName);
		int boneNum = skeleton->GetBoneVector().size();
		std::vector<BoneAnimationClip> boneAnimationClipVector(boneNum);

		FbxTimeSpan timeSpan = animStack->GetLocalTimeSpan();
		FbxTime startTime =	timeSpan.GetStart();
		FbxTime endTime =	timeSpan.GetStop();
		mAnimationClipStartFrame =startTime.GetFrameCount();
		mAnimationClipEndFrame =endTime.GetFrameCount();
		float animationClipEndTimeSecond = endTime.GetSecondDouble();
		animationClip->SetEndTimeSecond(animationClipEndTimeSecond);
		
		mTimeMode = startTime.GetGlobalTimeMode();
		animationClip->SetTimeMode(ConvertTimeMode(mTimeMode));
		animationClip->SetFrameEndCount(mAnimationClipEndFrame);

		//	scene->GetGlobalSettings().GetTimeMode();//혹시 다를수있으니깐
		//LoadAnimClip(animLayer, skeleton,animationClip);
		LoadModel(scene, oRawMeshDataVector, oRawMaterialDataVector, oRawTextureDataVector, skeleton,&boneAnimationClipVector);
		animationClip->SetBoneAnimationClipVector(std::move(boneAnimationClipVector));
		oAnimationClip = std::unique_ptr<AnimationClip>(animationClip);
	}
	else
	{
		//일단 static mesh만 생각해보자.
		LoadModel(scene, oRawMeshDataVector, oRawMaterialDataVector, oRawTextureDataVector,nullptr,nullptr);
		oAnimationClip = nullptr;
	}
	if (skeleton != nullptr)
	{
		oSkeleton = std::unique_ptr<Skeleton>(skeleton);
	}

}

void Quad::FbxParser::LoadModel(FbxScene * scene,std::vector<RawMeshData*> & oRawMeshDataVector,
	std::vector<RawMaterialData>& oRawMaterialDataVector ,std::vector<RawTextureData> & oRawTextureDataVector,Skeleton *  skeleton,
	std::vector<BoneAnimationClip>* pBoneAnimationClipVector)
{
	FbxNode* rootNode = scene->GetRootNode();


	std::queue<FbxNode*> nodeQueue;
	nodeQueue.push(rootNode);
	while (!nodeQueue.empty())
	{
		FbxNode* currNode = nodeQueue.front();
		nodeQueue.pop();

		FbxNodeAttribute* nodeAttribute = currNode->GetNodeAttribute();
		if (nodeAttribute)
		{
			if (nodeAttribute->GetAttributeType()== FbxNodeAttribute::EType::eMesh)
			{
				FbxMesh* mesh = (FbxMesh*)nodeAttribute;

			
				std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> rawBoundingBoxData;//min,max
				std::vector<ControlPoint> controlPointVector;
				LoadControlPoints(mesh, controlPointVector, rawBoundingBoxData);

				bool isStaticMesh = true;
				ConstructSknningMeshDataAndAnimClip(currNode, controlPointVector,skeleton, pBoneAnimationClipVector, isStaticMesh);

				//이 메시에서사용되는 머터리얼정보들만가져오게된다.
				//그메시에서유효한 머터리얼정보들을바탕으로 mesh구축
				std::vector<RawMaterialData> rawMaterialDataVector;
				LoadMaterials(mesh, rawMaterialDataVector, oRawTextureDataVector);

				RawMeshData* rawMeshData = nullptr;
				if (isStaticMesh)
				{
					RawStaticMeshData* rawStaticMeshData = new RawStaticMeshData;
					BuildStaticMesh(mesh, controlPointVector, rawMaterialDataVector, rawStaticMeshData);
					rawMeshData = rawStaticMeshData;
				}
				else
				{
					RawSkinningMeshData* rawSkinningMeshData = new RawSkinningMeshData;
					BuildSkinningMesh(mesh, controlPointVector, rawMaterialDataVector, rawSkinningMeshData);
					rawMeshData = rawSkinningMeshData;
				}
				rawMeshData->mBoundingBox = std::move(rawBoundingBoxData);
		


				oRawMeshDataVector.push_back(std::move(rawMeshData));
				//전체머터리얼vector로 삽입
				oRawMaterialDataVector.insert(oRawMaterialDataVector.end(), rawMaterialDataVector.begin(), rawMaterialDataVector.end());
			}
		}


		for (int childIndex = 0; childIndex < currNode->GetChildCount(); ++childIndex)
		{
			nodeQueue.push(currNode->GetChild(childIndex));
		}
	}
	

	//static mesh는 상관없지만  skinning mesh들이 하나의메시가아니라 여러개존재할수있다.
	//이들은 분리되어있지만 하나의 스켈레톤에 연관되어있으니 이들을 하나의 skinning mesh로 병합한다.

	MergeSkinningMesh(oRawMeshDataVector);


	//머터리얼,텍스처들은 , 한파일에있는 여러 메시들을 각각 로드하다보면 중복되는게 존재할수있다.
	//이름으로 중복을 처리	
	//이름으로 정렬하고 처리
	std::sort(oRawMaterialDataVector.begin(), oRawMaterialDataVector.end(),
		[](const RawMaterialData& matA, const RawMaterialData& matB) {  return matA.mName < matB.mName; });
	
	std::vector<RawMaterialData>::iterator newMatEndIt =std::unique(oRawMaterialDataVector.begin(), oRawMaterialDataVector.end(),
		[](const RawMaterialData& matA, const RawMaterialData& matB) { return matA.mName == matB.mName; });

	oRawMaterialDataVector.erase(newMatEndIt, oRawMaterialDataVector.end());


	//texture
	std::sort(oRawTextureDataVector.begin(), oRawTextureDataVector.end(),
		[](const RawTextureData& texA, const RawTextureData& texB) {  return texA.mFilePath < texB.mFilePath; });

	std::vector<RawTextureData>::iterator newTexEndIt = std::unique(oRawTextureDataVector.begin(), oRawTextureDataVector.end(),
		[](const RawTextureData& texA, const RawTextureData& texB){ return texA.mFilePath == texB.mFilePath; });

	oRawTextureDataVector.erase(newTexEndIt, oRawTextureDataVector.end());









}

void Quad::FbxParser::LoadControlPoints(FbxMesh* mesh, std::vector<ControlPoint>& oControlPointVector,std::pair<DirectX::XMFLOAT3,DirectX::XMFLOAT3> & oRawBoundingBoxData)
{

	int controlPointsCount =  mesh->GetControlPointsCount();
	oControlPointVector.resize(controlPointsCount);

	FbxVector4 * controlPointArray = mesh->GetControlPoints();

	oControlPointVector[0].mPosition.x= controlPointArray[0].mData[0];
	oControlPointVector[0].mPosition.y= controlPointArray[0].mData[1];
	oControlPointVector[0].mPosition.z= controlPointArray[0].mData[2];
	ConvertToDirectAxisSystem(oControlPointVector[0].mPosition);

	DirectX::XMFLOAT3 minPos = oControlPointVector[0].mPosition;
	DirectX::XMFLOAT3 maxPos = oControlPointVector[0].mPosition;


	for (int i = 1; i < controlPointsCount; ++i)
	{
		oControlPointVector[i].mPosition.x =controlPointArray[i].mData[0];
		oControlPointVector[i].mPosition.y =controlPointArray[i].mData[1];
		oControlPointVector[i].mPosition.z =controlPointArray[i].mData[2];

		ConvertToDirectAxisSystem(oControlPointVector[i].mPosition);

		minPos.x = min(minPos.x, oControlPointVector[i].mPosition.x);
		minPos.y = min(minPos.y, oControlPointVector[i].mPosition.y);
		minPos.z = min(minPos.z, oControlPointVector[i].mPosition.z);

		maxPos.x = max(maxPos.x, oControlPointVector[i].mPosition.x);
		maxPos.y = max(maxPos.y, oControlPointVector[i].mPosition.y);
		maxPos.z = max(maxPos.z, oControlPointVector[i].mPosition.z);
	}

	oRawBoundingBoxData.first = minPos;
	oRawBoundingBoxData.second = maxPos;
	return;
}

void Quad::FbxParser::ConstructSknningMeshDataAndAnimClip(FbxNode* node, std::vector<ControlPoint>& controlPointVector,
	 Skeleton * skeleton, std::vector<BoneAnimationClip>* pBoneAnimationClipVector,bool & oStaticMeshFlag)
{
	oStaticMeshFlag = false;
	FbxMesh* mesh = FbxCast<FbxMesh>(node->GetNodeAttribute());
	int deformerNum = mesh->GetDeformerCount();

//	FbxAMatrix geometryTransformMatrix = GetGeometryTransformation(node);


	for (int deformerIndex = 0; deformerIndex < deformerNum; ++deformerIndex)
	{
		
		FbxSkin* skinDeformer = FbxCast<FbxSkin>(mesh->GetDeformer(deformerIndex));
		if (skinDeformer == nullptr)
			continue;


		int clusterNum = skinDeformer->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < clusterNum; ++clusterIndex)
		{
			FbxCluster * cluster= skinDeformer->GetCluster(clusterIndex);
			FbxNode* linkNode = cluster->GetLink();

		//	const std::wstring & boneName =	Utility::ConvertToWString(linkNode->GetName(),true);
			const std::string& boneName = linkNode->GetName();
			int boneID = skeleton->GetBoneID(boneName);
			if (boneID == -1)
			{
				OutputDebugString(L"Cluster의 LinkNode가 skeleton계층구조에 없는 bone");
				assert(0);
			}

			BuildControlPointWeight(controlPointVector, cluster, boneID);
			
			//offset matrix 
			FbxAMatrix transformMatrix, transformLinkMatrix;
			cluster->GetTransformMatrix(transformMatrix);		//
			cluster->GetTransformLinkMatrix(transformLinkMatrix);
			FbxAMatrix offsetMatrix = transformLinkMatrix.Inverse() * transformMatrix;
			//적절한좌표계로의 변환처리를해줘야한다.(maya, direct, opengi)


			DirectX::XMFLOAT4X4 offsetMatarixF = ConvertFbxAMatrix(offsetMatrix);
			ConvertMatrixToDirectAxisSystem(offsetMatarixF);

			skeleton->SetOffsetMatrix(boneID, offsetMatarixF);
			
			//지금은 메시자체가에니메이션되는경우는 여기서 생각안한다.
			BuildBoneAnimationClip(boneID, skeleton, node,linkNode, pBoneAnimationClipVector);
			



		}
		
	}


	if (deformerNum == 0)
		oStaticMeshFlag = true;






}

void Quad::FbxParser::BuildControlPointWeight(std::vector<ControlPoint>& controlPointVector, FbxCluster* cluster,int boneID)
{
	//클러스터에 영향을 받는 컨트롤포인트들
	int clusterControlPointNum = cluster->GetControlPointIndicesCount();
	int* clusterControlPointIndexArray = cluster->GetControlPointIndices();
	double* clusterControlPointWeightArray = cluster->GetControlPointWeights();

	for(int i=0; i< clusterControlPointNum; ++i)
	{ 
		int controlPointIndex = clusterControlPointIndexArray[i];
		double controlPointWeight = clusterControlPointWeightArray[i];
	
		int & boneIndexCount = controlPointVector[controlPointIndex].mBoneIndexCount;
		controlPointVector[controlPointIndex].mBoneIndex[boneIndexCount] = boneID;
		controlPointVector[controlPointIndex].mWeight[boneIndexCount] = controlPointWeight;
		boneIndexCount++;
	}


}

void Quad::FbxParser::BuildBoneAnimationClip(int boneID, Skeleton* skeleton, FbxNode* meshNode , FbxNode * boneNode ,
	std::vector<BoneAnimationClip>* pBoneAnimationClipVector)
{
	FbxVector4 lT = meshNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 lR = meshNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 lS = meshNode->GetGeometricScaling(FbxNode::eSourcePivot);
	FbxAMatrix geometryMatrix(lT, lR, lS);
	FbxAMatrix A; 
	A.SetIdentity();
	A[2][2] = -1.0;

	std::vector<AnimationKeyFrame> keyFrameVector(mAnimationClipEndFrame+1);
	for (int frameCount = 0; frameCount <= mAnimationClipEndFrame; ++frameCount)
	{
		FbxTime time;
		time.SetFrame(frameCount, mTimeMode);
		FbxAMatrix currentTransformOffset = meshNode->EvaluateGlobalTransform(time) * geometryMatrix;//월드공간에 로컬공간으로 되돌아간것,
		FbxAMatrix boneAnimationKeyFrameWorldTransform = currentTransformOffset.Inverse() * boneNode->EvaluateGlobalTransform(time);

		switch(mAxisType)
		{
		case EAxisType::eMayZUp:

		{


		}

		break;
		case EAxisType::eOpenGL:
		{

			boneAnimationKeyFrameWorldTransform = A* boneAnimationKeyFrameWorldTransform* A;

		}
		break;

		}



		FbxVector4    t = boneAnimationKeyFrameWorldTransform.GetT();
		FbxQuaternion q = boneAnimationKeyFrameWorldTransform.GetQ();
		FbxVector4    s = boneAnimationKeyFrameWorldTransform.GetS();

		keyFrameVector[frameCount].mPosition = { (float)t[0], (float)t[1], (float)t[2] };
		keyFrameVector[frameCount].mQuaternion = { (float)q[0], (float)q[1], (float)q[2], (float)q[3] };
		keyFrameVector[frameCount].mScale = { (float)s[0], (float)s[1], (float)s[2] };
		keyFrameVector[frameCount].mTime = time.GetSecondDouble();



	}
	
	(*pBoneAnimationClipVector)[boneID].SetAnimationKeyFrameVector(std::move(keyFrameVector));




}

void Quad::FbxParser::MergeSkinningMesh(std::vector<RawMeshData*>& rawMeshDataVector)
{
	
	std::vector<RawSkinningMeshData*> rawSkinningMeshDataVector;
	for (int i = 0; i < rawMeshDataVector.size(); ++i)
	{
		if (rawMeshDataVector[i]->mMeshType == EMeshType::eSknningMesh)
		{
			RawSkinningMeshData* rawSkinningMeshData = static_cast<RawSkinningMeshData*>(rawMeshDataVector[i]);
			rawSkinningMeshDataVector.push_back(rawSkinningMeshData);
		}
	}

	//하나이거나 없으면 병합할이유가없음
	if (rawSkinningMeshDataVector.size() <= 1)
		return;


	std::vector<RawSubMeshData> mergeSubMeshDataVector;

	
	for (int i = 0; i < rawSkinningMeshDataVector.size(); ++i)
	{
		//어떤 머터리얼사용하는지 파악한다.
		for (int subMeshIndex = 0; subMeshIndex < rawSkinningMeshDataVector[i]->mRawSubMeshVector.size(); ++subMeshIndex)
		{
			const std::string materialName = rawSkinningMeshDataVector[i]->mRawSubMeshVector[subMeshIndex].mMaterialName;

			std::vector<RawSubMeshData>::iterator subMeshDataIterator =
			std::find_if(mergeSubMeshDataVector.begin(), mergeSubMeshDataVector.end(),
				[&materialName](const RawSubMeshData& subMeshData) { return subMeshData.mName == materialName ? true : false; });
			
			if (subMeshDataIterator == mergeSubMeshDataVector.end())
			{
				//추가
				RawSubMeshData subMeshData;
				subMeshData.mName = materialName;
				mergeSubMeshDataVector.push_back(subMeshData);
			}
		

		}
	}

	std::vector<std::vector<SkinningVertex>> skinningVertexVectorPerMaterial(mergeSubMeshDataVector.size());
	std::vector<std::vector<MeshIndexType>> skinningIndexVectorPerMaterial(mergeSubMeshDataVector.size());

	for (int i = 0; i < rawSkinningMeshDataVector.size(); ++i)
	{
		for (int subMeshIndex = 0; subMeshIndex < rawSkinningMeshDataVector[i]->mRawSubMeshVector.size(); ++subMeshIndex)
		{
			const std::string materialName = rawSkinningMeshDataVector[i]->mRawSubMeshVector[subMeshIndex].mMaterialName;
			size_t vertexOffset = rawSkinningMeshDataVector[i]->mRawSubMeshVector[subMeshIndex].mVertexOffset;
			size_t vertexEndIndex = rawSkinningMeshDataVector[i]->mVertexVector.size();
			if (subMeshIndex != (rawSkinningMeshDataVector[i]->mRawSubMeshVector.size() - 1))
			{
				vertexEndIndex = rawSkinningMeshDataVector[i]->mRawSubMeshVector[subMeshIndex + 1].mVertexOffset;
			}
			size_t indexStart = rawSkinningMeshDataVector[i]->mRawSubMeshVector[subMeshIndex].mIndexFirst;
			size_t indexEnd = rawSkinningMeshDataVector[i]->mRawSubMeshVector[subMeshIndex].mIndexLast;

			std::vector<SkinningVertex>& vertexVector = rawSkinningMeshDataVector[i]->mVertexVector;
			std::vector<MeshIndexType>& indexVector = rawSkinningMeshDataVector[i]->mIndexVector;



			std::vector<RawSubMeshData>::iterator subMeshDataIterator =
				std::find_if(mergeSubMeshDataVector.begin(), mergeSubMeshDataVector.end(),
					[&materialName](const RawSubMeshData& subMeshData) { return subMeshData.mName == materialName ? true : false; });

			int subMeshDataIndex = subMeshDataIterator - mergeSubMeshDataVector.begin();



			//정점,인덱스를 각 material별 버퍼에 복사.
			std::vector<SkinningVertex>& currentSkinningVertexVector = skinningVertexVectorPerMaterial[subMeshDataIndex];
			size_t  newVertexOffset = currentSkinningVertexVector.size();

			//vertex
			currentSkinningVertexVector.insert(currentSkinningVertexVector.end(), vertexVector.begin() + vertexOffset, vertexVector.begin() + vertexEndIndex);

			//index
			std::vector<MeshIndexType>& currentSkinningIndexVector = skinningIndexVectorPerMaterial[subMeshDataIndex];
			//인덱스값은 먼저 증가시킨다.
			std::for_each(indexVector.begin() + indexStart,
				indexVector.begin() + indexEnd, [&newVertexOffset](MeshIndexType& index) { index += newVertexOffset; });
			//복사
			currentSkinningIndexVector.insert(currentSkinningIndexVector.end(), indexVector.begin() + indexStart, 
				indexVector.begin() + indexEnd);
		
		}
	}


	//완전히 병합
	size_t totalVertexNum = 0;
	MeshIndexType totalIndexNum = 0;

	std::for_each(skinningVertexVectorPerMaterial.begin(), skinningVertexVectorPerMaterial.end(),
		[&totalVertexNum](const std::vector<SkinningVertex>& vertexVector) { totalVertexNum += vertexVector.size();  });

	std::for_each(skinningIndexVectorPerMaterial.begin(), skinningIndexVectorPerMaterial.end(),
		[&totalIndexNum](const std::vector<MeshIndexType>& indexVector) { totalIndexNum += indexVector.size();  });


	std::vector<SkinningVertex> mergeSkinningVertexVector(totalVertexNum);
	std::vector<MeshIndexType> mergeSkinningIndexVector(totalIndexNum);


	size_t vertexOffset = 0;
	size_t indexStart = 0;
	size_t indexEnd = 0;
	//복사 + subMeshData구축
	for (int vectorIndex = 0; vectorIndex < mergeSubMeshDataVector.size(); ++vectorIndex)
	{
		mergeSubMeshDataVector[vectorIndex].mVertexOffset = vertexOffset;
		mergeSubMeshDataVector[vectorIndex].mIndexFirst = indexStart;

		mergeSubMeshDataVector[vectorIndex].mIndexLast = indexStart+ skinningIndexVectorPerMaterial[vectorIndex].size();
		//mergeSubMeshDataVector[vectorIndex].mBoundingBox;

		std::copy(mergeSkinningVertexVector.end(), skinningVertexVectorPerMaterial[vectorIndex].begin(),
			skinningVertexVectorPerMaterial[vectorIndex].end());

		std::copy(mergeSkinningIndexVector.end(), skinningIndexVectorPerMaterial[vectorIndex].begin(),
			skinningIndexVectorPerMaterial[vectorIndex].end());

		vertexOffset += skinningVertexVectorPerMaterial[vectorIndex].size();
		indexStart = mergeSubMeshDataVector[vectorIndex].mIndexLast;

	}

	//기존의 skinningMeshData 제거
	for(std::vector<RawMeshData*>::iterator it =rawMeshDataVector.begin(); it !=rawMeshDataVector.end(); --it)
	{
		if ((*it)->mMeshType == EMeshType::eSknningMesh)
		{
			delete (*it);
			rawMeshDataVector.erase(it);
		}
	}

	RawSkinningMeshData* mergeRawSkinningMeshData = new RawSkinningMeshData;

	mergeRawSkinningMeshData->mRawSubMeshVector = std::move(mergeSubMeshDataVector);
	mergeRawSkinningMeshData->mVertexVector = std::move(mergeSkinningVertexVector);
	mergeRawSkinningMeshData->mIndexVector = std::move(mergeSkinningIndexVector);
	mergeRawSkinningMeshData->mName ="test";
	
	rawMeshDataVector.push_back(mergeRawSkinningMeshData);




}

void Quad::FbxParser::LoadMaterials(FbxMesh* mesh, std::vector<RawMaterialData>& oMaterialDataVector,
	std::vector< RawTextureData> & oTextureDataVector)
{
	FbxNode* node = mesh->GetNode();

	int materialCount  = node->GetMaterialCount();

	if (materialCount == 0)
	{
		//기본재질설정
		RawMaterialData rawMaterialData;
		rawMaterialData.mName = "Default";//기본재질에는 기본diffuseMap이있다.
		rawMaterialData.mDefaultFlag = true;
		oMaterialDataVector.push_back(rawMaterialData);


	}


	for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
	{
		RawMaterialData rawMaterialData;

		FbxSurfaceMaterial * material =	node->GetMaterial(materialIndex);
		bool lambert =material->Is<FbxSurfaceLambert>();
		bool phong =  material->Is<FbxSurfacePhong>();


		FbxSurfaceLambert* lambertMaterial = (FbxSurfaceLambert*)material;
		FbxString modelName = material->ShadingModel.Get();
		//default는 lambert
		double diffuseFactor = lambertMaterial->DiffuseFactor.Get();
		//rawMaterialData.mDiffuse.x = lambertMaterial->Diffuse.Get().mData[0]* diffuseFactor;
		//rawMaterialData.mDiffuse.y = lambertMaterial->Diffuse.Get().mData[1]* diffuseFactor;
		//rawMaterialData.mDiffuse.z = lambertMaterial->Diffuse.Get().mData[2]* diffuseFactor;

		double ambientFactor = lambertMaterial->AmbientFactor;
		rawMaterialData.mAmbient.x = lambertMaterial->Ambient.Get().mData[0] * ambientFactor;
		rawMaterialData.mAmbient.y = lambertMaterial->Ambient.Get().mData[1] * ambientFactor;
		rawMaterialData.mAmbient.z = lambertMaterial->Ambient.Get().mData[2] * ambientFactor;


		if (material->ShadingModel.Get() == "Phong")
		{
			FbxSurfacePhong* phongMaterial = (FbxSurfacePhong*)material;
			double specularFactor = phongMaterial->SpecularFactor;
			rawMaterialData.mSpecular.x = phongMaterial->Specular.Get().mData[0] * specularFactor;
			rawMaterialData.mSpecular.y = phongMaterial->Specular.Get().mData[1] * specularFactor;
			rawMaterialData.mSpecular.z = phongMaterial->Specular.Get().mData[2] * specularFactor;

			rawMaterialData.mShiness = phongMaterial->Shininess;
		}

		//텍스처가 겹칠수있다.
		FbxFileTexture * diffuseMap = lambertMaterial->Diffuse.GetSrcObject<FbxFileTexture>(0);
		FbxFileTexture* normalMap = lambertMaterial->NormalMap.GetSrcObject<FbxFileTexture>(0);

		if (diffuseMap != nullptr)
		{
			RawTextureData rawTextureData;
			rawTextureData.mFilePath = diffuseMap->GetFileName();// Utility::ConvertToWString(diffuseMap->GetFileName(), true);
			rawMaterialData.mDiffuseMapName = Utility::GetFileNameFromPath(rawTextureData.mFilePath);
			oTextureDataVector.push_back(std::move(rawTextureData));

		}
		if (normalMap != nullptr)
		{
			RawTextureData rawTextureData;
			rawTextureData.mFilePath = normalMap->GetFileName();// Utility::ConvertToWString(normalMap->GetFileName(), true);
			rawMaterialData.mNormalMapName = Utility::GetFileNameFromPath(rawTextureData.mFilePath);
			oTextureDataVector.push_back(std::move(rawTextureData));
		}
		rawMaterialData.mName = material->GetName();// Utility::ConvertToWString(material->GetName(), true);
		oMaterialDataVector.push_back(std::move(rawMaterialData));

	}





	return;

}

void Quad::FbxParser::BuildStaticMesh(FbxMesh* mesh, const std::vector<ControlPoint>& controlPointVector,
	const std::vector<RawMaterialData> & materialVector,RawStaticMeshData  * oRawMeshData)
{


	//load controlPointVector
	FbxNode* node = mesh->GetNode();
	oRawMeshData->mName = node->GetName();// Utility::ConvertToWString(node->GetName(), true);
	//subMesh 구축되는데 당연히 material별로 나뉜다.
	int subMeshNum = materialVector.size();
	std::vector<std::vector<StaticVertex>> staticVertexVectorPerSubMeshVector(subMeshNum);
	std::vector<std::vector<MeshIndexType>> staticIndexVectorPerSubMeshVector(subMeshNum);


	int polygonCount = mesh->GetPolygonCount();
	int vertexCount = mesh->GetPolygonVertexCount();
	int *controlPointIndexArray = mesh->GetPolygonVertices();

	FbxLayer* layer = mesh->GetLayer(0);

	//material
	FbxLayerElementMaterial * layerElementMaterial = layer->GetMaterials();
	FbxLayerElementMaterial::EMappingMode materialMappingMode = FbxLayerElementMaterial::EMappingMode::eAllSame;
	if(layerElementMaterial!=nullptr)
		layerElementMaterial->GetMappingMode();

	//diffuse layer
	FbxLayerElementUV* layerElementDiffuseUV = layer->GetUVs(FbxLayerElement::EType::eTextureDiffuse);
	FbxLayerElement::EMappingMode mappingModeDiffuseUV= FbxLayerElement::EMappingMode::eByControlPoint;;
	FbxLayerElement::EReferenceMode  referenceModeDiffuseUV = FbxLayerElement::EReferenceMode::eDirect;
	if (layerElementDiffuseUV != nullptr)
	{
		mappingModeDiffuseUV = layerElementDiffuseUV->GetMappingMode();
		referenceModeDiffuseUV = layerElementDiffuseUV->GetReferenceMode();
	}

	//normal layer
	FbxLayerElementNormal* layerElementNormal = layer->GetNormals();
	FbxLayerElement::EMappingMode mappingModeNormal= FbxLayerElement::EMappingMode::eByControlPoint;
	FbxLayerElement::EReferenceMode referenceModeNormal=  FbxLayerElement::EReferenceMode::eDirect;
	if (layerElementNormal != nullptr)
	{
		mappingModeNormal = layerElementNormal->GetMappingMode();
		referenceModeNormal = layerElementNormal->GetReferenceMode();
	}

	/*int indexOrder[3] = { 0,1,2 };
	switch(mAxisType)
	{
	case EAxisType::eMayZUp:;
	case EAxisType::eOpenGL:
		indexOrder[1] = 2;
		indexOrder[2] =1;
		break;
	}*/




	for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
	{
		//일반적으로 material은 메시전체 또는 폴리곤단위 중 하나라고 가정한다.
		int materialIndex = 0;

		if (layerElementMaterial)
		{
			switch (materialMappingMode)
			{
			case FbxLayerElement::EMappingMode::eAllSame:
			{
				materialIndex = layerElementMaterial->GetIndexArray()[0];
			}
			break;
			case FbxLayerElement::EMappingMode::eByPolygon:
			{
				materialIndex = layerElementMaterial->GetIndexArray()[polygonIndex];
			}
			break;
			}
		}

		std::vector<StaticVertex>& vertexVectorPerSubMesh = staticVertexVectorPerSubMeshVector[materialIndex];
		std::vector<MeshIndexType>& indexVectorPerSubMesh = staticIndexVectorPerSubMeshVector[materialIndex];


		for (int vertexIndex = 0; vertexIndex <3;  ++vertexIndex)
		{
			//int vertexIndex = indexOrder[vertexIndexCount];
			StaticVertex vertex;
			int controlPointIndex = controlPointIndexArray[polygonIndex * 3 + vertexIndex];
			vertex.mPos = controlPointVector[controlPointIndex].mPosition;


			//tex
			
			if (layerElementDiffuseUV != nullptr)
			{
				DirectX::XMFLOAT2 uv = { 0,0 };
				switch (mappingModeDiffuseUV)
				{
				case FbxLayerElement::EMappingMode::eByControlPoint:
				{
					if (referenceModeDiffuseUV == FbxLayerElement::EReferenceMode::eDirect)
					{
						uv.x = layerElementDiffuseUV->GetDirectArray()[controlPointIndex].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[controlPointIndex].mData[1];
					}
					else
					{
						int index = layerElementDiffuseUV->GetIndexArray()[controlPointIndex];
						uv.x = layerElementDiffuseUV->GetDirectArray()[index].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[index].mData[1];

					}

				}
				break;
				case FbxLayerElement::EMappingMode::eByPolygonVertex:
				{
					if (referenceModeDiffuseUV == FbxLayerElement::EReferenceMode::eDirect)
					{
						uv.x= layerElementDiffuseUV->GetDirectArray()[polygonIndex * 3 + vertexIndex].mData[0];
						uv.y= layerElementDiffuseUV->GetDirectArray()[polygonIndex * 3 + vertexIndex].mData[1];
					}
					else
					{
						int index = layerElementDiffuseUV->GetIndexArray()[polygonIndex * 3 + vertexIndex];
						uv.x = layerElementDiffuseUV->GetDirectArray()[index].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[index].mData[1];
					
					}
				}
				break;
				case FbxLayerElement::EMappingMode::eByPolygon:
				{
					if (referenceModeDiffuseUV == FbxLayerElement::EReferenceMode::eDirect)
					{
						uv.x = layerElementDiffuseUV->GetDirectArray()[polygonIndex].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[polygonIndex].mData[1];
					}
					else
					{
						int index = layerElementDiffuseUV->GetIndexArray()[polygonIndex];
						uv.x = layerElementDiffuseUV->GetDirectArray()[index].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[index].mData[1];
					}


				}
				break;
				}

				uv.y = 1.0f - uv.y;
				vertex.mTex = uv;
			}


			//normal
			if (layerElementNormal != nullptr)
			{
				DirectX::XMFLOAT3 normal = { 0,0,0 };
				switch (mappingModeNormal)
				{
				case FbxLayerElement::EMappingMode::eByControlPoint:
				{
					if (referenceModeNormal == FbxLayerElement::EReferenceMode::eDirect)
					{
						normal.x = layerElementNormal->GetDirectArray()[controlPointIndex].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[controlPointIndex].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[controlPointIndex].mData[2];
					}
					else
					{
						int index = layerElementNormal->GetIndexArray()[controlPointIndex];
						normal.x = layerElementNormal->GetDirectArray()[index].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[index].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[index].mData[2];

					}

				}
				break;
				case FbxLayerElement::EMappingMode::eByPolygonVertex:
				{
					if (referenceModeNormal == FbxLayerElement::EReferenceMode::eDirect)
					{
						normal.x = layerElementNormal->GetDirectArray()[polygonIndex * 3 + vertexIndex].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[polygonIndex * 3 + vertexIndex].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[polygonIndex * 3 + vertexIndex].mData[2];
					}
					else
					{
						int index = layerElementNormal->GetIndexArray()[polygonIndex * 3 + vertexIndex];
						normal.x = layerElementNormal->GetDirectArray()[index].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[index].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[index].mData[2];
					

					}
				}
				break;
				case FbxLayerElement::EMappingMode::eByPolygon:
				{
					if (referenceModeDiffuseUV == FbxLayerElement::EReferenceMode::eDirect)
					{
						normal.x = layerElementNormal->GetDirectArray()[polygonIndex].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[polygonIndex].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[polygonIndex].mData[2];
					}
					else
					{
						int index = layerElementNormal->GetIndexArray()[polygonIndex];
						normal.x = layerElementNormal->GetDirectArray()[index].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[index].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[index].mData[2];
					}


				}
				break;
				}

				vertex.mNormal = normal;
				ConvertToDirectAxisSystem(vertex.mNormal);
			}


			//vertex가 중복되는가?
			//그렇다면 vertex는 재추가되지않고 그 해당vertex의 인덱스값만 추가된다.
			std::vector<StaticVertex>::iterator it=
				std::find(vertexVectorPerSubMesh.begin(), vertexVectorPerSubMesh.end(), vertex);
			int index = 0;
			if (it == vertexVectorPerSubMesh.end())
			{
				index = vertexVectorPerSubMesh.size();
				vertexVectorPerSubMesh.push_back(vertex);
			}
			else
			{
				index = it - vertexVectorPerSubMesh.begin();
			}
			indexVectorPerSubMesh.push_back(index);
		}
	}




	//노멀이없었다면


	//for (int i = 0; i < staticIndexVectorPerSubMeshVector.size(); ++i)
	//	ConvertVertexWindingOrder(staticIndexVectorPerSubMeshVector[i]);






	//submesh, mesh구축
	int vertexOffset = 0;
	MeshIndexType indexFirst = 0;


	std::vector<RawSubMeshData> rawSubMeshVector(subMeshNum);
	for (int subMeshIndex = 0; subMeshIndex < subMeshNum; subMeshIndex++)
	{
		rawSubMeshVector[subMeshIndex].mVertexOffset = vertexOffset;
		rawSubMeshVector[subMeshIndex].mIndexFirst = indexFirst;
		rawSubMeshVector[subMeshIndex].mIndexLast = indexFirst + staticIndexVectorPerSubMeshVector[subMeshIndex].size();
		rawSubMeshVector[subMeshIndex].mMaterialName = materialVector[subMeshIndex].mName;


		vertexOffset += staticVertexVectorPerSubMeshVector[subMeshIndex].size();
		indexFirst = rawSubMeshVector[subMeshIndex].mIndexLast;


		for (int indexOrder = 0; indexOrder < staticIndexVectorPerSubMeshVector[subMeshIndex].size(); indexOrder += 3)
		{
			std::swap(staticIndexVectorPerSubMeshVector[subMeshIndex][indexOrder + 1], staticIndexVectorPerSubMeshVector[subMeshIndex][indexOrder + 2]);
		}



		oRawMeshData->mVertexVector.insert(oRawMeshData->mVertexVector.end(), staticVertexVectorPerSubMeshVector[subMeshIndex].begin(),
			staticVertexVectorPerSubMeshVector[subMeshIndex].end());
		oRawMeshData->mIndexVector.insert(oRawMeshData->mIndexVector.end(), staticIndexVectorPerSubMeshVector[subMeshIndex].begin(),
			staticIndexVectorPerSubMeshVector[subMeshIndex].end());


		
	}


	if (layerElementNormal == nullptr)
	{
		//노멀을 직접계산한다.
		VertexHelper::CaculateStaticMeshNormalVector(oRawMeshData->mVertexVector, oRawMeshData->mIndexVector);
	
	
	
	}


	oRawMeshData->mRawSubMeshVector = std::move(rawSubMeshVector);


}

void Quad::FbxParser::BuildSkinningMesh(FbxMesh* mesh, const std::vector<ControlPoint>& controlPointVector, const std::vector<RawMaterialData>& materialVector, RawSkinningMeshData* oRawMeshData)
{

	//load controlPointVector
	FbxNode* node = mesh->GetNode();
	oRawMeshData->mName = node->GetName();// Utility::ConvertToWString(node->GetName(), true);
	//subMesh 구축되는데 당연히 material별로 나뉜다.
	int subMeshNum = materialVector.size();
	std::vector<std::vector<SkinningVertex>> skinningVertexVectorPerSubMeshVector(subMeshNum);
	std::vector<std::vector<MeshIndexType>> skinningIndexVectorPerSubMeshVector(subMeshNum);


	int polygonCount = mesh->GetPolygonCount();
	int vertexCount = mesh->GetPolygonVertexCount();
	int* controlPointIndexArray = mesh->GetPolygonVertices();

	FbxLayer* layer = mesh->GetLayer(0);

	//material
	FbxLayerElementMaterial* layerElementMaterial = layer->GetMaterials();
	FbxLayerElementMaterial::EMappingMode materialMappingMode = layerElementMaterial->GetMappingMode();

	//diffuse layer
	FbxLayerElementUV* layerElementDiffuseUV = layer->GetUVs(FbxLayerElement::EType::eTextureDiffuse);
	FbxLayerElement::EMappingMode mappingModeDiffuseUV = layerElementDiffuseUV->GetMappingMode();
	FbxLayerElement::EReferenceMode  referenceModeDiffuseUV = layerElementDiffuseUV->GetReferenceMode();

	//normal layer
	FbxLayerElementNormal* layerElementNormal = layer->GetNormals();
	FbxLayerElement::EMappingMode mappingModeNormal = layerElementNormal->GetMappingMode();
	FbxLayerElement::EReferenceMode referenceModeNormal = layerElementNormal->GetReferenceMode();


	for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
	{
		//일반적으로 material은 메시전체 또는 폴리곤단위 중 하나라고 가정한다.
		int materialIndex = 0;
		switch (materialMappingMode)
		{
		case FbxLayerElement::EMappingMode::eAllSame:
		{
			materialIndex = layerElementMaterial->GetIndexArray()[0];
		}
		break;
		case FbxLayerElement::EMappingMode::eByPolygon:
		{
			materialIndex = layerElementMaterial->GetIndexArray()[polygonIndex];
		}
		break;
		}


		std::vector<SkinningVertex>& vertexVectorPerSubMesh = skinningVertexVectorPerSubMeshVector[materialIndex];
		std::vector<MeshIndexType>& indexVectorPerSubMesh = skinningIndexVectorPerSubMeshVector[materialIndex];


		for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
		{
			SkinningVertex vertex;
			int controlPointIndex = controlPointIndexArray[polygonIndex * 3 + vertexIndex];
			vertex.mPos = controlPointVector[controlPointIndex].mPosition;

			for (int i = 0; i < 2; ++i)
			{
				vertex.mBoneIndex[i].x = controlPointVector[controlPointIndex].mBoneIndex[i * 4 + 0];
				vertex.mBoneIndex[i].y = controlPointVector[controlPointIndex].mBoneIndex[i * 4 + 1];
				vertex.mBoneIndex[i].z = controlPointVector[controlPointIndex].mBoneIndex[i * 4 + 2];
				vertex.mBoneIndex[i].w = controlPointVector[controlPointIndex].mBoneIndex[i * 4 + 3];

				vertex.mBoneWeight[i].x = controlPointVector[controlPointIndex].mWeight[i * 4 +0];
				vertex.mBoneWeight[i].y = controlPointVector[controlPointIndex].mWeight[i * 4 +1];
				vertex.mBoneWeight[i].z = controlPointVector[controlPointIndex].mWeight[i * 4 +2];
				vertex.mBoneWeight[i].w = controlPointVector[controlPointIndex].mWeight[i * 4 +3];
			}


			//tex

			if (layerElementDiffuseUV != nullptr)
			{
				DirectX::XMFLOAT2 uv;
				switch (mappingModeDiffuseUV)
				{
				case FbxLayerElement::EMappingMode::eByControlPoint:
				{
					if (referenceModeDiffuseUV == FbxLayerElement::EReferenceMode::eDirect)
					{
						uv.x = layerElementDiffuseUV->GetDirectArray()[controlPointIndex].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[controlPointIndex].mData[1];
					}
					else
					{
						int index = layerElementDiffuseUV->GetIndexArray()[controlPointIndex];
						uv.x = layerElementDiffuseUV->GetDirectArray()[index].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[index].mData[1];

					}

				}
				break;
				case FbxLayerElement::EMappingMode::eByPolygonVertex:
				{
					if (referenceModeDiffuseUV == FbxLayerElement::EReferenceMode::eDirect)
					{
						uv.x = layerElementDiffuseUV->GetDirectArray()[polygonIndex * 3 + vertexIndex].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[polygonIndex * 3 + vertexIndex].mData[1];
					}
					else
					{
						int index = layerElementDiffuseUV->GetIndexArray()[polygonIndex * 3 + vertexIndex];
						uv.x = layerElementDiffuseUV->GetDirectArray()[index].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[index].mData[1];

					}
				}
				break;
				case FbxLayerElement::EMappingMode::eByPolygon:
				{
					if (referenceModeDiffuseUV == FbxLayerElement::EReferenceMode::eDirect)
					{
						uv.x = layerElementDiffuseUV->GetDirectArray()[polygonIndex].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[polygonIndex].mData[1];
					}
					else
					{
						int index = layerElementDiffuseUV->GetIndexArray()[polygonIndex];
						uv.x = layerElementDiffuseUV->GetDirectArray()[index].mData[0];
						uv.y = layerElementDiffuseUV->GetDirectArray()[index].mData[1];
					}


				}
				break;
				}

				uv.y = 1.0f - uv.y;
				vertex.mTex = uv;
			}


			//normal
			if (layerElementNormal != nullptr)
			{
				DirectX::XMFLOAT3 normal;
				switch (mappingModeNormal)
				{
				case FbxLayerElement::EMappingMode::eByControlPoint:
				{
					if (referenceModeNormal == FbxLayerElement::EReferenceMode::eDirect)
					{
						normal.x = layerElementNormal->GetDirectArray()[controlPointIndex].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[controlPointIndex].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[controlPointIndex].mData[2];
					}
					else
					{
						int index = layerElementNormal->GetIndexArray()[controlPointIndex];
						normal.x = layerElementNormal->GetDirectArray()[index].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[index].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[index].mData[2];

					}

				}
				break;
				case FbxLayerElement::EMappingMode::eByPolygonVertex:
				{
					if (referenceModeNormal == FbxLayerElement::EReferenceMode::eDirect)
					{
						normal.x = layerElementNormal->GetDirectArray()[polygonIndex * 3 + vertexIndex].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[polygonIndex * 3 + vertexIndex].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[polygonIndex * 3 + vertexIndex].mData[2];
					}
					else
					{
						int index = layerElementNormal->GetIndexArray()[polygonIndex * 3 + vertexIndex];
						normal.x = layerElementNormal->GetDirectArray()[index].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[index].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[index].mData[2];

					}
				}
				break;
				case FbxLayerElement::EMappingMode::eByPolygon:
				{
					if (referenceModeDiffuseUV == FbxLayerElement::EReferenceMode::eDirect)
					{
						normal.x = layerElementNormal->GetDirectArray()[polygonIndex].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[polygonIndex].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[polygonIndex].mData[2];
					}
					else
					{
						int index = layerElementNormal->GetIndexArray()[polygonIndex];
						normal.x = layerElementNormal->GetDirectArray()[index].mData[0];
						normal.y = layerElementNormal->GetDirectArray()[index].mData[1];
						normal.z = layerElementNormal->GetDirectArray()[index].mData[2];
					}


				}
				break;
				}

				vertex.mNormal = normal;
				ConvertToDirectAxisSystem(vertex.mNormal);

			}


			//vertex가 중복되는가?
			//그렇다면 vertex는 재추가되지않고 그 해당vertex의 인덱스값만 추가된다.
			std::vector<SkinningVertex>::iterator it =
				std::find(vertexVectorPerSubMesh.begin(), vertexVectorPerSubMesh.end(), vertex);
			size_t index = 0;
			if (it == vertexVectorPerSubMesh.end())
			{
				index = vertexVectorPerSubMesh.size();
				vertexVectorPerSubMesh.push_back(vertex);
			}
			else
			{
				index = it - vertexVectorPerSubMesh.begin();
			}
			indexVectorPerSubMesh.push_back(index);
		}
	}



	//for (int i = 0; i < skinningIndexVectorPerSubMeshVector.size(); ++i)
	//	ConvertVertexWindingOrder(skinningIndexVectorPerSubMeshVector[i]);





	//submesh, mesh구축
	int vertexOffset = 0;
	MeshIndexType indexFirst = 0;


	std::vector<RawSubMeshData> rawSubMeshVector(subMeshNum);
	for (int subMeshIndex = 0; subMeshIndex < subMeshNum; subMeshIndex++)
	{
		rawSubMeshVector[subMeshIndex].mVertexOffset = vertexOffset;
		rawSubMeshVector[subMeshIndex].mIndexFirst = indexFirst;
		rawSubMeshVector[subMeshIndex].mIndexLast = indexFirst + skinningIndexVectorPerSubMeshVector[subMeshIndex].size();
		rawSubMeshVector[subMeshIndex].mMaterialName = materialVector[subMeshIndex].mName;


		vertexOffset += skinningVertexVectorPerSubMeshVector[subMeshIndex].size();
		indexFirst = rawSubMeshVector[subMeshIndex].mIndexLast;


		for (int indexOrder = 0; indexOrder < skinningIndexVectorPerSubMeshVector[subMeshIndex].size(); indexOrder += 3)
		{
			std::swap(skinningIndexVectorPerSubMeshVector[subMeshIndex][indexOrder + 1], skinningIndexVectorPerSubMeshVector[subMeshIndex][indexOrder + 2]);
		}

		oRawMeshData->mVertexVector.insert(oRawMeshData->mVertexVector.end(), skinningVertexVectorPerSubMeshVector[subMeshIndex].begin(),
			skinningVertexVectorPerSubMeshVector[subMeshIndex].end());

		oRawMeshData->mIndexVector.insert(oRawMeshData->mIndexVector.end(), skinningIndexVectorPerSubMeshVector[subMeshIndex].begin(),
			skinningIndexVectorPerSubMeshVector[subMeshIndex].end());



	}





	/*std::vector<RawSubMeshData> rawSubMeshVector(subMeshNum);
	for (int subMeshIndex = 0; subMeshIndex < subMeshNum; subMeshIndex++)
	{
		rawSubMeshVector[subMeshIndex].mVertexOffset = vertexOffset;
		rawSubMeshVector[subMeshIndex].mIndexFirst = indexFirst;
		rawSubMeshVector[subMeshIndex].mIndexLast = indexFirst + staticIndexVectorPerSubMeshVector[subMeshIndex].size();
		rawSubMeshVector[subMeshIndex].mMaterialName = materialVector[subMeshIndex].mName;


		vertexOffset += staticVertexVectorPerSubMeshVector[subMeshIndex].size();
		indexFirst = rawSubMeshVector[subMeshIndex].mIndexLast;


		for (int indexOrder = 0; indexOrder < staticIndexVectorPerSubMeshVector[subMeshIndex].size(); indexOrder += 3)
		{
			std::swap(staticIndexVectorPerSubMeshVector[subMeshIndex][indexOrder + 1], staticIndexVectorPerSubMeshVector[subMeshIndex][indexOrder + 2]);
		}



		oRawMeshData->mVertexVector.insert(oRawMeshData->mVertexVector.end(), staticVertexVectorPerSubMeshVector[subMeshIndex].begin(),
			staticVertexVectorPerSubMeshVector[subMeshIndex].end());
		oRawMeshData->mIndexVector.insert(oRawMeshData->mIndexVector.end(), staticIndexVectorPerSubMeshVector[subMeshIndex].begin(),
			staticIndexVectorPerSubMeshVector[subMeshIndex].end());



	}*/













	if (layerElementNormal == nullptr)
	{
		//노멀을 직접계산한다.
		VertexHelper::CaculateSkinningMeshNormalVector(oRawMeshData->mVertexVector, oRawMeshData->mIndexVector);
	}



	oRawMeshData->mRawSubMeshVector = std::move(rawSubMeshVector);


}

void Quad::FbxParser::LoadBoneHierarchy(FbxNode* rootNode,Skeleton** oSkeleton)
{

	int childNodeCount = rootNode->GetChildCount();

	//mesh는 그냥 노드로 표현만
	std::queue<FbxNode*> nodeQueue;
	nodeQueue.push(rootNode);
	FbxNode* rootSkeletonNode = nullptr;
	//최상위 루트 bone노드 구하기
	while (!nodeQueue.empty())
	{

		FbxNode* fbxNode = nodeQueue.front();
		nodeQueue.pop();
		FbxNodeAttribute* nodeAttribute = fbxNode->GetNodeAttribute();

		if (nodeAttribute != nullptr && nodeAttribute->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton)
		{
			rootSkeletonNode = fbxNode;

			while(!nodeQueue.empty())
				nodeQueue.pop();

			break;
		}
		else
		{
			int childNodeCount = fbxNode->GetChildCount();
			for (int childNodeIndex = 0; childNodeIndex < childNodeCount; ++childNodeIndex)
			{
				FbxNode* childNode = fbxNode->GetChild(childNodeIndex);
				nodeQueue.push(childNode);
			}

		}
	}

	if (rootSkeletonNode != nullptr)
	{
		*oSkeleton = new Skeleton;
		//*oSkeleton = SkeletonManager::CreateSkeleton()
	}else
	{ 
		return;
	}



	//bone 계층구조 추출
	FbxNode* parentNode = rootSkeletonNode->GetParent();
	//skeleton은아닌데 그skeleton의 최상위부모노드도 추가 
	if (parentNode != rootNode)
	{
		nodeQueue.push(parentNode);
		rootSkeletonNode = parentNode;
	}
	else
	{
		nodeQueue.push(rootSkeletonNode);
	}


	int parentBoneIndex = -1;
//	const std::wstring& boneName = Utility::ConvertToWString(rootSkeletonNode->GetName(), true);
	const std::string& boneName = rootSkeletonNode->GetName();
	FbxNodeAttribute* rootSkeletonNodeAttribute = rootSkeletonNode->GetNodeAttribute();
	if (rootSkeletonNodeAttribute != nullptr  && rootSkeletonNodeAttribute->GetAttributeType() == FbxNodeAttribute::EType::eNull)
	{
		//Bone* bone = new Bone(boneName, EBoneType::eNull);
		Bone* bone = BoneFactory::CreateBone();
		bone->SetName(boneName);
		bone->SetBoneType(EBoneType::eNull);

		(*oSkeleton)->AddBone(bone, parentBoneIndex);
	}
	if (rootSkeletonNodeAttribute != nullptr && rootSkeletonNodeAttribute->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton)
	{
		Bone* bone = BoneFactory::CreateBone();
		bone->SetName(boneName);
		bone->SetBoneType(EBoneType::eBone);
		(*oSkeleton)->AddBone(bone, parentBoneIndex);
	}
	else if (rootSkeletonNodeAttribute != nullptr && rootSkeletonNodeAttribute->GetAttributeType() == FbxNodeAttribute::EType::eMesh)
	{
		Bone* bone = BoneFactory::CreateBone();
		bone->SetName(boneName);
		bone->SetBoneType(EBoneType::eMesh);
		(*oSkeleton)->AddBone(bone, parentBoneIndex);
	}
	parentBoneIndex = 0;







	while (!nodeQueue.empty())
	{

		FbxNode* fbxSkeletonNode = nodeQueue.front();
		nodeQueue.pop();


		int childNodeCount = fbxSkeletonNode->GetChildCount();
	
		for (int childNodeIndex = 0; childNodeIndex < childNodeCount; ++childNodeIndex)
		{
			FbxNode* childNode = fbxSkeletonNode->GetChild(childNodeIndex);
			FbxNodeAttribute* childNodeAttribute = childNode->GetNodeAttribute();
		//	const std::wstring& boneName = Utility::ConvertToWString(childNode->GetName(), true);
			const std::string& boneName = childNode->GetName();
			if (rootSkeletonNodeAttribute != nullptr && (rootSkeletonNodeAttribute->GetAttributeType() == FbxNodeAttribute::EType::eNull))
			{
				Bone* bone = BoneFactory::CreateBone();
				bone->SetName(boneName);
				bone->SetBoneType(EBoneType::eNull);

				(*oSkeleton)->AddBone(bone, parentBoneIndex);
			}else if (childNodeAttribute->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton)
			{
				Bone* bone = BoneFactory::CreateBone();
				bone->SetName(boneName);
				bone->SetBoneType(EBoneType::eBone);
				(*oSkeleton)->AddBone(bone, parentBoneIndex);
			}
			else if (childNodeAttribute->GetAttributeType() == FbxNodeAttribute::EType::eMesh)
			{
				Bone* bone = BoneFactory::CreateBone();
				bone->SetName(boneName);
				bone->SetBoneType(EBoneType::eMesh);
				(*oSkeleton)->AddBone(bone, parentBoneIndex);
			}
			nodeQueue.push(childNode);

		}

		parentBoneIndex++;
	}

	const std::vector<Bone*>& boneVector = (*oSkeleton)->GetBoneVector();

















}


void Quad::FbxParser::LoadAnimClip(FbxAnimLayer* animLayer,Skeleton & skeleton, AnimationClip & oAnimaitionClip)
{

	//property -	curvenode - curve
	//이 propety를 소유한노드는 일반적으로일단, skeleton,또는 간혹mesh
	//이 property의 각 성분(ex x,y,z)마다 curve가 연결된다.
	//curve는 curveKey의 모임이다. curveKey들은 시간에따라 정렬되어있다. curveKey안에 어떻게 값이 변하는지가있다.

	//curveNode가 에니메이션될떄만 처리하고 ,
	//그 연결된 속성이뭔가 연결된 노드는어떤노드인가 (skeleton,mesh)(이름)
	//그 시간에 그 속성값을가진다(ex )위치: x,y,z,)

	//skeleton을 입력으로 받고 그 skeleton에서 그 bone을 이름으로 찾고, id를 얻고(인덱스)
	//animationclip을 구축한다.

	std::vector<BoneAnimationClip> boneAnimationClipVector(skeleton.GetBoneVector().size());
	std::vector<bool> mBoneAnimationClipUploadStateVector(skeleton.GetBoneVector().size(),false);
	
	int animCurveNodeNum = animLayer->GetSrcObjectCount<FbxAnimCurveNode>();
	for (int animCurveNodeIndex = 0; animCurveNodeIndex < animCurveNodeNum; ++animCurveNodeIndex)
	{
		FbxAnimCurveNode* animCurveNode = animLayer->GetSrcObject<FbxAnimCurveNode>(animCurveNodeIndex);

		if (!animCurveNode->IsAnimated())
			continue;
		//에니메이션이안되도 초기값이 있을수있는데 아닌가.


		FbxProperty  property = animCurveNode->GetDstProperty();
		FbxObject* object = property.GetFbxObject();//이 프로퍼티를 소유한 object(node는 누구인가)
		FbxNode* node = FbxCast<FbxNode>(object);
		const char* nodeName = node->GetName();

		//int boneID = skeleton.GetBoneID(Utility::ConvertToWString(nodeName,true));
		int boneID = skeleton.GetBoneID(nodeName);
		FbxString propertyName = property.GetName();

		if (mBoneAnimationClipUploadStateVector[boneID])
		{	//이노드는 이미처리했다.
			continue;
		}

		// 뼈대공간에서 각 시간(frame)에 루트공간으로의 변환행렬을 구하고
		//그 행렬에서  s r t를 추출하여 저장한다.
		//뼈대의 animation clip 을 구축한다 (key frame모음)
		//오프셋행렬은 여기서 처리하지않는다.



		//그리고 따로 각 bone의 각 키프레임마다 다음프레임과의 보간방법을 추출하여 저장한다.



		
































		//모든것을 다일일이 처리할수도있고
		//if (property == node->LclTranslation)
		//{
		//	int channelNum = animCurveNode->GetChannelsCount();
		//	for (int channelIndex = 0; channelIndex < channelNum; ++channelIndex)
		//	{
		//		FbxAnimCurve * curve= animCurveNode->GetCurve(channelIndex);
		//			int a = 2;
		//	}
		//}
		//else if (property == node->LclScaling)
		//{
		//	int channelNum = animCurveNode->GetChannelsCount();
		//	for (int channelIndex = 0; channelIndex < channelNum; ++channelIndex)
		//	{
		//		FbxAnimCurve* curve = animCurveNode->GetCurve(channelIndex);
		//		int a = 2;
		//	}
		//}
		//else if (property == node->LclRotation)
		//{
		//	int channelNum = animCurveNode->GetChannelsCount();
		//		for (int channelIndex = 0; channelIndex < channelNum; ++channelIndex)
		//	{
		//		FbxAnimCurve * curve= animCurveNode->GetCurve(channelIndex);
		//			int a = 2;
		//	}
		//}
		//


		////int curveCount = animCurveNode->GetCurveCount

		////boneAnimationClipVector[boneID]








	}





}

void Quad::FbxParser::AnimCurveNode(FbxAnimLayer* AnimLayer)
{

	int animCurveNodeNum = AnimLayer->GetSrcObjectCount<FbxAnimCurveNode>();	

	for (int animCurveNodeIndex = 0; animCurveNodeIndex < animCurveNodeNum; ++animCurveNodeIndex)
	{
		FbxAnimCurveNode * animCurveNode = AnimLayer->GetSrcObject<FbxAnimCurveNode>(animCurveNodeIndex);


		bool isComposite = animCurveNode->IsComposite();

		bool isAnimated = animCurveNode->IsAnimated();
		if (isAnimated == true)
		{
			//curveNode에는 하나의 property만 연결된다.
			FbxProperty  dstProperty = animCurveNode->GetDstProperty();
			FbxObject* object = dstProperty.GetFbxObject();//이 프로퍼티를 소유한 object(node는 누구인가)
			FbxNode* node = FbxCast<FbxNode>(object);
			const char* nodeName = node->GetName();

			FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
			switch (nodeAttribute->GetAttributeType())
			{
			case FbxNodeAttribute::EType::eSkeleton:
			{


			}
			break;
			case FbxNodeAttribute::EType::eMesh:
			{



			}
			break;
			}





		}


			//FbxString propertyName = dstProperty.GetName();

			int propertyChannelCount = animCurveNode->GetChannelsCount();
			for (int channelIndex = 0; channelIndex < propertyChannelCount; ++channelIndex)
			{
				FbxString propertyChannelName = animCurveNode->GetChannelName(channelIndex);
				FbxAnimCurve* curve = animCurveNode->GetCurve(channelIndex, 0);




				int a = 2;
			}






	}


}

DirectX::XMFLOAT4X4 Quad::FbxParser::ConvertFbxAMatrix(const FbxAMatrix& fbxAMatrix)
{

	DirectX::XMFLOAT4X4 matrix;
	for (int row = 0; row < 4; ++row)
	{
		matrix.m[row][0]=	fbxAMatrix[row][0];
		matrix.m[row][1]=fbxAMatrix[row][1];
		matrix.m[row][2]=fbxAMatrix[row][2];
		matrix.m[row][3]= fbxAMatrix[row][3];
	
		
	}
	return matrix;





	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::FbxParser::ConvertToDirectAxisSystem(DirectX::XMFLOAT3& oPos)
{


	switch (mAxisType)
	{
	case EAxisType::eMayZUp:
	{
		float tempY = oPos.y;
		oPos.y = oPos.z;
		oPos.z = tempY;
	}
		break;
	case EAxisType::eOpenGL:
	{
		oPos.z *= -1.0f;
	}
		break;
	}


}

void Quad::FbxParser::ConvertQauternionToDirectAxisSystem(DirectX::XMFLOAT3& oPos)
{
	switch (mAxisType)
	{
	/*case EAxisType::eMayZUp:
	{
		float tempY = oPos.y;
		oPos.y = oPos.z;
		oPos.z = tempY;
	}
	break;*/
	case EAxisType::eOpenGL:
	{
		oPos.x *= -1.0f;
		oPos.y *= -1.0f;
	}
	break;
	}


}

void Quad::FbxParser::ConvertMatrixToDirectAxisSystem(DirectX::XMFLOAT4X4& matrix)
{

	switch (mAxisType)
	{
		case EAxisType::eMayZUp:
		{
			
		}
		break;
	case EAxisType::eOpenGL:
	{
		DirectX::XMMATRIX A = DirectX::XMMatrixSet(1, 0, 0, 0,
													0, 1, 0, 0,
													0, 0, -1, 0,
													0, 0, 0, 1);
		DirectX::XMStoreFloat4x4(&matrix,A* DirectX::XMLoadFloat4x4(&matrix)* A);

		
	}
	break;
	}



}

Quad::EAnimationTimeMode Quad::FbxParser::ConvertTimeMode(FbxTime::EMode timeMode) const
{
	switch (timeMode)
	{
	case FbxTime::EMode::eFrames30:

		return EAnimationTimeMode::eFPS30;

	case FbxTime::EMode::eFrames60:

		return EAnimationTimeMode::eFPS60;

		
	default:
		OutputDebugString(L"에니메이션프레임모드가 기존의것들과 다릅니다.\n");
		assert(-1);
		return EAnimationTimeMode::eFPS30;

	}

}

void Quad::FbxParser::ConvertVertexWindingOrder(std::vector<MeshIndexType>& oIndexVector)
{

	if (mAxisType != EAxisType::eDirect)
	{
		for (int i = 0; i < oIndexVector.size(); i += 3)
		{
			std::swap(oIndexVector[i + 1], oIndexVector[i + 2]);
		}
	}


}

