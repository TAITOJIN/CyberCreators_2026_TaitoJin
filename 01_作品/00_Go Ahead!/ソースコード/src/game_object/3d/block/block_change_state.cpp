//--------------------------------------------------------------------------------
// 
// static と dynamic の状態変化をするブロック [block_change_state.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_change_state.h"
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
BlockChangeState::BlockChangeState()
	: BlockBase()
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockChangeState::BlockChangeState(BlockChangeState&& right) noexcept
	: BlockBase(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockChangeState::init(const json& inParam)
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
void BlockChangeState::uninit()
{
	// 親クラスの処理
	BlockBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockChangeState::update()
{
	if (getIsStatic())
	{
		// シーン情報の取得
		auto scene = getScene<SceneBase>();

		auto impacts = scene->getImpacts();
		for (const auto& element : impacts)
		{
			auto impact = element.lock();
			if (!impact)
			{
				continue;
			}

			// 剛体の取得
			btRigidBody* rigidBody = getRigitBodyPtr();
			rigidBody->activate(true);

			Collision::Sphere sphere;
			{
				// 中心位置
				sphere.pos = getPos();

				// 半径
				auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
				Vec3 scale = getScale();
				Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
				size.x *= scale.x;
				size.y *= scale.y;
				size.z *= scale.z;
				sphere.radius = (size.x + size.y + size.z) * 0.333f;
			}

			if (Collision::isCollisionSphereVsSphere(impact->getSphere(), sphere))
			{
#if USE_NEW_SHARED_DATA
				auto& gm = GM;
				gm.lockCS();
				auto data = scene->getSharedData();
				if (!data->m_isBreakSnowman)
				{
					if (getTag() == "5_snowman_0")
					{
						data->m_isBreakSnowman = true;
					}
				}
				gm.unlockCS();
#endif

				setIsStatic(false);
				createBlockRigidBody();

				btRigidBody* rigidbody = getRigitBodyPtr();
				if (rigidbody)
				{
					Vec3 diff = getPos() - impact->getSphere().pos;
					D3DXVec3Normalize(&diff, &diff);
					diff *= IMPACT_STRENGTH;
					rigidbody->applyImpulse(btVector3(diff.x, diff.y, diff.z), btVector3(0.0f, -1.0f, 0.0f));
				}

				break;
			}
		}
	}

	// 親クラスの処理
	BlockBase::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockChangeState::draw() const
{
	// 親クラスの処理
	BlockBase::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockChangeState::inspector()
{
	// 親クラスの処理
	BlockBase::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockChangeState::save(json& outObject) const
{
	// 親クラスの処理
	BlockBase::save(outObject);
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockChangeState& BlockChangeState::operator=(BlockChangeState&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockBase::operator=(std::move(right));
	}

	return *this;
}
