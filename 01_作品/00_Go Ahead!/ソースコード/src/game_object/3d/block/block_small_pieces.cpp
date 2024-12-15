//--------------------------------------------------------------------------------
// 
// 粉々なブロック [block_small_pieces.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_small_pieces.h"
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
BlockSmallPieces::BlockSmallPieces()
	: BlockDummy()
	, m_move(DEF_VEC3)
	, m_maxLife(0.0f)
	, m_life(0.0f)
	, m_firstScale(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockSmallPieces::BlockSmallPieces(BlockSmallPieces&& right) noexcept
	: BlockDummy(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockSmallPieces::init(const json& inParam)
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
		m_move = inParam[PARAM_BLOCK_SMALL_PIECES];

		// スケールの反映
		setScale(Vec3(m_firstScale, m_firstScale, m_firstScale));
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockSmallPieces::uninit()
{
	// 親クラスの処理
	BlockDummy::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockSmallPieces::update()
{
	// デルタタイムの取得
	float deltaTime = getScene<SceneBase>()->getDeltaTime();

	// 位置の更新
	{
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
void BlockSmallPieces::draw() const
{
	// 親クラスの処理
	BlockDummy::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockSmallPieces::inspector()
{
	// 親クラスの処理
	BlockDummy::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockSmallPieces::save(json& outObject) const
{
	if (!getDummyFlags().isFlagSet(DUMMY_FLAGS::SAVE))
	{
		return;
	}

	// 親クラスの処理
	BlockDummy::save(outObject);

	outObject[WATERDROP_PARAM_FIRST_SCALE] = m_firstScale;
	outObject[WATERDROP_PARAM_MAX_LIFE] = m_maxLife;
	outObject[PARAM_BLOCK_SMALL_PIECES] = m_move;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockSmallPieces& BlockSmallPieces::operator=(BlockSmallPieces&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockDummy::operator=(std::move(right));
	}

	return *this;
}
