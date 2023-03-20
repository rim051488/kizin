#pragma once
#include <map>
#include <array>
#include <string>
#include "InputID.h"
#include "../Vector2.h"
#include "../../SceneManager.h"

// �g���K�[���
enum class Trg
{
	Now,				// ����
	Old,				// ��O
	Max
};

// ���͑��u�̎��
enum class CntType
{
	Key,				// �L�[�{�[�h
	Pad,				// �p�b�h
	Max
};

using InputData = std::array<std::pair<bool, bool>, static_cast<size_t>(InputID::Max) + 1>;

class Controller
{
public:
	bool operator()(float delta)
	{
		if (this != nullptr)
		{
			Update(delta);
		}
		return this != nullptr;
	}
	Controller();
	virtual ~Controller();

	/// <summary> ������ </summary>
	/// <returns> ������true���s��false </returns>
	virtual bool Init(void) = 0;

	/// <summary> �X�V </summary>
	/// <param name="delta"> �f���^�^�C�� </param>
	virtual void Update(float delta) = 0;

	/// <summary> �R���g���[���[�̎�ނ̎擾 </summary>
	/// <returns> �R���g���[���[�̎�� </returns>
	virtual CntType GetCntType(void) = 0;

	/// <summary> �����N���b�N���Ă��邩 </summary>
	/// <param name="id">�L�[�̎��</param>
	/// <returns>��������true���s����false</returns>
	//bool MousePress(InputID id);

	/// <summary> �������Ă��邩 </summary>
	/// <param name="id"> �L�[�̎�� </param>
	/// <returns> ������true���s��false </returns>
	bool Press(InputID id);

	/// <summary> �������u�� </summary>
	/// <param name="id"> �L�[�̎�� </param>
	/// <returns> ������true���s��false </returns>
	bool Pressed(InputID id);

	/// <summary> �������u�� </summary>
	/// <param name="id"> �L�[�̎�� </param>
	/// <returns> ������true���s��false </returns>
	bool Released(InputID id);

	/// <summary> ��������Ă��Ȃ��Ƃ� </summary>
	/// <param name="id"> �L�[�̎�� </param>
	/// <returns> ������true���s��false </returns>
	bool NotPress(InputID id);

	/// <summary> ������������Ă��Ȃ��� </summary>
	/// <returns> ������true���s��false </returns>
	bool IsNeutral();

	/// <summary>
	/// �J�[�\���ʒu���Z�b�g(�f�t�H���g�����͒��S)
	/// </summary>
	/// <param name="pos"> �J�[�\���̈ʒu </param>
	virtual void SetCursorPos(const Vector2& pos = lpSceneMng.screenSize_<float> / 2.0f) = 0;

	/// <summary>
	/// �J�[�\���ʒu�̎擾
	/// </summary>
	/// <param name=""></param>
	/// <returns> �J�[�\���̍��W </returns>
	const Vector2& GetCursorPos(void) const&
	{
		return cursorPos_;
	}


	/// <summary>
	/// �J�[�\���̃X�s�[�h���Z�b�g
	/// </summary>
	/// <param name="speed"> �X�s�[�h </param>
	void SetCursorSpeed(float speed)
	{
		cursorSpeed_ = speed;
	}


	/// <summary>
	/// ��(�ړ�)�p
	/// </summary>
	/// <param name=""></param>
	/// <returns> ���͂��ꂽ���� </returns>
	const Vector2& GetLeftInput(void) const&
	{
		return leftInput_;
	}

	/// <summary>
	/// ���_�ړ��Ɏg�����W���擾
	/// </summary>
	/// <param name=""></param>
	/// <returns> ���S����̈ړ�������ʏ�̍��W </returns>
	const Vector2& GetRightInput(void) const&
	{
		return rightInput_;
	}

	/// <summary>
	/// ����{�^����������Ă��邩
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool PressDecision(void) const;

	/// <summary>
	/// ����{�^���������ꂽ�u�Ԃ�
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool PressedDecision(void) const;

	/// <summary>
	/// �L�����Z���{�^���������ꂽ��
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool PressCancel(void) const;

	/// <summary>
	/// �L�����Z���{�^���������ꂽ�u�Ԃ�
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool PressdCancel(void) const;
protected:
	/// <summary> ���͏�� </summary>
	//CntData cntData_;
	InputData data_;

	// ���艺���̏��̏��
	std::pair<bool, bool> decisionData_;

	// �L�����Z�����������̃f�[�^
	std::pair<bool, bool> cancelData_;

	// �J�[�\���̍��W
	Vector2 cursorPos_;

	// �J�[�\���̃X�s�[�h(ui���Ɏg�p)
	float cursorSpeed_;

	// �ړ��p�̓��͂��ꂽ����
	Vector2 leftInput_;

	// �U������p���͍��W
	Vector2 rightInput_;

};

