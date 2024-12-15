//--------------------------------------------------------------------------------
// 
// 衝撃 [impact.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/impact.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/random_generator.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float MAX_SCALE_IMPACT = 200.0f;	// 最大爆発半径
	constexpr float SPEED_IMPACT = 300.0f;		// 半径の広がる速度

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Impact::Impact()
	: GameObject()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Impact::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject::init(inParam);
	if (FAILED(hr))
	{
		return hr;
	}

	// 初期パラメータの設定
	{
		// 中心位置
		m_sphere.pos = inParam[FLAG_POS];

		// 半径
		m_sphere.radius = 1.0f;
	}

	auto& impacts = getScene()->getImpacts();
	m_idx = impacts.size();
	impacts.push_back(shared_from_this()->downcast<Impact>());

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Impact::uninit()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Impact::update()
{
	m_sphere.radius += SPEED_IMPACT * getScene()->getDeltaTime();
	if (m_sphere.radius >= MAX_SCALE_IMPACT)
	{ // 大きくなった
		auto& impacts = getScene()->getImpacts();
		auto it = impacts.begin();
		for (uint32_t i = 0; i < m_idx; i++)
		{
			++it;
		}
		impacts.erase(it);
		setIsDestroy(true);
	}
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Impact::draw() const
{
	// DO_NOTHING
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void Impact::inspector()
{
	// 親クラスの処理
	GameObject::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void Impact::save(json& outObject) const
{
	// 親クラスの処理
	GameObject::save(outObject);
}
