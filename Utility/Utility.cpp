#include "Utility/Utility.h"
#include"unicode/ucnv.h"
#include"unicode/ustring.h"
#include"unicode/unistr.h"


#include<string>

#include<shobjidl_core.h>
#include<shlwapi.h>

namespace Quad
{
	void* Utility::AllocAligned(size_t size, uint8_t align)
	{
		

		uint8_t* pActualAddr = new uint8_t[size + align];


		uint8_t* pStartAddr = (uint8_t*)(((uintptr_t)pActualAddr + align) & (~(align - 1)));


		ptrdiff_t shift = pStartAddr - pActualAddr;
		assert(0 < shift && shift <= 256);
		pStartAddr[-1] = (uint8_t)shift;

		return pStartAddr;

	}

	void Utility::FreeAligned(void* ptr)
	{
		if (ptr == nullptr)
			return;

		uint8_t* pStartAddr = (uint8_t*)ptr;

		ptrdiff_t shift = pStartAddr[-1];
		if (shift == 0)
			shift = 256;

		uint8_t* pActualAddr = pStartAddr - shift;

		delete[] pActualAddr;

		return;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> Utility::CreateDefaultBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, void* pElements, UINT64 elementSize, UINT64 elementNum, Microsoft::WRL::ComPtr<ID3D12Resource>& mUploadBuffer)
	{
		D3D12_RESOURCE_DESC uploadBufferDesc;
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Alignment = 0;	//64kb;
		uploadBufferDesc.Width = elementSize * elementNum;
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


		CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		ThrowIfFailed(device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&mUploadBuffer)));

		D3D12_RANGE read_range{ 0,0 };
		void* pMapped = nullptr;
		ThrowIfFailed(mUploadBuffer->Map(0, nullptr, (void**)&pMapped));

		memcpy(pMapped, pElements, elementSize * elementNum);

		mUploadBuffer->Unmap(0, nullptr);


		D3D12_RESOURCE_DESC defaultBufferDesc;
		defaultBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		defaultBufferDesc.Alignment = 0;	//64kb;
		defaultBufferDesc.Width = elementSize * elementNum;
		defaultBufferDesc.Height = 1;
		defaultBufferDesc.DepthOrArraySize = 1;
		defaultBufferDesc.MipLevels = 1;
		defaultBufferDesc.SampleDesc.Count = 1;
		defaultBufferDesc.SampleDesc.Quality = 0;
		defaultBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		defaultBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		defaultBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		Microsoft::WRL::ComPtr<ID3D12Resource > defaultBuffer;

		CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(device->CreateCommittedResource(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &defaultBufferDesc, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&defaultBuffer)));

		// 리소스 상태 전환
		auto defaultBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(1, &defaultBufferTransition);

		//auto uploadBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(uploadBuffer.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE);
		//commandList->ResourceBarrier(1, &uploadBufferTransition);

		// 리소스 복사
		commandList->CopyResource(defaultBuffer.Get(), mUploadBuffer.Get());
		//commandList->CopyBufferRegion(defaultBuffer.Get(), 0, uploadBuffer.Get(), 0, elementSize * elementNum);
		// 리소스 상태 전환
		auto uploadBufferTransitionAfterCopy = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
		commandList->ResourceBarrier(1, &uploadBufferTransitionAfterCopy);

		return defaultBuffer;
		//return defaultBuffer;
	}

	DirectX::XMFLOAT4X4 Utility::GetIdentityMatrixF()
	{
		DirectX::XMFLOAT4X4 identityMatrix = { 1.0f,0,0,0,
											0,1.0f,0,0,
											0,0,1.0f,0,
											0,0,0,1.0f };

		return identityMatrix;

	}

	DirectX::XMMATRIX Utility::GetIdentityMatrix()
	{
		return DirectX::XMMatrixSet(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	std::string Utility::ConvertToString(const std::wstring  & str, bool unicode)
	{
		std::string ret;
		icu::UnicodeString u16String = str.c_str();
		u16String.toUTF8String(ret);
		
		return ret;



	}

	std::wstring Utility::ConvertToWString( const std::string& str, bool unicode)
	{
		if (unicode)
		{
			icu::UnicodeString u16String = icu::UnicodeString::fromUTF8(str);
			std::wstring ret((wchar_t*)u16String.getBuffer(),u16String.length());
			return ret;
		}
		else
			return L"";
	}

	
	
	std::string Utility::GetExtension(const std::string& filePath)
	{

		size_t dotIndex = filePath.find_last_of('.');
		if (dotIndex == std::string::npos)
			return "";

		std::string extension = filePath.substr(dotIndex + 1);
		return extension;
	}

	std::string Utility::GetFileNameFromPath(const std::string& filePath)
	{
		
		size_t offset1 =	filePath.find_last_of(L'\\');
		size_t offset2 =	filePath.find_last_of(L'/');

	
		if (offset1 == std::string::npos && offset2 ==std::string::npos)
		{
			return filePath;
		}

		if (offset1 == std::wstring::npos)
		{
			return filePath.substr(offset2 + 1);
		}
		else if (offset2 == std::wstring::npos)
		{
			return filePath.substr(offset1 + 1);
		}
		else
		{
			if (offset1 > offset2)
				return filePath.substr(offset1 + 1);
			else
				return filePath.substr(offset2 + 1);
		}

		 


	}

	std::string Utility::RemoveExtension(const std::string& fileName)
	{

		size_t dotPoisiton =  fileName.find_last_of(L'.');
		return 	fileName.substr(0, dotPoisiton);
	
	}

	size_t Utility::CaculateConstantBufferElementOffset(int offset, int elementNum, int elementSize)
	{
	

			//한요소는 하나의 4차원벡터안에 들어가야하는데
			//16바이트에서 한요소의 크기를 뺴서 들어갈수있는 여분의 offset을 계산한다.
			// 16-12 =4 , -> offset을 16로 나누었을떄 0,4가 나머지이면 3차원벡터성분이 들어갈자리가있음으로 가능
			int additonalOffset = 16 - elementSize;

			int elementIndex = 0;
			bool spaceState = false;
			while (elementIndex < elementNum)
			{
				for (int i = 0; i <= additonalOffset; i += 4)
				{
					if (offset % 16 == i)
					{
						offset += elementSize;
						elementIndex++;
						spaceState = true;
						break;
					}
				}

				//공간이모자르면
				if (spaceState == false)
					offset = 16 * ((offset / 16) + 1);//최소16의배수로 맞춘다.
				else
					spaceState = false;
			}

	
			return offset;

	}

	bool Utility::ConvertBoolValue(std::string str)
	{

		std::transform(str.begin(), str.end(),str.begin(),towupper);
		if (str == "TRUE")
			return true;
		else if (str == "FALSE")
			return false;
		else
		{
			//디버그출력
			return false;
		}
			
	}


	std::string Utility::GetFolderPathFromPickingDialog()
	{

		IFileDialog* pfd = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pfd));
			
		if (SUCCEEDED(hr))
		{
			pfd->SetOptions(FOS_PICKFOLDERS);

			if (SUCCEEDED(hr))
			{

				hr = pfd->Show(NULL);
				if (SUCCEEDED(hr))
				{
					IShellItem* psiResult;
					hr = pfd->GetResult(&psiResult);
					if (SUCCEEDED(hr))
					{
						PWSTR  FolderPath;
						hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &FolderPath);

						if (SUCCEEDED(hr))
						{
							MessageBox(nullptr, FolderPath, L"파일열기상자", 0);

							return Utility::ConvertToString(FolderPath,true);
						}
					}

				}
			}
		}

		return "";
	}

	bool Utility::CheckFolderPath(const std::string& path)
	{
		std::wstring pathW = Utility::ConvertToWString(path, true);
		return	PathIsDirectoryW(pathW.c_str());
	}

	std::string Utility::SetNewCurrentDirectory(const std::string& newDirectory)
	{
		wchar_t currDirectory[MAX_PATH];
		GetCurrentDirectoryW(MAX_PATH, currDirectory);

		SetCurrentDirectoryW(Utility::ConvertToWString(newDirectory,true).c_str());

		return Utility::ConvertToString(currDirectory,true);

		
	}

	


	//void Utility::CaculateSkinningMeshNormalVector(std::vector<SkinningVertex>& vertexVector, const std::vector<MeshIndexType>& indexVector)
	//{

	//	MeshIndexType index = 0;
	//	while (index < indexVector.size())
	//	{

	//		MeshIndexType vertexOneIndex = indexVector[index];
	//		MeshIndexType vertexTwoIndex = indexVector[index + 1];
	//		MeshIndexType vertexThreeIndex = indexVector[index + 2];

	//		SkinningVertex& vertexOne = vertexVector[vertexOneIndex];
	//		SkinningVertex& vertexTwo = vertexVector[vertexTwoIndex];
	//		SkinningVertex& vertexThree = vertexVector[vertexThreeIndex];

	//		//이 삼각형의 노멀을 구하고 이 정점들의 노멀값은 정규화하고 노멀값에 더한다.
	//		//그리고 다 처리하고나서 다시 정규화한다.

	//		//v0 -v1 = e1  , v0 v2  =e2
	//		DirectX::XMFLOAT3& vertexOnePos = vertexOne.mPos;
	//		DirectX::XMFLOAT3& vertexTwoPos = vertexTwo.mPos;
	//		DirectX::XMFLOAT3& vertexThreePos = vertexThree.mPos;

	//		DirectX::XMVECTOR e1;
	//		DirectX::XMVECTOR e2;

	//		e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&vertexTwoPos), DirectX::XMLoadFloat3(&vertexOnePos));
	//		e2 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&vertexThreePos), DirectX::XMLoadFloat3(&vertexOnePos));

	//		DirectX::XMFLOAT3 normal;
	//		DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e1, e2)));



	//		vertexOne.mNormal.x += normal.x;
	//		vertexOne.mNormal.y += normal.y;
	//		vertexOne.mNormal.z += normal.z;
	//		vertexTwo.mNormal.x += normal.x;
	//		vertexTwo.mNormal.y += normal.y;
	//		vertexTwo.mNormal.z += normal.z;
	//		vertexThree.mNormal.x += normal.x;
	//		vertexThree.mNormal.y += normal.y;
	//		vertexThree.mNormal.z += normal.z;

	//		index += 3;

	//	}

	//	for (int i = 0; i < vertexVector.size(); ++i)
	//	{
	//		DirectX::XMStoreFloat3(&vertexVector[i].mNormal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vertexVector[i].mNormal)));
	//	}
	//}
	
}
