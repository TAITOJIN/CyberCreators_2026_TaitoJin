//--------------------------------------------------------------------------------
// 
// ブロックのゴール [block_goal.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_goal.h"
// // // character
#include "game_object/3d/character/player.h"
// game_manager
#include "game_manager/game_manager.h"
// scene
#include "scene/scene_game.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockGoal::BlockGoal()
	: BlockDummy()
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockGoal::BlockGoal(BlockGoal&& right) noexcept
	: BlockDummy(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockGoal::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockDummy::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockGoal::uninit()
{
	// 親クラスの処理
	BlockDummy::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockGoal::update()
{
	auto scene = getScene<SceneGame>();
	if (!scene)
	{
		// MEMO: デバッグ時などはゴール判定は行わないが、エディターなどで状態の更新を行うため親クラスの処理は呼ぶ
		BlockDummy::update();
		return;
	}

	// プレイヤーとの当たり判定を行い、ゴール判定をする

	// ゴールの当たり判定情報
	Collision::Rectangle goalRect;	// 矩形情報
	float goalY = 0.0f;				// 高さの原点
	float goalHeight = 0.0f;		// 高さ
	{
		// 中心位置
		Vec3 pos = getPos();
		goalRect.center = Vec2(pos.x, pos.z);

		// 半径
		auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
		Vec3 scale = getScale();
		Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
		size.x *= scale.x;
		size.y *= scale.y;
		size.z *= scale.z;
		goalRect.halfExtend = Vec2(size.x, size.z);

		// 回転値
		Quaternion rot = getRot();
		Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
		goalRect.rot = euler.y;

		// 高さ (位置)
		goalY = pos.y;

		// 高さ (大きさ)
		goalHeight = size.y;
	}

	// プレイヤーとの当たり判定
	auto player = scene->getPlayer();
	if (player)
	{
		Vec3 playerPos = player->getPos();

		if (Collision::isCollisionRectVsPoint(goalRect, Vec2(playerPos.x, playerPos.z))&&
			playerPos.y >= goalY - goalHeight &&
			playerPos.y <= goalY + goalHeight)
		{
			auto effect = scene->getGoalEffect();
			if (effect)
			{
				if (!effect->getIsEffecting())
				{
					effect->setIsEffecting(true);
				}
			}
			else
			{
				if (!scene->getIsGoal())
				{ // まだゴールしていない
					// ゴール状態に設定
					scene->setIsGoal(true);
				}
			}
		}
	}

	// 親クラスの処理
	BlockDummy::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockGoal::draw() const
{
	// 親クラスの処理
	BlockDummy::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockGoal::inspector()
{
	// 親クラスの処理
	BlockDummy::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockGoal::save(json& outObject) const
{
	if (!getDummyFlags().isFlagSet(DUMMY_FLAGS::SAVE))
	{
		return;
	}

	// 親クラスの処理
	BlockDummy::save(outObject);
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockGoal& BlockGoal::operator=(BlockGoal&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockDummy::operator=(std::move(right));
	}

	return *this;
}
