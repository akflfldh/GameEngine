#pragma once




#include"Predeclare.h"
#include"Object/UiEntity.h"
#include"TextCursorEntity.h"
#include<string>

#include"harfBuzzsrc/hb.h"
#include"Font.h"
#include<functional>

#include"Object/UiEntity.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	struct TextLineInfo
	{
		int characterNum = 0;
		int characterEntityFirstIndex = 0;
		int characterEntityLastIndex = 0;

	};


	enum class ETextFilterMode
	{
		//어떤 글자들을 마스킹할것인가(걸러낼것인가)
		eNone=0,
		eAlphabet,
		eDigit
	};


	//멀티라인이면 여러라인이존재할수있고 한라인의범위를벗어난 문자들은 다음라인으로 내려간다.
	//One라인이면 계속 한라인에서 글자들이 위치한다.
	enum class ETextLineMode
	{
		eMultiLine = 0,
		eOneLine
	};




	class CORE_API_LIB TextBox:public UiEntity
	{
	public:
		TextBox(const std::string& name="");
		virtual ~TextBox() ;

		virtual void Initialize() override;

		static TextBox* Create(Map* map, int mapLayerIndex, int maxCharacterNum, ETextLineMode textLineMode = ETextLineMode::eMultiLine, int fontSizeIndex = 0, Font* mFont = nullptr);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		virtual void SetSize(float width, float height) override;
		CLASSNAME(TextBox)
		CLASSSIZE(TextBox)

		void SetText(const std::string& text);
		std::string GetText() const ;

		void SetTextColor(const DirectX::XMFLOAT3& color);
		const DirectX::XMFLOAT3& GetTextColor() const;

		void SetAlignmentDirection(ETextAlignmentDirection dir);
		ETextAlignmentDirection GetAlignmentDirection()const;


		virtual void SetStencilRefValue(unsigned char value) override;

		void SetDrawFlag(bool flag) override;
		void SetTextDrawFlag(bool flag);
		bool GetTextDrawFlag()const;

		void SetTextEditMode(bool state);
		bool GetTextEditMode() const;

		void ResetVisibleLine();


		void SetEnterCallBack(std::function<void()>callback); 
		void SetEnterEndFlag(bool flag);
		bool GetEnterEndFlag()const;


		void SetTextFilterMode(ETextFilterMode filterMode);
		ETextFilterMode GetTextFilterMode() const;
		
		ETextLineMode GetTextLineMode() const;

		void SetOnlyEnglishFlag(bool flag);
		bool GetOnlyEnglishFlag() const;

		void SetOnlyUpperCaseFlag(bool flag);
		bool GetOnlyUpperCaseFlag()const;

 	private:


	
		//virtual void HandleLButtonDown(Event* pEvent);
	//	virtual void HandleLButtonUp(Event* pEvent);

		virtual void HandleHover(Event* pEvent) override;
		void HandleKeyDown(Event* pEvent);
		void HandleLButtonDown(Event* pEvent) override;
		void HandleMouseMove(Event* pEvent);
		void HandleLButtonUp(Event* pEvent) override;
		void HandleWindowActive(Event* pEvent);

		void SetOneLineFlag(bool flag);
		bool GetOneLineFlag() const;

	private:

		void MoveCursorByArrowKey(int key);
		void InsertCharacter(UNICODE_VALUE newCharacter);
		void DeleteCharacter();

		void HanguleCharacterProcess(KeyEvent* keyEvent);



	
		void ReBuildCharacterPositionLeftAlignment(const GlyphMetrics* glyphMetrics, int  characterEntityIndex);
		void CenterAlignment(int  characterEntityIndex);
		void RightAlignment(int  characterEntityIndex);

		void ReBuildCharacterMetrics(int characterOffset);

		void SetLineHeight(float height);
		float GetLineHeight();

		void SetBaseLinePanelCharacterPosY(float y);
		float GetBaseLinePanelCharacterPosY() const;

		// 다음 파라미터의 의미는 text안 문자들은 여전히 모두 초기화, 빠르게 재사용을위해 characterEntity를 맵에서 제거하지않을뿐
		void Reset(const std::wstring& newText);




		void InitNewCharacter(TextCharacterPanelEntity* newCharacterPanelEntity, wchar_t ch);
		//현재 커서만 몇번라인에있는지를 계산하는헬퍼함수
		int GetCurrentCursorLineNum() const;
		int GetLineNum(int positionInText)const;
		void SetCursorPosition(int positionInText);


		//커서위치에따라서 visible라인조정
		void SetVisibleLine();


		void SetCursorPosition(MouseEvent* mouseEvent);
		void ReSetDragRegion();


		

		//입력된 글자를 filter mode에 맞추어 검사한다.
		bool CheckFilterMode(UNICODE_VALUE newCharacter);

		//oneLine모드에서 텍스트박스안에있는 글자들은 draw , 그렇지않은 글자들은 draw false
		void SetChracterDrawInOneLine();


	

	protected:
		void InitCreating(int maxCharacterNum, ETextLineMode textLineMode, int fontSizeIndex, Font* font);

	
	private:
		Font* mFont;
		std::wstring mText;
		std::vector<TextCharacterPanelEntity*> mTextCharacterPanelEntityVector;
		TextCursorEntity* mTextCursorEntity;
		DirectX::XMFLOAT3 mTextColor;

		std::vector<Quad::TextLineInfo> mLineInfoVector;
		ETextAlignmentDirection mAlignmentDirection;

		int mMaxCharacterNum;

		
		//텍스트박스너비만큼 글자가 차면 다음라인으로 넘어가는가? 아니면 계속 한라인으로 유지되는가?
		bool mOneLineFlag; 

		//항상왼쪽정렬기준으로 설정한시작지점
		float mCharacterPanelStartLocalPosX;
		float mCharacterPanelStartLocalPosY;

		
		float mLineHeight;
		//ascender - descender 에 곱한다
		float mLineHeightScaleFactor =0.8f;
		//CharacterPanel에서의 baseline의 로컬좌표
		float mBaseLinePanelLocalPosY;

		int mVisibleLineNum;
		int mVisibleLineStartIndex;

		//drag

		//항상 드래그가없으면 -1로 초기화
		int mDragCursorStartPositionInText = -1;
		
		bool mTextEditMode = false;

		bool mTextDrawFlag = true;










		//int mDragCursorCurrentEndPositionInText = -1;

		//harfbuzz
		hb_buffer_t* mHbBuffer =nullptr;








		//callback

		bool mEnterEndFlag = false;
		std::function<void()> mEnterCallback;
		


		
		ETextFilterMode mTextFilterMode;
		ETextLineMode mTextLineMode;





		bool mOnlyEnglishFlag;//true이면 영어만, false이면 한글도 들어온다.
		bool mOnlyUpperCaseFlag; //true이면 알파벳은 대문자만 들어온다.
	};
	REGISTERCLASS(TextBox)

}