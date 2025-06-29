#pragma once






#include<unordered_map>
#include<functional>
#include"InputType.h"
#include<string>
#include<vector>

#include<Core/CoreDllExport.h>
namespace Quad
{

	class CORE_API_LIB InputSystem
	{
	public:
		static InputSystem* GetInstance();
		~InputSystem();


		//win proc에서 호출될것이다.
		static void OnMouseEvent(EMouseInput mouseInput,int channel);


		static void RegisterMouseAction(EMouseInput mouseInput,const std::string  &actionTag ,std::function<void()> callback,int channel = 0);
		template<typename T>
		static void RegisterMouseAction(EMouseInput mouseInput, const std::string& actionTag, T* object, void(T::* callback)(), int channel = 0);

	private:
		InputSystem() = default;

											//키(tag = "Move") 
					//int : channel 
		std::unordered_map<int,	std::unordered_map < EMouseInput, std::unordered_map<std::string, std::vector<std::function<void()>>>>> mMouseInputTable;


		

	};



	template<typename T>
	inline void InputSystem::RegisterMouseAction(EMouseInput mouseInput, const std::string& actionTag, T* object, void(T::* callback)(), int channel)
	{
		RegisterMouseAction(mouseInput, actionTag, std::bind(callback, object),channel);
	}


}