#pragma once



#ifdef D3DX
#include<Windows.h>
namespace Render { using WindowHandle = HWND; }
//namespace Render{using }



#endif

#include<memory>
#include<vector>
#include"MaterialType.h"
namespace GRM { class IGpuResource; }

namespace Render
{


	using RenderChannelID = uint32_t;
	using RootSignatureID = uint16_t;




	//렌더시스템에 새로운채널을 생성하기위해 전달하는 구조체
	struct CreationRenderChannelInfo
	{
		WindowHandle mWindowHandle;
		float mLeft = 0.0f;
		float mTop=0.0f;
		float mWidth = 100.0f;
		float mHeight = 100.0f;
		float mMinZ = 0.0f;
		float mMaxZ = 0.0f;

		size_t mCameraID = 0;

	};


	struct MeshItem
	{
		uint32_t mVertexNum = 0;
		uint32_t mIndexNum = 0;
		uint32_t mVertexOffset = 0;
		uint32_t mIndexOffset = 0;
		GRM::IGpuResource* mVertexBuffer=nullptr;
		GRM::IGpuResource* mIndexBuffer =nullptr; //만약 indexBuffer nullptr이면 vertex만 사용한 draw수행


		bool operator==(const MeshItem& rhs) const
		{
			if (mVertexBuffer != rhs.mVertexBuffer) return false;
			if (mIndexBuffer != rhs.mIndexBuffer) return false;
			if (mVertexNum != rhs.mVertexNum) return false;
			if (mVertexOffset != rhs.mVertexOffset) return false;
			if (mIndexNum != rhs.mIndexNum) return false;
			if (mIndexOffset != rhs.mIndexNum) return false;


			return true;

		}

		bool operator!=(const MeshItem& rhs) const
		{
			return !this->operator==(rhs);
		}


	};

	struct InstanceItem
	{
		uint32_t mInstanceCount = 1;      // 이 드로우 콜에서 그릴 인스턴스 개수 (1이면 비-인스턴싱)
		uint32_t mInstanceBufferOffset = 0;
	};


	struct Viewport
	{
		float TopLeftX = 0.0f;
		float TopLeftY = 0.0f;
		float Width = 0.0f;
		float Height = 0.0f;
		float MinDepth = 0.0f;
		float MaxDepth = 1.0f; 

	};

	// Scissor Rect 정보를 담는 구조체 (D3D12_RECT와 유사)
	struct ScissorRect
	{
		long Left = 0;
		long Top = 0;
		long Right = 0;
		long Bottom = 0;

		bool operator==(const ScissorRect& rhs) const
		{
			if (Left != rhs.Left)return false;
			if (Top != rhs.Top)return false;
			if (Right != rhs.Right)return false;
			if (Bottom != rhs.Bottom)return false;
			return true;
		}

		bool operator!=(const ScissorRect& rhs) const
		{
			return !operator==(rhs);
		}


	};

	//채널별로 전역적으로 매프레임시 제공되어야하는 정보
	struct FrameContext
	{
		GRM::IGpuResource* mGlobalPassBuffer;
		Viewport mViewport;
		ScissorRect mScissorRect;
		//UINT mGlobalPassConstantsRootParameterIndex; 이것도 아마 엔진에서 고정한대로 나오지않을까?
	};


	struct BindingGpuResource
	{
		std::string mName;
		Render::EShaderResourceType mType;
		uint32_t mOffset;			//구조적버퍼일떄 유효 , 구조적버퍼의 baseOffset
		GRM::IGpuResource* gpuResource;
	};



	//동일한 Gpu머터리얼을 사용하는 Asset머터리얼들 각각에대해서
	//동일한 Mesh(SubMesh)를 사용하는 인스턴스 단위로 나누고 .
	//추가로 같은 ScissorRect를 가진 단위로 또 한번 분류한다.


	//동일한 AssetMaterial을 사용하면
	//동일한 ScissorRect를 사용하고 , 동일한 Mesh를 사용하는 인스턴스들에대한 렌더아이템이다.
	struct RenderItem
	{
		MaterialID mMaterialID;
		ScissorRect mScissor;
		MeshItem mMeshItem;
		InstanceItem mInstance;
		//바인딩할 gpuResource들
		std::vector< BindingGpuResource> mBindingGpuResourceVector;

	};


	//시스템 내부용
	struct InternalRenderItem : public Render::RenderItem
	{
		uint64_t mFirstSortKey;
		uint64_t mSecondSortKey;

		bool operator<(const InternalRenderItem& rhs)
		{
			if (mFirstSortKey != rhs.mFirstSortKey) return mFirstSortKey < rhs.mFirstSortKey;

			if (mSecondSortKey != rhs.mSecondSortKey) return mSecondSortKey < rhs.mSecondSortKey;
	
			return false;
		}

	};
}
