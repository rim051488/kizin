#include "PlayerAttackBehavior.h"
#include "../../Object/ObjectManager.h"
#include "../Info/ObjectInfo.h"
#include "../../Factory/PlayerAttackFactory.h"

#include "../../Common/Debug.h"
#include "PlayerBehavior.h"
#include "EnemyBehavior.h"
#include "../../UI/UiID.h"


PlayerAttackBehavior::PlayerAttackBehavior() :
	combo_{ -1 }, attackTime_{ 0.0f }, attackStartTime_{ 0.0f }, animTime_{ 0.0f }, frameIdx_{ -1 }, update_{ nullptr }
{
	for (int i = 0; i < static_cast<int>(FactoryID::Max); i++)
	{
		effect_.push_back(std::make_tuple(static_cast<FactoryID>(i), zeroVector3<float>, false));
	}
	delta_ = 0.0f;
	hitTime_ = 0.0f;
	isAttackInterval_ = false;
	DebugLog("生成");
}

void PlayerAttackBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	delta_ = delta;

	// 攻撃のボーンの座標に合わせる
	if (playerModel_.IsActive())
	{
		auto pos = MV1GetFramePosition(*playerModel_->GetHandle(), frameIdx_);
		transform_->Pos() = Vector3{ pos.x, pos.y, pos.z };
	}

	auto player = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());
	if (player.IsActive())
	{
		transform_->SetRotation({ Deg2Rad(90.0f),0.0f,-player->GetRotation().ToEuler().y});
	}

	(this->*update_)(objectManager, delta);

	if (isAttackInterval_)
	{
		hitTime_ += delta_;
		if (hitTime_ >= 2.0f)
		{
			// ↓攻撃できなくなるバグの原因おそらくこれ
			//auto playerBehavior = objectManager.GetComponent<PlayerBehavior>(objectManager.GetPlayerID());
			//playerBehavior->ResetCombo(objectManager);
			isAttackInterval_ = false;
		}
	}
}

void PlayerAttackBehavior::Effect(ObjectManager& objectManager)
{
	auto player = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());
	if (!player.IsActive())
	{
		return;
	}

	// エフェクト関係
	for (auto& e : effect_)
	{
		auto& flag = std::get<2>(e);
		if (flag)
		{
			auto& factory = std::get<0>(e);
			auto& pos = std::get<1>(e);
			auto id = objectManager.CreateFromFactory(factory, objectManager.GetPlayerID(), player->GetPos() + (player->GetRotation() * pos), {0.0f,player->GetRotation().ToEuler().y ,0.0f});
			objectManager.Begin(id);
			flag = false;
		}
	}
}

void PlayerAttackBehavior::AddEffect(FactoryID id, ObjectManager& objectManager, const Vector3& offset)
{
	// エフェクトの種類取得
	auto& effect = effect_.at(static_cast<int>(id));
	// 座標格納
	auto& pos = std::get<1>(effect);		// 座標格納場所
	//auto frameIdx = MV1SearchFrame(*playerModel_->GetHandle(), frameNameCenter_.c_str());			// 基準にするフレーム
	//pos = MV1GetFramePosition(*playerModel_->GetHandle(), frameIdx);								// 座標格納

	pos = offset;

	// 発動フラグ
	auto& flag = std::get<2>(effect);
	flag = true;
}

void PlayerAttackBehavior::Begin(ObjectManager& objectManager)
{
	auto player = (objectManager.GetComponent<Transform>(objectManager.GetPlayerID()));
	playerModel_ = objectManager.GetComponent<ModelRender>(objectManager.GetPlayerID());
	transform_ = objectManager.GetComponent<Transform>(ownerId_);

	frameNameBlade_ = L"Blade-R";
	frameNameCenter_ = L"Center";
	frameIdx_ = MV1SearchFrame(*playerModel_->GetHandle(), frameNameBlade_.c_str());
	auto pos = MV1GetFramePosition(*playerModel_->GetHandle(), frameIdx_);
	transform_->Pos() = player->GetPos();

	collider_ = objectManager.GetComponent<Collider>(ownerId_);
	collider_->SetHitFunc(std::bind(&PlayerAttackBehavior::OnHit, this, std::placeholders::_1, std::placeholders::_2));
	collider_->SetActiveFlag(false);
	attackTime_ = 2.0f;

	// コンボごとの攻撃アニメーション
	combo_ = objectManager.GetComponent<PlayerBehavior>(objectManager.GetPlayerID())->GetAtkCnt() + 1;
	if (combo_)
	{
		if (combo_ % 3 == 1)
		{
			AddEffect(FactoryID::VerticalEffect, objectManager, {0.0f,0.0f, 0.0f});
		
		}
		else if (combo_ % 3 == 2)
		{
			AddEffect(FactoryID::HorizontalEffect, objectManager, { 0.0f,0.0f, -100000000.0f });
		
		}
		else if (combo_ % 3 == 0)
		{
			AddEffect(FactoryID::ThrustEffect, objectManager, { 0.0f,35.0f, -100.0f });
		
		}
	}
	update_ = &PlayerAttackBehavior::UpdateNonAttack;
}

void PlayerAttackBehavior::End(ObjectManager& objectManager)
{
	attackStartTime_ = 0.0f;
}

void PlayerAttackBehavior::OnHit(Collider& col, ObjectManager& objectManager)
{
	auto atr = objectManager.GetComponent<ObjectInfo>(col.GetOwnerID())->GetAttribute();
	if (atr == ObjectAttribute::Enemy)
	{
		hitTime_ = 0.0f;
		isAttackInterval_ = true;
	}
}

void PlayerAttackBehavior::Destory(ObjectManager& objManager)
{
	objManager.GetFactory(FactoryID::PlayerAttack).Destroy(ownerId_);
}

void PlayerAttackBehavior::UpdateAttack(ObjectManager& objectManager, float delta)
{
	Effect(objectManager);
	attackTime_ -= delta;
	if (attackTime_ <= 0.0f)
	{
		objectManager.GetComponent<ObjectInfo>(ownerId_)->Destory();
	}
}

void PlayerAttackBehavior::UpdateNonAttack(ObjectManager& objectManager, float delta)
{
	attackStartTime_ -= delta;
	if (attackStartTime_ <= 0.0f)
	{
		update_ = &PlayerAttackBehavior::UpdateAttack;
		collider_->SetActiveFlag(true);
	}
}
