#pragma once

#include"Predeclare.h"
#include"Core/ContentItem.h"

#include<string>

//#include"Object/Macro.h"
#include"Utility/UniqueIDTable.h"

#include"Core/CoreDllExport.h"


#include<windows.h>

namespace Quad
{
	/*REGISTERCLASS(Asset)*/
	class CORE_API_LIB Asset:public ContentItem
	{

		friend class UniqueIDTable<Mesh>;
		friend class UniqueIDTable<Material>;
		friend class UniqueIDTable<Texture>;
		friend class UniqueIDTable<Skeleton>;
		friend class UniqueIDTable<AnimationClip>;
	public:
		Asset(const std::string& name ,EAssetType assetType);
		virtual ~Asset() = 0;


		EAssetType GetAssetType() const;

		unsigned long long GetUniqueID() const;

		unsigned long long GetDestLogicalFolderUnqiueID() const;
		void SetDestLogicalFolderUnqiueID(unsigned long long id);

		virtual void Serialize(const std::string & tag="") override;
		virtual void DeSerialize(const std::string & tag="") override;

		virtual bool CheckType(const ContentItem* source)override;

	protected:

		virtual bool InnerCopy(const ContentItem* source) override;


	private:
		void SetUniqueID(unsigned long long id);


		EAssetType mAssetType;
		unsigned long long mUniqueID;

		//asset이 속해있는 논리적폴더의 식별자 
		unsigned long long mDestLogicalFolderUniqueID;
		
	
	};

}

