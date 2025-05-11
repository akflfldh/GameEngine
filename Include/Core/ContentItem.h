#pragma once


#include"Predeclare.h"
#include<string>

#include"Core/CoreDllExport.h"


namespace Quad
{
	class CORE_API_LIB ContentItem
	{
	public:
		ContentItem(const std::string& name,EContentItemType contentItemType);
		virtual ~ContentItem() = 0;

		void SetName(const std::string& name);
		void SetName(std::string&& name);
		const std::string& GetName() const;

		EContentItemType GetEContentItemType() const;

		virtual void Serialize(const std::string & tag= "");
		virtual void DeSerialize(const std::string& tag = "");


		void SetEngineContentItemFlag(bool flag);
		bool GetEngineContentItemFlag() const;


		bool Copy(ContentItem* source);
		virtual bool CheckType(const ContentItem * source );


	protected:

		bool virtual InnerCopy(const ContentItem* source);


	private:
		std::string mName;
		EContentItemType mContentItemType;

		bool mIsEngineContentItem;

	};

}

