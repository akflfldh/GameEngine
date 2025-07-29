#pragma once



#include<string>
#include<vector>

#include<memory>
#include<IGpuResource.h>
#include<unordered_map>
#include<GpuTypes.h>
#include<CoreMath.h>


namespace Render
{

	using MaterialID = uint32_t;
#define MaterialIDNone 0




	enum class EShaderStage:uint32_t
	{
		eVertex=0,
		ePixel,
		eGeometry,
		eDomain,
		eHull,
		eCompute
	};

	struct ShaderSourceInfo
	{
		void* mShadeCode;
		size_t mShaderCodeSize;
		std::string mEntryPoint;		//셰이더 진입함수이름
		std::string mTarget;			//ex) vs_5_1, ps_5_0
		EShaderStage mStage;			//각 셰이더 타입
	};


	enum class EShaderResourceType
	{
		eConstantBuffer,
		eStructuredBuffer,
		eTexture,
		eSampler
	};

	enum class EShaderResourceDimension
	{
		eNone=0,
		eBuffer,
		eTex2D,
		eSampler
	};





	struct ShaderResourceInfo
	{
		std::string mName;
		bool mIsEngineShaderResource;
		EShaderResourceDimension mDimension;
		EShaderResourceType mType;
	};

	struct TextureShaderResourceInfo :public ShaderResourceInfo
	{
		GRM::ETextureFormat mFormat;
	};


	struct BufferShaderResourceInfo :public ShaderResourceInfo
	{
		uint32_t mBufferID;
	};


	struct SamplerShaderResourceInfo :public ShaderResourceInfo
	{
		
	};


	//pass에서 사용하는 리소스들의 정보
	struct ShaderResourceInfoSet
	{
		std::vector<TextureShaderResourceInfo> mTextureShaderResourceInfoVector;
		std::vector<BufferShaderResourceInfo> mBufferShaderResourceInfoVector;
		std::vector< SamplerShaderResourceInfo> mSamplerShaderResourceInfoVector;
	};



	//렌더링파이프라인 설정
	enum class ECullMode
	{
		eNone=0,
		eFront,
		eBack
	};

	enum class EBlendMode
	{
		eOpaque = 0 , 
		eAlphaBlend,
//		eAdditive // 불투명, 알파 블렌딩, 가산 블렌딩
	};

	enum class EDepthStencilCompareMode
	{
		eNone= 0,
		eLess,
		eLessEqual,
		eEqual
	};

	enum class EDepthWriteMode
	{
		eDisabled =0,
		eEnabled
		
	};
	
	enum class EFillMode
	{
		eSolidMode=0,
		eWireFrameMode
	};


	struct RenderPass
	{
		std::string mPassName;
		std::vector< ShaderSourceInfo> mShaderStage;
		std::vector<std::string > mUsageShaderResourceVector;

		std::vector<uint8_t> mShaderCode;

		ECullMode mCullMode;
		EBlendMode mBlendMode;
		EDepthStencilCompareMode mDepthCompareMode = EDepthStencilCompareMode::eLess;
		EDepthWriteMode mDepthWriteMode;
		EFillMode mFillMode;
	};

	enum InputLayoutType
	{
		eUI=0,
		eStaticMesh,
		eSkinningMesh
	};

	struct MainRenderPass :public RenderPass
	{
		bool mDefaultFlag = false;
		bool mOpaque = true; //투명한타입인지 불투명한 타입의 패스인지
		
		
		//UI메시를 위한 InputLayout인지 
		//스키닝메시를 위한 InputLayout인지,
		//정적메시를 위한 InputLayout 인지
		int mInputType = InputLayoutType::eUI;
	};








	struct CreationMaterialInfo
	{
		std::string mName;	//머터리얼 이름(shader의 이름)
		
		ShaderResourceInfoSet mShaderResourceInfoSet;

		MainRenderPass mMainRenderPass;
		std::vector<RenderPass> mPostRenderPass;
		//엔진에서 기본제공하는 shader들중 사용하는 shader목록 
		std::vector<std::string> mDefaultPassVector;

	};

}







namespace std
{
	template<>
	struct hash< Render::EShaderStage>
	{


		size_t operator()(const Render::EShaderStage& stage) const noexcept
		{
			return static_cast<size_t>(stage);
		}
	};

}









//	struct MaterialDefaultProperties
//	{
//		CoreMath::Vector4 mBaseColor = CoreMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
//		float mRoughness = 0.5f;
//		float mMetallic = 0.0f;
//		CoreMath::Vector3 mEmissiveColor = CoreMath::Vector3(0.0f, 0.0f, 0.0f);
//		// ... 필요한 다른 스칼라, 벡터, 컬러 속성들 ...
//		float mTextureTileScale = 1.0f; // 텍스처 타일링 스케일 (선택 사항)
//	};
//
//	// --- 텍스처 참조 (머티리얼에 기본으로 포함될 텍스처) ---
//// 텍스처 파일 경로 대신, 이미 로드되어 GpuResourceManager에 등록된 IGpuResource 참조를 받음
//	struct MaterialTextureInfo
//	{
//		// 이 텍스처가 셰이더의 어떤 슬롯에 바인딩될지에 대한 정보 (RootParameterIndex 등)는
//		// MaterialManager가 셰이더 리플렉션을 통해 결정하거나, MaterialManager가 자체 바인딩 규칙을 가집니다.
//		// 여기서는 어떤 종류의 텍스처인지 만을 명시합니다.
//		enum class Type : uint32_t
//		{
//			Albedo,
//			Normal,
//			MetallicRoughness,
//			Emissive,
//			// ... 기타 텍스처 타입
//		};
//		Type mType;
//		GRM::IGpuResource* mTextureResource; // <<--- GPU 리소스 참조 (중요)
//	};

