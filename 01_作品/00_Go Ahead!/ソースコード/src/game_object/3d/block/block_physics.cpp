//--------------------------------------------------------------------------------
// 
// 物理挙動が特にあるブロック [block_physics.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_physics.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float RESTART_HEIGHT = 100.0f;			// リスタートになる高さ (MIN_WORLD_Y + この数値)
	constexpr float ADJUST_BUOYANCY_UNDERWATER = 0.1f;	// 水中の浮力の補正
	constexpr uint32_t NUM_BURNT_BLOCK = 10;		 	// 放出する焦げブロックの数

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockPhysics::BlockPhysics()
	: BlockBase()
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockPhysics::BlockPhysics(BlockPhysics&& right) noexcept
	: BlockBase(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockPhysics::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockBase::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockPhysics::uninit()
{
	// 親クラスの処理
	BlockBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockPhysics::update()
{
	// シーン情報の取得
	auto scene = getScene<SceneBase>();

	// 重力の設定
	btRigidBody* rigidBody = getRigitBodyPtr();
	//rigidBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
	//rigidBody->setFlags();
	rigidBody->activate(true);

	// 重力
	btVector3 gravity = scene->getWorld()->getDynamicsWorld()->getGravity();

	// 上昇気流との当たり判定
	auto pairs = scene->getFlameAndUpdraftPairs();
	for (auto& pair : pairs)
	{
		auto updraft = pair.second.second.lock();
		if (!updraft)
		{
			continue;
		}

		if (!updraft->getIsEnabled())
		{
			continue;
		}

		Collision::Rectangle rect;	// 矩形の当たり判定
		float posY = 0.0f;			// y 座標
		float height = 0.0f;		// 高さ
		{
			// 中心位置
			Vec3 pos = getPos();
			rect.center = Vec2(pos.x, pos.z);

			// y 座標
			posY = pos.y;

			// 半径
			auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
			Vec3 scale = getScale();
			Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
			size.x *= scale.x;
			size.y *= scale.y;
			size.z *= scale.z;
			rect.halfExtend = Vec2(size.x, size.z);

			// 向き
			Quaternion rot = getRot();
			Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
			rect.rot = euler.y;

			// 高さ (大きさ)
			height = size.y;
		}

		if (Collision::isCollisionBlockVsBlock(
			updraft->getRect(), rect,
			updraft->getPosY(), posY,
			updraft->getHeight(), height))
		{
			gravity *= -1.0f * ADJUST_BUOYANCY_UNDERWATER;
		}
	}

	// 重力の設定
	rigidBody->setGravity(gravity);

	// 親クラスの処理
	BlockBase::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockPhysics::draw() const
{
	// 親クラスの処理
	BlockBase::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockPhysics::inspector()
{
	// 親クラスの処理
	BlockBase::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockPhysics::save(json& outObject) const
{
	// 親クラスの処理
	BlockBase::save(outObject);
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockPhysics& BlockPhysics::operator=(BlockPhysics&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockBase::operator=(std::move(right));
	}

	return *this;
}
