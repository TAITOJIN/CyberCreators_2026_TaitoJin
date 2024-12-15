//--------------------------------------------------------------------------------
// 
// 水雫ブロック [block_water_drop.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_water_drop.h"
// game_manager
#include "game_manager/game_manager.h"
// scene
#include "scene/scene_game.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//===================================================
	// 定数
	//===================================================
	constexpr float GRAVITY_STRENGTH = -10.0f;	// 重力の強さ

} // namespace /* anonymous */
//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockWaterDrop::BlockWaterDrop()
	: BlockDummy()
	, m_move(DEF_VEC3)
	, m_maxLife(0.0f)
	, m_life(0.0f)
	, m_firstScale(0.0f)
	, m_gravity(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockWaterDrop::BlockWaterDrop(BlockWaterDrop&& right) noexcept
	: BlockDummy(std::move(right))
	, m_move(std::move(right.m_move))
	, m_maxLife(std::move(right.m_maxLife))
	, m_life(std::move(right.m_life))
	, m_firstScale(std::move(right.m_firstScale))
	, m_gravity(std::move(right.m_gravity))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockWaterDrop::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockDummy::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_firstScale = inParam[WATERDROP_PARAM_FIRST_SCALE];
		m_maxLife = inParam[WATERDROP_PARAM_MAX_LIFE];
		m_life = m_maxLife;
		m_gravity = inParam[WATERDROP_PARAM_GRAVITY];

		// スケールの反映
		setScale(Vec3(m_firstScale, m_firstScale, m_firstScale));
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockWaterDrop::uninit()
{
	// 親クラスの処理
	BlockDummy::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockWaterDrop::update()
{
	// デルタタイムの取得
	float deltaTime = getScene<SceneBase>()->getDeltaTime();

	// 落下していく
	{
		m_move.y += m_gravity * deltaTime;
		setPos(getPos() + m_move);
		Vec3 pos = getPos();
		getRigitBodyPtr()->getWorldTransform().setOrigin(btVector3(pos.x, pos.y, pos.z));
	}

	// 寿命を減らしていく
	m_life -= deltaTime;
	if (m_life <= 0.0f)
	{ // 寿命がなくなった
		m_life = 0.0f;		// 保持
		setIsDestroy(true);	// 破棄
	}

	// スケールの算出と反映
	{
		float scale = (m_firstScale * m_life) / m_maxLife;	// 現在のスケールの計算
		setScale(Vec3(scale, scale, scale));				// スケールの反映
	}

	// 親クラスの処理
	BlockDummy::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockWaterDrop::draw() const
{
	// 親クラスの処理
	BlockDummy::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockWaterDrop::inspector()
{
	// 親クラスの処理
	BlockDummy::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockWaterDrop::save(json& outObject) const
{
	if (!getDummyFlags().isFlagSet(DUMMY_FLAGS::SAVE))
	{
		return;
	}

	// 親クラスの処理
	BlockDummy::save(outObject);

	outObject[WATERDROP_PARAM_FIRST_SCALE] = m_firstScale;
	outObject[WATERDROP_PARAM_MAX_LIFE] = m_maxLife;
	outObject[WATERDROP_PARAM_GRAVITY] = m_gravity;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockWaterDrop& BlockWaterDrop::operator=(BlockWaterDrop&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockDummy::operator=(std::move(right));

		m_move = std::move(right.m_move);
		m_maxLife = std::move(right.m_maxLife);
		m_life = std::move(right.m_life);
		m_firstScale = std::move(right.m_firstScale);
		m_gravity = std::move(right.m_gravity);
	}

	return *this;
}
