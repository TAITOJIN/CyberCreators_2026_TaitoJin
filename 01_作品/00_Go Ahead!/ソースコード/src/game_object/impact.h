//--------------------------------------------------------------------------------
// 
// 衝撃 [impact.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/game_object.h"
// utility
#include "utility/collision.h"

//===================================================
// 衝撃のクラス
//===================================================
class Impact : public GameObject
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Impact();
	~Impact() override = default;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	const Collision::Sphere& getSphere() const { return m_sphere; }	// 球の当たり判定の取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Impact(const Impact&) = delete;
	void operator=(const Impact&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Collision::Sphere m_sphere;	// 球の当たり判定の情報
	uint32_t m_idx;				// インデックス
};
