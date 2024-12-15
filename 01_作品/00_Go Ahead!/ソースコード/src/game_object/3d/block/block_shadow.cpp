//--------------------------------------------------------------------------------
// 
// 影ブロック [block_shadow.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_shadow.h"
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
// マクロ定義
//---------------------------------------------------
#define USE_RIGIDBODY	(0)

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float RESTART_HEIGHT = 10.0f;	// リスタートになる高さ (MIN_WORLD_Y + この数値)
	constexpr float RAY_END = 1000.0f;			// レイの終点

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockShadow::BlockShadow()
	: BlockDummy()
	, m_firstScale(DEF_VEC3)
	, m_scaleCounter(0.0f)
	, m_doAdd(true)
	, m_posParent(DEF_VEC3)
	, m_isAnim(true)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockShadow::BlockShadow(BlockShadow&& right) noexcept
	: BlockDummy(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockShadow::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockDummy::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_firstScale = inParam[FLAG_SCALE];
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockShadow::uninit()
{
	// 親クラスの処理
	BlockDummy::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockShadow::update()
{
	float deltaTime = getScene()->getDeltaTime();

	if (m_isAnim)
	{
		if (m_doAdd)
		{
			m_scaleCounter += deltaTime;
			if (m_scaleCounter >= 1.0f)
			{ // 1 秒経過
				m_scaleCounter = 1.0f;
				m_doAdd = false;
			}
		}
		else
		{
			m_scaleCounter -= deltaTime;
			if (m_scaleCounter <= 0.0f)
			{ // 1 秒経過
				m_scaleCounter = 0.0f;
				m_doAdd = true;
			}
		}

		Vec3 scale = getScale();
		scale.x = 0.1f + m_firstScale.x * m_scaleCounter;
		scale.z = 0.1f + m_firstScale.z * m_scaleCounter;
		setScale(scale);
	}

	Vec3 pos = m_posParent - Vec3(0.0f, RAY_END, 0.0f);
	Quaternion rot = DEF_ROT;

	// プレイヤーと衝突しないように補正する
	Vec3 posParent = m_posParent;
	posParent += Vec3(1.0f, -0.1f, 1.0f);
	posParent += getOffset();

	btVector3 from = btVector3(posParent.x, posParent.y, posParent.z);
	btVector3 to = from - btVector3(0, RAY_END, 0);
	
	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	getScene<SceneBase>()->getWorld()->getDynamicsWorld().get()->rayTest(from, to, rayCallback);
	
	// 衝突した点と法線を取得
	if (rayCallback.hasHit())
	{
		btVector3 hitPoint = rayCallback.m_hitPointWorld;
		btQuaternion hitRot = rayCallback.m_collisionObject->getWorldTransform().getRotation();

		pos = Vec3(hitPoint.getX(), hitPoint.getY(), hitPoint.getZ());
		rot = Quaternion(hitRot.getX(), hitRot.getY(), hitRot.getZ(), hitRot.getW());

		pos -= Vec3(1.0f, -0.1f, 1.0f);
		pos -= getOffset();
	}

	pos.y += 0.1f;
	setPos(pos);
	setRot(rot);
	
	// 影は破棄しない
	if (getPos().y <= MIN_WORLD_Y + RESTART_HEIGHT)
	{ // 最低高度に到達した
		// 位置
		Vec3 keepPos = getPos();
		keepPos.y = MIN_WORLD_Y + RESTART_HEIGHT;
		setPos(keepPos);
	}

	// マトリックスの更新
	GameObject3D::updateMtx();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockShadow::draw() const
{
	// 親クラスの処理
	BlockDummy::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockShadow::inspector()
{
	// 親クラスの処理
	BlockDummy::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockShadow::save(json& outObject) const
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
BlockShadow& BlockShadow::operator=(BlockShadow&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockDummy::operator=(std::move(right));
	}

	return *this;
}
