//--------------------------------------------------------------------------------
// 
// 煙ブロック [block_smoke.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_dummy.h"

//===================================================
// 煙ブロックのクラス
//===================================================
class BlockSmoke : public BlockDummy
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockSmoke();
	~BlockSmoke() override = default;
	BlockSmoke(BlockSmoke&& right) noexcept;
	BlockSmoke& operator=(BlockSmoke&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockSmoke(const BlockSmoke&) = delete;
	void operator=(const BlockSmoke&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_move;		// 移動量
	float m_maxLife;	// 最大寿命
	float m_life;		// 寿命
	float m_firstScale;	// 最初のスケール (x, y, z で統一)
	float m_gravity;	// 重力
};
