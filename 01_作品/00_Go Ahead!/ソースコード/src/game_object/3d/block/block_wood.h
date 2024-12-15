//--------------------------------------------------------------------------------
// 
// 木ブロック [block_wood.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/block/block_physics.h"
// utility
#include "utility/collision.h"

//===================================================
// 木ブロックのクラス
//===================================================
class BlockWood : public BlockPhysics
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockWood();
	~BlockWood() override = default;
	BlockWood(BlockWood&& right) noexcept;
	BlockWood& operator=(BlockWood&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	const Collision::Rectangle& getRect() const { return m_rect; }	// 矩形の当たり判定の取得
	const float& getPosY() const { return m_posY; }					// y 座標の取得
	const float& getHeight() const { return m_height; }				// 高さの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockWood(const BlockWood&) = delete;
	void operator=(const BlockWood&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Collision::Sphere m_sphere;		// 球の当たり判定の情報
	Collision::Rectangle m_rect;	// 矩形の当たり判定
	float m_posY;					// y 座標
	float m_height;					// 高さ
};
