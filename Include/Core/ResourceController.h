#pragma once

#include<string>
#include<Shlwapi.h>

#pragma comment(lib,"Shlwapi.lib") 


#include"Controller.h"
#include"ResourceManager/MeshManager.h"
//#include"MaterialManager.h"
//#include"TextureManager.h"
#include"MapManager.h"
#include"RenderSystem.h"
#include"ResourceLoader.h"
#include"ResourceStorer.h"
#include "MapController.h"
//#include"System/FileUISystem.h"
//#include"System/GamePlayUiSystem.h"
//#include"GeoGenerator.h"
#include"MaterialGenerator.h"
#include"HeapManager/DescriptorHeapManagerMaster.h"
#include"Core/Event.h"
//#include"WorldEditEntityFactory.h"
#include"Utility/Singleton.h"
#include"Object/Line/Line.h"
#include"ShaderManager.h"
#include"Shader/ShaderResource.h"
#include"HeapManager/SamplerManager.h"
#include"ResourceManager/SkeletonManager.h"
#include"ResourceManager/AnimationClipManager.h"

#include"ResourceManager/EffectManager/EffectManager.h"

//message를 정의한다.
//#define EngineMode
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB ResourceController :public Controller, public ReaderWriter, public Singleton<ResourceController>
	{
		friend class Application;

#ifdef EngineMode

		friend class EngineModeDirector;
#endif


#ifdef EditorMode

		friend class EditorModeDirector;

#endif 


	public:
		ResourceController() = default;
		~ResourceController() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
			GraphicCommand * graphicsCommandObject,
			MeshManager* meshManager, MaterialManager* materialManager, TextureManager* textureManager,
			MapManager* mapManager, RenderSystem* renderSystem, ResourceLoader* resourceLoader,
			ResourceStorer* resourceStorer, FileUISystem* fileUiSystem, GamePlayUiSystem* gamePlayUiSystem,
			DescriptorHeapManagerMaster* descriptorHeapManagerMaster);



		//event 메세지가 오면 적절하게 처리한다.
		//

		static void Update();

		void OnResize();

		//void AddEvent(Event* event);

		
		static void RemoveStructuredBuffer(StructuredBuffer* buffer);

		//fileUi에게 로드한asset들을 시각적으로 표현하도록하기위해 전달하는 이벤트 
		//void SendLoadFileResponseEvent(const std::vector<Asset*>& assetVector);
		//void SendLoadFileResponseEvent(std::vector<Asset*>&& assetVector);



		//에디터가 디폴트asset들을 다 로드하고,다른시스템들이 초기화된후, 호출하여 기본asset들을 fileUi 윈도우에 띄우기위한 이벤트전달함수.
		//한번만 호출한다.
		void SendDefaultAssetLoadResponseEvent();


		//Load Asset에 포함되지않음.
		void LoadEffect(const std::string& effectFolderPath, BaseWindowController* controller);



		void SaveAssetPackage(const std::string& editorAssetFolderPath, const std::string& userAssetFolderPath, const std::string& outputFolderPath);
		void LoadAssetPackage(const std::string& folderPath);


		void SaveMeshPackage(const std::string& outputFolderPath);
		void LoadMeshPackage(const std::string& assetFolderPath);


		void SaveMaterialPackage(const std::string& outputFolderPath);
		void LoadMaterialPackage(const std::string& assetFolderPath);


		void SaveTexturePackage(const std::string& editorAssetFolderPath, const std::string& userAssetFolderPath, const std::string& outputFolderPath);
		void LoadTexturePackage(const std::string& assetFolderPath);


		void SaveSkeletonPackage(const std::string& outputFolderPath);
		void LoadSkeletonPackage(const std::string& assetFolderPath);


		void SaveAnimClipPackage(const std::string& outputFolderPath);
		void LoadAnimClipPackage(const std::string& assetFolderPath);









		void SaveUserAsset(const std::string& assetFolderPath);
		std::vector<Asset*> LoadUserAsset(const std::string& assetFolderPath);

		
		void SaveUserMaterial(const std::string& assetFolderPath);
		void LoadUserMaterial(const std::string& assetFolderPath);


		void SaveUserTexture(const std::string& assetFolderPath);
		void LoadUserTexture(const std::string& assetFolderPath);


		void SaveUserMesh(const std::string& assetFolderPath);
		void LoadUserMesh(const std::string& assetFolderPath);


		void SaveUserSkeleton(const std::string& assetFolderPath);
		void LoadUserSkeleton(const std::string& assetFolderPath);

		void SaveUserAnimClip(const std::string& assetFolderPath);
		void LoadUserAnimClip(const std::string& assetFolderPath);


	







	protected:

	
	//void ImportFbxFile(const std::string& filePath, std::vector<Asset*>& oAssetVector);
	//	Asset* LoadMeshFile(const std::string& filePath);
		//Asset* LoadMaterialFile(const std::string& filePath);
		Texture* LoadTextureFile(const std::string& filePath);

		Mesh* CreateMesh(RawMeshData* rawMeshData);
		Material* CreateMaterial(RawMaterialData & rawMaterialData);
		bool CreateTexture(RawTextureData& rawTextureData);


		//bool RegisterNewClassFromSchema(const std::string & filePath);
		

		void CreateDefaultInputLayout();
		//effect
			bool CreateConstantBuffer(const std::string & name , UINT bufferElementSize , UINT bufferElementNum);
		StructuredBuffer* CreateStructuredBuffer(UINT elementSize, UINT elementMaxNum);
		/*void SetPipelineStateOption(D3D12_GRAPHICS_PIPELINE_STATE_DESC& oGraphicsPipelineStateDesc, const  std::vector<std::pair<std::wstring, std::wstring>>& pipelineStateOptionVector);
		*/



		//다시 shader 구축


		//먼저 effect로드 ,
		//effect에서 각 셰이더의 경로(이름), 진입점(vs,ps), 버전(vs_5_1, ps_5_1)의 내용을 구문분석
		//그 정보를바탕은 셰이더 로드,컴파일,(이미존재하는지도 파악)
		//그후 id3d12reflection을 이용해서 셰이더 리소스들을 구축


	//	void LoadEffect(const std::string& name);

		void CreateEffect(RawEffectDataTwo & rawEffectDataTwo);
		//RenderPassTwo* CreateRenderPass(RawRenderPassDataTwo& rawRenderPassTwoData, bool isSkinningShader);
		//void ShaderResourceReflection(RenderPassTwo* renderPass,Shader * shader, EShaderResourceVisibility shaderResourceVisibility);


		//bool CreateRootSignature(RenderPassTwo * renderPass);
		//bool CreateGraphicsPipeline(const RawRenderPassDataTwo& rawRenderPassTwoData,
		//	RenderPassTwo* renderPass, const std::vector<Shader*>& shaderVector,
		//	Shader* vertexShader);
		//void SetGraphicsPipelineCfg(RenderPassTwo* renderPass,
		//	D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipelineStateDesc, const std::vector<std::pair<std::wstring, std::wstring>>& graphicsPipelineCfgVector);



		//D3D12_SHADER_VISIBILITY ConvertD3DShaderVisibility(EShaderResourceVisibility visibility);
		//D3D12_DESCRIPTOR_RANGE_TYPE ConvertD3DShaderRangeType(ShaderResource* shaderResource);
	

		//UINT GetInputElementOffset(const std::string& sementicName);
		//DXGI_FORMAT GetInputElementFormat(const std::string& sementicName,
		//	D3D_REGISTER_COMPONENT_TYPE componentType, BYTE mask);
		////모든것을 포함하는 버택스의 정의가있어야하고 
		////그것을 바탕으로 offset값을 계산한

		//void SetPassOrPassFlag(ShaderResourceConstantBuffer* shaderResourceConstantBuffer);
		//D3D12_STENCIL_OP ConvertStencilOP(const std::wstring& str,RenderPassTwo * renderPass) const;
		//D3D12_COMPARISON_FUNC ConvertComparisionFunEnum(const std::wstring& str) const;
		//D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimitiveTopologyType(const std::wstring& str) const;
		//D3D_PRIMITIVE_TOPOLOGY ConvertCommandListPrimitiveTopologyType(const std::wstring& str) const;


		////모든타입이 아닌 default타입에대해서만 처리하고있다.
		//UINT GetDefaultVariableSize(const D3D12_SHADER_TYPE_DESC & variableTypeDesc);
		//UINT GetDefaultVariableElementSize(D3D_SHADER_VARIABLE_TYPE type); //defaultVariableSize의 헬퍼































		bool CreateNewMeshFromGenerator();
		bool CreateNewDefaultMaterialFromGenerator();

		void CreateSampler();


		bool DeleteTexture(const std::wstring& name);
		bool DeleteTexture(Texture* texture);



		//여러 functions
		//Event* GetEvent();


		void DragAndDropFile(Event* event);

		//bool LoadAndCreateTextureResource(const std::wstring & filePath, const std::wstring & textureName);


		void HandleDeleteTextureEvent(Event * pEvent);

		

	private:
		//handle event


		void HandleEvent(Event* pEvent);


		//드래그앤드랍요청이 fileUiController로 부터온것에대한 대답
		//void HandleLoadRequestFile(Event* pEvent);





	private:
		void InitFileUiSystem();
		void InitGamePlayUiSystem();
		void LoadEffect(const std::string& folderPath);
		void LoadCommonTexture();
		void InitLayout();
		D3D12_INPUT_ELEMENT_DESC MakeInputElement(LPCSTR semanticName, UINT sematicIndex, DXGI_FORMAT format, UINT inputSlot, UINT AlignedByteOffset,
			D3D12_INPUT_CLASSIFICATION inputSlotClass= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, UINT instanceDataStepRate=0);
		void CreateDefaultShaderResource();

		bool CreateBoundaryMeshFromGenerator();
		void InitBlendEnumUnMap();
		void InitStencilEnumUnMap();
		void InitComparisonFuncEnumUnMap();
		void LoadDefaultModel();




		void SaveMaterialMetaDataFile(const std::string &filePath);
		void SaveTextureMetaDataFile(const std::string& filePath);
		void SaveMeshMetaDataFile(const std::string& filePath);
		void SaveSkeletonMetaDataFile(const std::string& filePath);
		void SaveAnimationClipMetaDataFile(const std::string& filePath);


		
		//material들을 로드한후 호출해야한다
		//메타데이터를 로드할때 비로서 직렬화수행, EditorDirector는 asset을 반환받고, fileui에 출력 
		std::vector<Asset*> LoadMaterialMetaDataFile(const std::string& filePath);

		//LoadMaterialMetaDataFile과 동일
		std::vector<Asset*> LoadTextureMetaDataFile(const std::string& filePath);

		std::vector<Asset*> LoadMeshMetaDataFile(const std::string& filePath);

		std::vector<Asset*>LoadSkeletonMetaDataFile(const std::string& filePath);

		std::vector<Asset*>LoadAnimationClipMetaDatFile(const std::string& filePath);

	private:
		//utility
		D3D12_BLEND ConvertBlendFactorEnum(const std::wstring& str);
		D3D12_BLEND_OP ConvertBlendOpEnum(const std::wstring& str);
		D3D12_STENCIL_OP ConvertStencilOpEnum(const std::wstring  & str);
		D3D12_COMPARISON_FUNC ConvertComparisonFuncEnum(const std::wstring& str);

		D3D12_SRV_DIMENSION ConvertSRVDimensionEnum(D3D12_RESOURCE_DIMENSION dimension);
		
		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& fileName, const D3D_SHADER_MACRO* pMacro,
			ID3DInclude* pInclude, const std::wstring& entryPoint, const std::wstring& version, UINT flags1,
			UINT flag2);
		
		D3D12_CULL_MODE ConvertCullMode(const std::wstring& str);


		ERenderPassID ConvertRenderPassID(const std::wstring& str) const;
		D3D12_BLEND_OP ConvertD3DBlendOpEnum(const std::wstring& str);

		ETextureFormat ConvertTextureFormatEnum(DXGI_FORMAT format);


		void FillSRVTEX2DDesc(D3D12_SHADER_RESOURCE_VIEW_DESC& oSrv, Microsoft::WRL::ComPtr<ID3D12Resource>resource);


	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		GraphicCommand* mGraphicsCommandObject=nullptr;
		MeshManager* mMeshManager=nullptr;
		MaterialManager* mMaterialManager=nullptr;
		TextureManager* mTextureManager=nullptr;
		EffectManager mEffectManager;
		MapManager* mMapManager =nullptr;
		ConstantBufferManager* mConstantBufferManager =nullptr;
		StructuredBufferFactory* mStructuredBufferFactory=nullptr;
		RenderSystem* mRenderSystem =nullptr ;
		ResourceLoader* mResourceLoader = nullptr; 
		ResourceStorer* mResourceStorer =nullptr ;


		FileUISystem* mFileUiSystem =nullptr ;
		GamePlayUiSystem* mUiSystem =nullptr ;
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster= nullptr;
		ShaderResourceFactory* mShaderResourceFactory= nullptr;
		Quad::Map* mCurrentMap =nullptr ;
		ShaderManager mShaderManager;
		SamplerManager mSamplerManager;
		SkeletonManager mSkeletonManager;
		AnimationClipManager mAnimationClipManager;



		//CircleQueue<Event*> mEventQueue;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mUploadVertexTempBuffer;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mUploadIndexTempBuffer;	//매프레임 마지막에 펜스를 확인하고 임시버퍼들을 제거 해야한다.



		std::unordered_map<std::wstring, D3D12_BLEND> mBlendFactorEnumUnMap;
		std::unordered_map<std::wstring, D3D12_BLEND_OP > mBlendOpEnumUnMap;
		std::unordered_map<std::wstring, D3D12_STENCIL_OP> mStencilOpEnumUnMap;
		std::unordered_map<std::wstring, D3D12_COMPARISON_FUNC> mComparisonFuncEnumUnMap;
		




		std::vector<Asset*> mEditorDefaultAssetVector;



	};



}