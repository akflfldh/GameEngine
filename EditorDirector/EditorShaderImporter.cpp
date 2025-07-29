#include "EditorShaderImporter.h"
#include<assert.h>
#include<Logger.h>
#include<JsonParser.h>
#include<IMaterialManager.h>
#include<MaterialType.h>

#include<Utility.h>
#include<vector>

Quad::EditorShaderImporter* Quad::EditorShaderImporter::mInstance = nullptr;

Quad::EditorShaderImporter* Quad::EditorShaderImporter::GetInstance()
{
	if (mInstance == nullptr)
	{
		LOG_MESSAGE_ERROR("EditorShaderImporter", "EditorShaderImporter를 생성하지않은채로 GetInstance호출");
		assert(0);
	}

	return mInstance;

}

Quad::EditorShaderImporter::EditorShaderImporter(Render::IMaterialManager* materialManager)
	:mGpuMaterialManager(materialManager),mJsonParser(std::make_unique<JsonParser::Parser>())
{

	if (mInstance != nullptr)
	{
		LOG_MESSAGE_ERROR("EditorShaderImporter", "EditorShaderImporter 인스턴스를 중복생성");
		assert(0);
	}

	mInstance = this;






}

Quad::EditorShaderImporter::~EditorShaderImporter()
{
}

