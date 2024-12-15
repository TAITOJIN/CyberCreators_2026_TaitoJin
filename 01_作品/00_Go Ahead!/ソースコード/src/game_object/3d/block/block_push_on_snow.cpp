//--------------------------------------------------------------------------------
// 
// 雪の上にある押せるブロック [block_push_on_snow.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_push_on_snow.h"
// game_manager
#include "game_manager/game_manager.h"
// scene
#include "scene/scene_debug.h"
// utility
#include "utility/calculate.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float ADD_SCALE = 1.0f;

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockPushOnSnow::BlockPushOnSnow()
	: BlockPush()
	, m_posOld(DEF_VEC3)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockPushOnSnow::BlockPushOnSnow(BlockPushOnSnow&& right) noexcept
	: BlockPush(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockPushOnSnow::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockPush::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_posOld = getFirstPos();
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockPushOnSnow::uninit()
{
	// 親クラスの処理
	BlockPush::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockPushOnSnow::update()
{
	// 過去の位置の更新
	m_posOld = getPos();

	// 親クラスの処理
	BlockPush::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockPushOnSnow::draw() const
{
	// 親クラスの処理
	BlockPush::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockPushOnSnow::inspector()
{
	// 親クラスの処理
	BlockPush::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockPushOnSnow::save(json& outObject) const
{
	// 親クラスの処理
	BlockPush::save(outObject);
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockPushOnSnow& BlockPushOnSnow::operator=(BlockPushOnSnow&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockPush::operator=(std::move(right));
	}

	return *this;
}
