#include "Utility/UniqueIDTable.h"
//#include"LogicalContentItemUiEntityFolder.h"


namespace Quad
{

	//template<typename T>
	//inline void UniqueIDTable<T>::Register(T* element)
	//{
	//	unsigned long long id = mIDGenerator.GetNewID();
	//	element->SetUniqueID(id);

	//	mTable[id] = element;
	//}
	//template<typename T>
	//inline void UniqueIDTable<T>::UnRegister(T* element)
	//{
	//	unsigned long long id = element->GetUniqueID();
	//	UnRegister(id);
	//}
	//template<typename T>
	//void UniqueIDTable<T>::UnRegister(unsigned long long id)
	//{
	//	//mIDGenerator.ReturnID(id);
	//	mTable.erase(id);


	//}
	//template<typename T>
	//inline T* UniqueIDTable<T>::GetElement(unsigned long long id) const
	//{


	//	typedef typename std::unordered_map<unsigned long long, T*>::const_iterator const_iterator;
	//	const_iterator itr = mTable.find(id);

	//	return itr != mTable.cend() ? itr->second : nullptr;
	//}

	//template<typename T>
	//const std::unordered_map<unsigned long long, T*>& UniqueIDTable<T>::GetTable() const
	//{
	//	return mTable;
	//	// TODO: 여기에 return 문을 삽입합니다.
	//}

	//template<typename T>
	//void UniqueIDTable<T>::RegisterFromFile(T* element, unsigned long long id)
	//{
	//	element->SetUniqueID(id);
	//	mTable[id] = element;
	//	return;
	//}

	//template<typename T>
	//unsigned long long UniqueIDTable<T>::PeekUniqueID() const
	//{
	//	return mIDGenerator.PeekNewID();
	//}

	//template<typename T>
	//void UniqueIDTable<T>::SetNextAvailalbeUniqueID(unsigned long long id)
	//{
	//	mIDGenerator.SetNextAvailableID(id);

	//}

	


//
//template class UniqueIDTable<Object>;
//template class UniqueIDTable<Effect>;
//template class UniqueIDTable<Material>;
//template class UniqueIDTable<Mesh>;
//template class UniqueIDTable<Texture>;
//template class UniqueIDTable<LogicalContentItemUiEntityFolder>;
//
//
//



}