bool Quad::EditorShaderImporter::Import(const std::string& shaderFile)
{



	std::string folderPath = CoreUtility::Utility::GetParentFolderPathFromPath(shaderFile);





	//.shader파일을 읽어서 
	//.shader파일은 json포맷이다.
	
	bool ret =mJsonParser->ReadFile(shaderFile);
	if (!ret)
	{	

		LOG_MESSAGE_ERROR("EditorShaderImporter", (shaderFile + " 임포트 실패").c_str());
		return false;
	}


	//renderSystem이 받아드릴수있는형태로 가공
	//Build CreationMaterialInfo
	Render::CreationMaterialInfo creationMaterialInfo;


	while (1)
	{
		//keyword

		std::string key = mJsonParser->GetKeyFromReadPointer();
		

		//총  4가지 keyword
		/*
			Name
			UserResource
			DefaultShader
			UserShader
		*/
		//일단은 Name, UserResource, UserShader 만
		if (key == "Name")
		{
			std::string shaderName;
			bool ret =mJsonParser->GetValueFromReadPointer<std::string>(shaderName);
			if (!ret)
			{
				return false;
			}
			creationMaterialInfo.mName = shaderName;

		}
		else if (key == "UserResource")
		{
			
			mJsonParser->IntoReadPointer();


			//user resource info
			while (1)
			{
				std::string userResourceName = mJsonParser->GetKeyFromReadPointer();

				bool ret =mJsonParser->IntoReadPointer();
				if (!ret)
				{
					return false;
				}


				std::string resourceTypeKey  = mJsonParser->GetKeyFromReadPointer();

				if (resourceTypeKey != "Type")
				{
					return false;
				}


				//type 검사
				std::string resourceType;
				ret = mJsonParser->GetValueFromReadPointer<std::string>(resourceType);
				if (!ret)
				{
					return false;
				}


				if (resourceType == "Tex2D")
				{


					mJsonParser->NextReadPointer();
					std::string formatKey = mJsonParser->GetKeyFromReadPointer();
					
					if (formatKey != "FORMAT")
					{
						return false;
					}
					std::string format;
					mJsonParser->GetValueFromReadPointer<std::string>(format);


					Render::TextureShaderResourceInfo textureResourceInfo;

					textureResourceInfo.mName = userResourceName;
					textureResourceInfo.mIsEngineShaderResource = false;
					textureResourceInfo.mType = Render::EShaderResourceType::eTexture;
					textureResourceInfo.mDimension = Render::EShaderResourceDimension::eTex2D;
					textureResourceInfo.mFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM;
					creationMaterialInfo.mShaderResourceInfoSet.mTextureShaderResourceInfoVector.push_back(std::move(textureResourceInfo));



				}
				else if (resourceType == "StructuredBuffer" || resourceType == "ConstantBuffer")
				{




					mJsonParser->NextReadPointer();
					std::string bufferIDKey = mJsonParser->GetKeyFromReadPointer();

					if (bufferIDKey != "BufferID")
					{
						return false;
					}


					int bufferID = 0;
					ret =mJsonParser->GetValueFromReadPointer<int>(bufferID);
					if (!ret)
					{
						return false;
					}



					Render::BufferShaderResourceInfo bufferResourceInfo;
					bufferResourceInfo.mBufferID = bufferID;

					bufferResourceInfo.mDimension = resourceType == "StructuredBuffer" ? Render::EShaderResourceDimension::eBuffer: 
						Render::EShaderResourceDimension::eNone;

					bufferResourceInfo.mName = userResourceName;

					bufferResourceInfo.mType = resourceType == "StructuredBuffer" ? Render::EShaderResourceType::eStructuredBuffer : Render::EShaderResourceType::eConstantBuffer;

					bufferResourceInfo.mIsEngineShaderResource = false;


					creationMaterialInfo.mShaderResourceInfoSet.mBufferShaderResourceInfoVector.push_back(std::move(bufferResourceInfo));


				}
				else if(resourceType=="Sampler")
				{
					Render::SamplerShaderResourceInfo samplerResourceInfo;
					samplerResourceInfo.mName = userResourceName;
					samplerResourceInfo.mType = Render::EShaderResourceType::eSampler;
					samplerResourceInfo.mDimension = Render::EShaderResourceDimension::eSampler;
					samplerResourceInfo.mIsEngineShaderResource = false;

					creationMaterialInfo.mShaderResourceInfoSet.mSamplerShaderResourceInfoVector.push_back(std::move(samplerResourceInfo));
				}
				/*	
			
			else
				{
					return false;
				}*/

				mJsonParser->OutReadPointer();
				ret = mJsonParser->NextReadPointer();

				if (ret == false)
				{
					mJsonParser->OutReadPointer();
					break;
				}
			}

		}
		else if (key == "UserShader")
		{


			mJsonParser->IntoReadPointer();


			std::string passKey =  mJsonParser->GetKeyFromReadPointer();


			if (passKey == "MainPass")
			{

				bool ret = mJsonParser->IntoReadPointer();
				if (!ret)
				{
					return false;
				}


				while (1)
				{
					std::string passConfigKey;
					passConfigKey = mJsonParser->GetKeyFromReadPointer();

					if (passConfigKey == "OPAQUE")
					{
						bool opaqueFlag = false;
						ret =mJsonParser->GetValueFromReadPointer<bool>(opaqueFlag);
						if (!ret)
						{
							return false;
						}
						creationMaterialInfo.mMainRenderPass.mOpaque = opaqueFlag;



					}
					else if (passConfigKey == "InputType")
					{
						int inputType = 0;
						ret = mJsonParser->GetValueFromReadPointer<int>(inputType);
						if (!ret)
						{
							return false;
						}
						creationMaterialInfo.mMainRenderPass.mInputType = inputType;


					}
					else if (passConfigKey == "HLSL")
					{

						mJsonParser->IntoReadPointer();




						std::string hlslFileKey = mJsonParser->GetKeyFromReadPointer();
						if (hlslFileKey != "File")
						{
							//log
							return false;
						}



						std::string hlslFile;
						bool ret =mJsonParser->GetValueFromReadPointer<std::string>(hlslFile);
						if (!ret)
						{
							return false;
							
						}

						
						while (mJsonParser->NextReadPointer())
						{
							std::string shaderEntryPointKey = mJsonParser->GetKeyFromReadPointer();


							if(shaderEntryPointKey == "VS")
							{
								std::string vsEntryPoint;
								bool ret =mJsonParser->GetValueFromReadPointer<std::string>(vsEntryPoint);
								
								if (!ret)
								{
									return false;
								}
								creationMaterialInfo.mMainRenderPass.mShaderStage.push_back({ nullptr,0, vsEntryPoint,"vs_5_0",Render::EShaderStage::eVertex });
							}
							else if (shaderEntryPointKey == "PS")
							{

								std::string psEntryPoint;
								bool ret = mJsonParser->GetValueFromReadPointer<std::string>(psEntryPoint);

								if (!ret)
								{
									return false;
								}
								creationMaterialInfo.mMainRenderPass.mShaderStage.push_back({ nullptr,0, psEntryPoint,"ps_5_0",Render::EShaderStage::ePixel });
							}
						}




						ret =mBinaryReader.StartRead(folderPath+"/"+hlslFile);
						if (!ret)
						{
							return false;
						}

						uint64_t fileSize = mBinaryReader.GetFileSize();

						std::vector<uint8_t> buffer(fileSize);
						ret =mBinaryReader.Read(buffer.data(),fileSize);
						if (!ret)
						{
							return false;
						}

						creationMaterialInfo.mMainRenderPass.mShaderCode = std::move(buffer);
						mBinaryReader.FlushBuffer();



						mJsonParser->OutReadPointer();


					}
					else if (passConfigKey == "RenderState")
					{
						mJsonParser->IntoReadPointer();


						while (1)
						{
							std::string renderStateConfigKey;
							renderStateConfigKey = mJsonParser->GetKeyFromReadPointer();


							if (renderStateConfigKey == "CullMode")
							{

								int cullMode = 0;
								bool ret = mJsonParser->GetValueFromReadPointer<int>(cullMode);
								if (!ret)
								{
									return false;
								}

								creationMaterialInfo.mMainRenderPass.mCullMode = (Render::ECullMode)cullMode;

							}
							else if (renderStateConfigKey == "FillMode")
							{
								int fillMode = 0;
								bool ret = mJsonParser->GetValueFromReadPointer<int>(fillMode);
								if (!ret)
								{
									return false;
								}

								creationMaterialInfo.mMainRenderPass.mFillMode = (Render::EFillMode)fillMode;



							}
							else if (renderStateConfigKey == "DepthCompareMode")
							{
								int depthCompareMode = 0;
								bool ret = mJsonParser->GetValueFromReadPointer<int>(depthCompareMode);
								if (!ret)
								{
									return false;
								}

								creationMaterialInfo.mMainRenderPass.mDepthCompareMode = (Render::EDepthStencilCompareMode)depthCompareMode;

							}
							else if (renderStateConfigKey == "BlendMode")
							{
								int blendMode = 0;
								bool ret = mJsonParser->GetValueFromReadPointer<int>(blendMode);
								if (!ret)
								{
									return false;
								}

								creationMaterialInfo.mMainRenderPass.mBlendMode = (Render::EBlendMode)blendMode;
							}
							else if (renderStateConfigKey == "DepthWriteMode")
							{
								int depthWriteMode = 0;
								bool ret = mJsonParser->GetValueFromReadPointer<int>(depthWriteMode);
								if (!ret)
								{
									return false;
								}

								creationMaterialInfo.mMainRenderPass.mDepthWriteMode = (Render::EDepthWriteMode)depthWriteMode;


							}

							if (mJsonParser->NextReadPointer() == false)
								break;

						}



						mJsonParser->OutReadPointer();



					}
					else if (passConfigKey == "Uses")
					{

						mJsonParser->IntoReadPointer();

						do
						{
							std::string resourceName;
							bool ret =mJsonParser->GetValueFromReadPointer<std::string>(resourceName);
							if (!ret)
							{
								return false;
							}

							creationMaterialInfo.mMainRenderPass.mUsageShaderResourceVector.push_back(resourceName);

						} while (mJsonParser->NextReadPointer());

						mJsonParser->OutReadPointer();

					}

					bool ret =mJsonParser->NextReadPointer();

					if (ret == false)
						break;
				}


				mJsonParser->OutReadPointer();
			}


			mJsonParser->OutReadPointer();

		}
		else if (key == "DefaultShader")
		{

		}
		else
		{

			return false;
		}


		if(!mJsonParser->NextReadPointer())
			break;


	}



	//mJsonParser->


















	//MaterialManager(Gpu버전)에게 gpu머터리얼생성요청
	if (mGpuMaterialManager->CreateMaterial(creationMaterialInfo) == MaterialIDNone)
	{
		return false;
	}


	return true;




}
