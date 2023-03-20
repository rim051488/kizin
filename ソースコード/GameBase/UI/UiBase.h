#pragma once
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "../Common/Vector2.h"
#include "../Common/SharedHandle.h"
#include "UiID.h"
#include <filesystem>

class ObjectManager;
class Controller;
class BaseScene;

class UiBase
{
public:
	UiBase(const Vector2& pos);
	virtual ~UiBase() = default;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="delta"> �f���^�^�C�� </param>
	/// <param name="scene"> �V�[�� </param>
	/// <param name="objMng"> �I�u�W�F�N�g�}�l�[�W���[ </param>
	/// <param name="controller"> �R���g���[���[ </param>
	virtual void Update(float delta, BaseScene& scene,ObjectManager& objMng, Controller& controller) = 0;

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="mainScr"></param>
	virtual void Draw(int mainScr) = 0;

	/// <summary>
	/// ID�̎擾
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	virtual UiID GetUIID(void) = 0;

	/// <summary>
	/// ���[�h�I�����̏���
	/// </summary>
	/// <param name="scene"></param>
	virtual void Loaded(BaseScene& scene);

	/// <summary>
	/// �{�^���Ƃ��ċ@�\���邩(ButtonBase���p�����Ă��邩�H)
	/// </summary>
	/// <param name=""></param>
	/// <returns> �{�^���̎�true�A�����łȂ��Ƃ�false </returns>
	virtual bool IsButton(void) const
	{
		return false;
	}

	/// <summary>
	/// ���W�̎擾
	/// </summary>
	/// <param name=""></param>
	/// <returns> ���W </returns>
	const Vector2& GetPos(void) const&
	{
		return pos_;
	}

	/// <summary>
	/// �T�C�Y�̎擾(�c��)
	/// </summary>
	/// <param name=""></param>
	/// <returns> �T�C�Y(�c��) </returns>
	const Vector2& GetSize(void) const&
	{
		return size_;
	}

	/// <summary>
	/// �`�悷�ׂ����̃t���O���Z�b�g����
	/// </summary>
	/// <param name="flag"></param>
	void SetDrawFlag(bool flag)
	{
		isDraw_ = flag;
	}


protected:
	// ���W
	Vector2 pos_;

	// �T�C�Y
	Vector2 size_;

	// �`�悷�ׂ����̃t���O
	bool isDraw_;
};

