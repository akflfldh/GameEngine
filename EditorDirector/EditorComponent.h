#pragma once



#include"BaseComponent.h"

namespace Quad
{

	//3d를 위한 editorComoonent
	class EditorComponent:public BaseComponent
	{
	public:
		void SetSelectKeepingFlag(bool flag);
		bool GetSelectKeepingFlag()const;

		void SetSelectAvailableFlag(bool flag);
		bool GetSelectAvailableFlag()const;

		void SetSelectBlockFlag(bool flag);
		bool GetSelectBlockFlag()const;

		void SetEntireSelectAvailableFlag(bool flag);
		bool GetEntireSelectAvailableFlag()const;


		virtual void SetDrawFlag(bool flag);
		bool GetDrawFlag()const;

		void SetEntireDrawFlag(bool flag);
		bool GetEntrieDrawFlag()const;


		virtual void SetActiveFlag(bool flag);
		bool GetActiveFlag()const;

		void SetSelectFlag(bool flag);
		void SendSelectEvent(bool state, bool exclusiveEventHandleFlag = false);
		bool GetSelectFlag()const;

		//유저는 건들지 말것 (기즈모 등를위한것)
		bool GetEnginObjectFlag() const;
		void SetEngineObjectFlag(bool flag);
	private:


		//bool mDrawFlag;
		//bool mEntireDrawFlag;
		bool mActiveFlag;
		bool mSelectFlag;


		bool mSelectKeepingFlag = true;

		bool mSelectAvailableFlag = true;

		bool mSelectBlockFlag = true;

		bool mEntireSelectAvailableFlag = true;

		bool mIsEngineObject = false;

	};

}
