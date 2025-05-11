#include "TextureLoader.h"
#include "GraphicCommand.h"

#include"Asset/Texture/Texture.h"

namespace Quad
{
	void TextureLoader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, GraphicCommand* graphicsCommandObject)
	{

		mDevice = device;
		mGraphicsCommandObject = graphicsCommandObject;
	}

	bool TextureLoader::LoadTextureFile(const std::string  & filePath, Texture& oTexture)
	{
		if (mGraphicsCommandObject->GetCloseState())
			mGraphicsCommandObject->ResetCommandList(nullptr);

		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> newTextureResource = nullptr;
		//if (filePath.find(L".dds") == std::wstring::npos)        //문제가발생할수있다 파일이름이 .dds.txt이런식이면

		std::string extension = PathFindExtensionA(filePath.c_str());
		std::wstring filePathW;
		filePathW= Utility::ConvertToWString(filePath, true);

		if (extension ==".dds")
		{
			//여기서 exception보다는 false를 리턴하게 해야한다.
			newTextureResource = LoadTextureFromDDS(filePathW, uploadBuffer);
			

		}
		else if (extension == ".bmp" || extension == ".jpg" || extension == ".png")
		{
			newTextureResource = LoadTextureFromBITMAP(filePathW, uploadBuffer);
		}
		else
		{
			return false;
		}


		if (newTextureResource != nullptr)
		{
			oTexture.SetTextureResource(newTextureResource);
		}
		else if (newTextureResource == nullptr)
		{
			return false;
		}

		//mUploadBuffers.push_back(uploadBuffer);
		mGraphicsCommandObject->ExecuteCommandList();
		mGraphicsCommandObject->FlushCommandQueue();


		return true;
	}


	void TextureLoader::ReleaseUploadBuffer()
	{
		mUploadBuffers.clear();

	}

	void TextureLoader::test()
	{


	








	}

	Microsoft::WRL::ComPtr<ID3D12Resource> TextureLoader::LoadTextureFromDDS(const std::wstring& filename, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
	{
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();


		Microsoft::WRL::ComPtr<ID3D12Resource> defaultTexture;
		std::unique_ptr<uint8_t[]> ddsData;
		std::vector<D3D12_SUBRESOURCE_DATA> subresourceData;
		/*ThrowIfFailed(DirectX::LoadDDSTextureFromFile(mDevice.Get(), filename.data(), defaultTexture.GetAddressOf(), ddsData, subresourceData));*/


		HRESULT ret = DirectX::LoadDDSTextureFromFile(mDevice.Get(), filename.data(), defaultTexture.GetAddressOf(), ddsData, subresourceData);
		if (ret != S_OK)
		{
			return nullptr;
		}




		size_t subResourceNum = subresourceData.size();//defaultTexture->GetDesc().MipLevels; -텍스처배열일수도있기에 이렇게하면안될거같다.
		//subresourceData가 가리키는 데이터가 바로 ddsData이다

		//하나의 거대한 버퍼를만들어서 텍스처를 업로드하기위해 재사용할수도있을거같다.
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT* placedSubResourcesFootPrint = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[subResourceNum];
		UINT* numRow = new UINT[subResourceNum];
		UINT64* rowSizeInByte = new UINT64[subResourceNum];
		UINT64* pTotalBytes = new UINT64;
		D3D12_RESOURCE_DESC defaultTextureDesc = defaultTexture->GetDesc();
		mDevice->GetCopyableFootprints(&defaultTextureDesc, 0, (UINT)subResourceNum, 0, placedSubResourcesFootPrint, numRow, rowSizeInByte, pTotalBytes);
		//업로드힙의 자원에서의 sub리소스들의 (다른리소스로 복사할수있는)layout을 얻는다.

		D3D12_RESOURCE_DESC uploadBufferDesc;
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Alignment = 0;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.Width = *pTotalBytes;
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		CD3DX12_HEAP_PROPERTIES uploadheap(D3D12_HEAP_TYPE_UPLOAD);
		mDevice->CreateCommittedResource(&uploadheap, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer));


		D3D12_RANGE range{ 0,0 };
		uint8_t* pMapped = nullptr;
		uploadBuffer->Map(0, &range, (void**)&pMapped);
		for (int i = 0; i < subResourceNum; ++i)
		{

			uint8_t* pData = (uint8_t*)subresourceData[i].pData;
			UINT64 offset = placedSubResourcesFootPrint[i].Offset;
			for (unsigned int slice = 0; slice < placedSubResourcesFootPrint[i].Footprint.Depth; ++slice)
			{
				for (unsigned int r = 0; r < numRow[i]; ++r)
				{
					memcpy((void*)&pMapped[offset + r * placedSubResourcesFootPrint[i].Footprint.RowPitch], pData + r * subresourceData[i].RowPitch, rowSizeInByte[i]);
					//	pData +=  subresourceData[i].RowPitch;
					//offset += placedSubResourcesFootPrint[i].Footprint.RowPitch;
				}
				pData += subresourceData[i].SlicePitch;
				offset += placedSubResourcesFootPrint[i].Footprint.RowPitch * numRow[i];
			}
		}
		uploadBuffer->Unmap(0, nullptr);

		//UpdateSubresources

		for (int i = 0; i < subResourceNum; ++i)
		{
			D3D12_TEXTURE_COPY_LOCATION src{ uploadBuffer.Get(),D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT ,placedSubResourcesFootPrint[i] };
			D3D12_TEXTURE_COPY_LOCATION dst{ defaultTexture.Get(),D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,(UINT64)i };

			graphicsCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
		}

		return defaultTexture;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> TextureLoader::LoadTextureFromBITMAP(const std::wstring& filename, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
	{
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();




		Microsoft::WRL::ComPtr<ID3D12Resource> defaultTexture;
		std::unique_ptr<uint8_t[]> decodedInitData;
		D3D12_SUBRESOURCE_DATA subresourceData; //LoadWICTexture는 밉맵을 생성하지못한다.(공간은 예약가능하다고한다)
	/*	ThrowIfFailed(DirectX::LoadWICTextureFromFile(mDevice.Get(), filename.c_str(), defaultTexture.GetAddressOf(), decodedInitData, subresourceData));*/

		HRESULT ret= DirectX::LoadWICTextureFromFile(mDevice.Get(), filename.c_str(), defaultTexture.GetAddressOf(), decodedInitData, subresourceData);

		if (ret != S_OK)
		{
			return nullptr;
		}
		


		D3D12_RESOURCE_DESC defaultTextureDesc = defaultTexture->GetDesc();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedSubResourceFootPrint;
		UINT numRow = 0;
		UINT64  rowSizeInByte = 0;
		UINT64 totalByte = 0;
		mDevice->GetCopyableFootprints(&defaultTextureDesc, 0, 1, 0, &placedSubResourceFootPrint, &numRow, &rowSizeInByte, &totalByte);


		D3D12_RESOURCE_DESC uploadBufferDesc;
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Alignment = 0;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.Width = totalByte;
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		ThrowIfFailed(mDevice->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&uploadBuffer)));

		D3D12_RANGE mapRange = { 0,0 };
		uint8_t* pMapped = nullptr;
		ThrowIfFailed(uploadBuffer->Map(0, &mapRange, (void**)&pMapped));



		UINT64 offset = placedSubResourceFootPrint.Offset;
		UINT64 rowPitch = placedSubResourceFootPrint.Footprint.RowPitch;
		UINT8* pData = (UINT8*)subresourceData.pData;

		for (UINT sliceIndex = 0; sliceIndex < placedSubResourceFootPrint.Footprint.Depth; ++sliceIndex)
		{

			//4x4압축이될수있기때문에 placedSubResourceFootPrint에서 height를 rownum로 사용하지않는다. 
			//height은 압축이전의 행의수를 의미하기에
			for (UINT rowIndex = 0; rowIndex < numRow; ++rowIndex)
			{

				memcpy((void*)&pMapped[offset + rowPitch * rowIndex], pData + rowIndex * subresourceData.RowPitch, rowSizeInByte);

			}
			offset += rowPitch * numRow;
			pData += subresourceData.SlicePitch;
		}



		uploadBuffer->Unmap(0, nullptr);




		D3D12_TEXTURE_COPY_LOCATION src{ uploadBuffer.Get(),D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT ,placedSubResourceFootPrint };
		D3D12_TEXTURE_COPY_LOCATION dst{ defaultTexture.Get(),D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,(UINT64)0 };

		graphicsCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);


		return defaultTexture;
	}


}