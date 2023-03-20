#pragma once
#include <memory>
#include <unordered_map>
#include <forward_list>
#include "ComponentID.h"
#include "ComponentConcept.h"

#include "../Common/Debug.h"

class ComponentBase;

// �g���񂵂����R���|�[�l���g���v�[�����邽�߂̃N���X
class ComponentPool
{
	using CompUptr = std::unique_ptr<ComponentBase>;
	using CompFList = std::forward_list<CompUptr>;
public:
	ComponentPool();
	~ComponentPool();

	/// <summary>
	/// �v�[������w��̌^�����o��
	/// </summary>
	/// <typeparam name="T"> ���o���R���|�[�l���g�̌^ </typeparam>
	/// <param name=""></param>
	/// <returns> ���o�����R���|�[�l���g </returns>
	template<CompC T>
	std::unique_ptr<T> Pop(void);

	/// <summary>
	/// �R���|�[�l���g���v�[���Ɋi�[����
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="comp"> �i�[����R���|�[�l���g </param>
	template<CompC T>
	void Push(std::unique_ptr<T>&& comp);

private:

	// �R���|�[�l���g�̃v�[��
	std::unordered_map<ComponentID, CompFList> compPool_;
};

template<CompC T>
inline std::unique_ptr<T> ComponentPool::Pop(void)
{
	if (!compPool_[T::id_].empty())
	{
		// �v�[�����󂾂����琶������
		auto ptr = compPool_[T::id_].front().get();
		compPool_[T::id_].front().release();
		compPool_[T::id_].pop_front();
		//DebugLog("Pop");
		return std::unique_ptr<T>{static_cast<T*>(ptr)};
	}
	else
	{
		// ���������琶������
		return std::move(std::make_unique<T>());
	}
}

template<CompC T>
inline void ComponentPool::Push(std::unique_ptr<T>&& comp)
{
	compPool_[comp->GetID()].emplace_front(std::move(comp));
}
