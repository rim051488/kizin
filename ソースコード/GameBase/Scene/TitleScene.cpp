#include <DxLib.h>
#include "TitleScene.h"
#include "Transition/BackLoading.h"
#include "../SceneManager.h"
#include "../Shader/PEManager.h"
#include "../Shader/PEID.h"
#include "../Common/Camera.h"
#include "../Common/ResourceMng.h"
#include "../Common/SoundPross.h"
#include "SelectScene.h"
#include "../UI/UiManager.h"
#include "../Object/ObjectManager.h"
#include "../Component/Render/ModelRender.h"
#include "../Component/Transform/Transform.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Behavior/CameraBehavior.h"
#include "../Common/Debug.h"

constexpr float logoUiOffTIme{ 0.5f };

TitleScene::TitleScene() :
	BaseScene{ScreenID::Title,SceneID::Title},
	update_{&TitleScene::UpdateNon}, draw_{&TitleScene::DrawNon}, stepTime_{0.0f}
{
	SetMakeSceneFunc(std::bind(&TitleScene::MakeGameFunc, this, std::placeholders::_1), SceneID::Select);
	
	peMng_ = std::make_unique<PEManager>();
	
	uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/title.ui", true, false, false);

	// シャドウマップ
	int x, y;
	GetDrawScreenSize(&x, &y);
	shadowMap_ = MakeScreen(x, y, false);
	// シャドウマップ用カメラ情報の初期化
	SetUseASyncLoadFlag(false);
	shadowBuff_ = CreateShaderConstantBuffer(sizeof(LIGHT_MAT) * 4);
	SetUseASyncLoadFlag(true);
	lightMat_ = static_cast<LIGHT_MAT*>(GetBufferShaderConstantBuffer(shadowBuff_));
	lightMat.view = MGetIdent();
	lightMat.proj = MGetIdent();



	CreateBackGround();

	AddLoadedFunc(std::bind(&TitleScene::Loaded, this, std::placeholders::_1));

	// タイトルシーン用にロードしたサウンドを呼びだす
	lpSooundPross.Init(SceneID::Title);

	// タイトルシーンのBGMを再生
	lpSooundPross.PlayBackSound(SOUNDNAME_BGM::TitleSceneBGM,true,false);
}

BaseScene::SceneUptr TitleScene::MakeGameFunc(SceneUptr own)
{
	return std::make_unique<BackLoading>(std::move(own), std::make_unique<SelectScene>(std::move(objMng_)), 2.0f);
}

void TitleScene::Update(float delta, Controller& controller)
{
	(this->*update_)(delta, controller);
}

void TitleScene::DrawScene(void)
{
	SetupShadowMap();
	SetDrawScreen(*screenHandle_);
	ClsDrawScreen();
	lightMat_[0] = lightMat;
	(this->*draw_)();
	MV1SetUseOrigShader(false);
	// 影の描画

	// 描画

	// ポストエフェクト
	//peMng_->SetFlag(PEID::Default, true);

	//objMng_->Draw();
	// ここに直書きしているが後から変えること
	//DrawFormatString(0, 0, 0xffffff, TEXT("%dIDのシーンです"), static_cast<unsigned int>(scID_));
}

bool TitleScene::IsLoaded(void)
{
	return BaseScene::IsLoaded() && uiMng_->IsLoaded();
}

void TitleScene::UpdateNon(float delta, Controller& controller)
{
	objMng_->Update(delta, controller, *this);
	uiMng_->Update(delta, *this, *objMng_, controller);
	if (CheckHitKeyAll())
	{
		update_ = &TitleScene::UpdateLogoOff;
		draw_ = &TitleScene::DrawLogoOff;
	}
}

void TitleScene::UpdateLogoOff(float delta, Controller& controller)
{
	if ((stepTime_ += delta) >= logoUiOffTIme)
	{
		ChangeSceneID(SceneID::Select);
	}
}

void TitleScene::SetupShadowMap(void)
{
	// 描画先を影用深度記録画像に変更する
	SetDrawScreen(shadowMap_);
	// 影用深度記録画像を一度真っ白にする
	SetBackgroundColor(255, 255, 255);
	ClsDrawScreen();
	SetBackgroundColor(0, 0, 0);

	// シャドウマップ用にカメラをセット
	camera_->SetUpShadow(offsetOrtho, offsetNear, offsetFar, camTar);

	MV1SetUseOrigShader(true);
	objMng_->SetupDepthTex(*shadowPs_, -1);
	MV1SetUseOrigShader(false);

	lightMat.view = GetCameraViewMatrix();
	lightMat.proj = GetCameraProjectionMatrix();
}

void TitleScene::DrawLogoOff(void)
{
	camera_->SetUpScreen();
	objMng_->ShadowDraw(shadowMap_, shadowBuff_);
	auto alpha =1.0f - stepTime_ / logoUiOffTIme;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.0f * alpha));
	uiMng_->Draw(*screenHandle_);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void TitleScene::DrawNon(void)
{
	camera_->SetUpScreen();
	objMng_->ShadowDraw(shadowMap_, shadowBuff_);
	uiMng_->Draw(*screenHandle_);
}

void TitleScene::CreateBackGround(void)
{
	objMng_ = std::make_unique<ObjectManager>(1ull);

	auto id = objMng_->MakeObjectID();

	auto render = std::make_unique<ModelRender>();
	static_cast<Render&>(*render).Load("Resource/resource/Title/Title.mv1");
	render->SetNonShader();
	objMng_->AddComponent(std::move(render), id);

	auto transform = std::make_unique<Transform>();
	transform->Scale() = Vector3{ 1.0f,1.0f,1.0f };
	objMng_->AddComponent(std::move(transform), id);

	objMng_->AddComponent(std::make_unique<ObjectInfo>(), id);

	camera_->SetRotation(Quaternion(0.0f, Deg2Rad(180.0f), 0.0f));
	camera_->SetPos(Vector3(0.0f, 200.0f, 200.0f));
	SetLightDirection(VGet(0.0f, -1.0f, 0.0f));
	SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	SetLightSpcColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	SetLightAmbColor(GetColorF(0.5f, 0.5f, 0.5f, 1.0f));

	//auto camera = objMng_->MakeObjectID();
	//objMng_->AddComponent(std::make_unique<ObjectInfo>(), camera);
	//
	//objMng_->AddComponent(std::make_unique<Transform>(), camera);
	//objMng_->GetComponent<Transform>(camera)->Pos() = Vector3{ 0.0f, 200.0f, 200.0f };
	//objMng_->GetComponent<Transform>(camera)->SetRotation({ 0.0f,Deg2Rad(180.0f) ,0.0f });
	//auto cameraBehavior = std::make_unique<CameraBehavior>();
	//cameraBehavior->SetSyncID(objMng_->GetPlayerID());
	//cameraBehavior->SetType(CameraBehavior::Type::Non);
	//objMng_->AddComponent(std::move(cameraBehavior), camera);
	//objMng_->SetCameraID(camera);

}

void TitleScene::Loaded(Controller& controller)
{
	lpSceneMng.GetResourceMng().Loaded();
	objMng_->Begin();
	objMng_->Update(0.0f, controller, *this);
	uiMng_->Begin(*this);
}

