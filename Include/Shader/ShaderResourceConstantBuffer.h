#pragma once
#include "Shader/ShaderResource.h"
#include<vector>
#include"header.h"
#include"Object/ObjectType.h"
#include"Core/CoreDllExport.h"

namespace Quad
{
 

    struct   ShaderResourceVariable
    {
        std::wstring mName;
        unsigned int mSize;
        unsigned int mOffsetInShaderResource;
        unsigned int mElementNum = 1;//일반적인 변수나 , 원소가 하나뿐인 배열은 1 , 그외에는 원소가 2이상인 배열(1차웜만일단고려)
        //EShaderResourceVariableType mVariableType;
        D3D_SHADER_VARIABLE_CLASS mClass;
        std::wstring mTypeName; //일반적인 타입이라면 사용할일이없겠으나, (디버깅출력용말고)
        //구조체의경우에는 구조체에 접근할때 사용할것.
    };

    struct  ShaderResourceStructTypeDesc
    {
        std::wstring mName; //shaderResourceVariable mTypeName 동일
        std::vector< ShaderResourceVariable> mVariableVector;
        unsigned int mSize;
        unsigned int mAlignedSize;

    };


   class  CORE_API_LIB ShaderResourceConstantBuffer :
        public ShaderResource
    {
    public:
        ShaderResourceConstantBuffer(const std::wstring& name,char registerIndex, char registerSpace);
        ~ShaderResourceConstantBuffer() = default;

        ShaderResourceConstantBuffer(const ShaderResourceConstantBuffer& shaderResourceConstantBuffer);

       
        void Initialize(const std::vector<ShaderResourceVariable>& variableVector, UINT dataSize);
        void Initialize(std::vector<ShaderResourceVariable>&& variableVector, UINT dataSize);

        void SetConstantBuffer(ConstantBuffer* constantBuffer);
       
        //upload 할때마다 upload카운트가 하나씩증가 ,업로드할때 같은 렌더패스의 같은 데이터는 연속적으로 upload해야한다.
        void UploadData(RenderItem* renderItem, const PassData& passData);
        void UploadLightStructData(char* dataTempBuffer, const ShaderResourceStructTypeDesc* pShaderResourceStructTypeDesc, int elementNum, int structOffset, const PassData& passData, ELightType lightType);

        void Reset();//0으로 초기화 -매프레임마다 해야할것
        ViewIndex GetViewIndex(UINT uploadIndex) const;
        
        void AddShaderResourceStructDesc(ShaderResourceStructTypeDesc* structTypeDesc);
        const ShaderResourceStructTypeDesc* GetShaderResourceStructDesc(const std::wstring& name)const;


        const std::vector<ShaderResourceVariable>& GetShaderResourceVariableVector() const;
        void UploadData(int variableIndex, void* data, size_t size= 0);
        void UploadStructData(int variableIndex, int elementIndex, int structAlignSize, int offset, void* data, int dataSize);
        void EndUploadData();


    private:
        std::vector<ShaderResourceVariable> mVariableVector;
        std::unordered_map<std::wstring, std::shared_ptr<ShaderResourceStructTypeDesc>> mStructTypeDescUnMapTable;
        ConstantBuffer* mConstantBuffer;//공통의 상수버퍼
        UINT  mConstantBufferOffset;//상수버퍼내에 시작offset값.
        UINT  mCurrenntUploadCount;
        UINT mDataSize;//상수버퍼 총 사이즈
        char* mDataTempBuffer;

        std::vector<ViewIndex> mViewIndexVector;
       
    };



}

