//--------------------------------------------------------------------------------
// 
// 焦げたブロック [block_burnt.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_burnt.h"
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
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float SUB_SCALE = 2.0f;	// 縮小値

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockBurnt::BlockBurnt()
	: BlockDummy()
	, m_timeCounter(0.0f)
	, m_startSmallTime(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockBurnt::BlockBurnt(BlockBurnt&& right) noexcept
	: BlockDummy(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockBurnt::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockDummy::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_startSmallTime = inParam[BURNT_BLOCK_PARAM_START_SMALL_SEC];
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockBurnt::uninit()
{
	// 親クラスの処理
	BlockDummy::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockBurnt::update()
{
	auto scene = getScene<SceneBase>();
	float deltaTime = scene->getDeltaTime();

	m_timeCounter += deltaTime;
	if (m_timeCounter >= m_startSmallTime)
	{
		Vec3 scale = getScale();
		if (scale.z >= 0.0f)
		{
			scale.z -= SUB_SCALE * deltaTime;
			scale.x = scale.y = scale.z;
			setScale(scale);
			//createBlockRigidBody();
			if (scale.z <= 0.0f)
			{
				setIsDestroy(true);
			}
		}
	}

	// 親クラスの処理
	BlockDummy::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockBurnt::draw() const
{
	// 親クラスの処理
	BlockDummy::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockBurnt::inspector()
{
	// 親クラスの処理
	BlockDummy::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockBurnt::save(json& outObject) const
{
	if (!getDummyFlags().isFlagSet(DUMMY_FLAGS::SAVE))
	{
		return;
	}

	// 親クラスの処理
	BlockDummy::save(outObject);

	outObject[BURNT_BLOCK_PARAM_START_SMALL_SEC] = m_startSmallTime;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockBurnt& BlockBurnt::operator=(BlockBurnt&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockDummy::operator=(std::move(right));
	}

	return *this;
}
