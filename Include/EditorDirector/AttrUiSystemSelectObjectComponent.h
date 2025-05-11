#pragma once


#include"Predeclare.h"
#include"Component.h"
namespace Quad
{
	class AttrUiSystemSelectObjectComponent:public Component<AttrUiSystemSelectObjectComponent>
	{
	public:
		AttrUiSystemSelectObjectComponent() = default;
		~AttrUiSystemSelectObjectComponent() = default;

		void Update(float delta) override;
		
		void SetSelectedObject(Object* object);
		Object* GetSelectedObject() const;

		void SetSubMeshIndex(int index);
		int GetSubMeshIndex()const;

	private:
		Object* mSelectedObject =nullptr;
		int mSubMeshIndex =0;




	};

}
