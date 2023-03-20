#pragma once
#include <DxLib.h>
#include "../Controller.h"

constexpr int motionRange = 30000;                                  // �X�e�B�b�N�̔�������

// �Q�[���p�b�h�p
class Pad :
	public Controller
{
public:
	Pad(int padType);
	~Pad();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool Init(void) override;

	/// <summary>
	/// �A�b�v�f�[�g
	/// </summary>
	/// <param name="delta"> �f���^ </param>
	void Update(float delta) override;

	/// <summary>
	/// �R���g���[���[�̃^�C�v�̎擾
	/// </summary>
	/// <param name=""></param>
	/// <returns> �R���g���[���[�̃^�C�v </returns>
	CntType GetCntType(void) override { return CntType::Pad; };

	/// <summary>
	/// �Q�[���p�b�h�̏����擾
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const DINPUT_JOYSTATE& GetPadState(void) const&
	{
		return state_;
	}

	const int GetPadType(void) const;
private:

	/// <summary>
	/// �J�[�\���̍��W���Z�b�g����
	/// </summary>
	/// <param name="pos"></param>
	void SetCursorPos(const Vector2& pos = lpSceneMng.screenSize_<float> / 2.0f) final;

	/// <summary>
	/// �v���C�X�e�[�V�����n�̃p�b�h�̉E�X�e�B�b�N�̍X�V
	/// </summary>
	/// <param name=""></param>
	void UpdatePsPad(float delta);

	/// <summary>
	/// xbox�n�̃p�b�h�̉E�X�e�B�b�N�̍X�V
	/// </summary>
	/// <param name=""></param>
	void UpdateXboxPad(float delta);

	// �Q�[���p�b�h�̏��
	DINPUT_JOYSTATE state_;

	// ���݂̃Q�[���p�b�h�̎��
	int nowPadType_;

	// �X�V����
	void(Pad::* update_)(float);
};

