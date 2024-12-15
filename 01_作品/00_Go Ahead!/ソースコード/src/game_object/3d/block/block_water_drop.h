//--------------------------------------------------------------------------------
// 
// 水雫ブロック [block_water_drop.h]
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
// 水雫ブロックのクラス
//===================================================
class BlockWaterDrop : public BlockDummy
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockWaterDrop();
	~BlockWaterDrop() override = default;
	BlockWaterDrop(BlockWaterDrop&& right) noexcept;
	BlockWaterDrop& operator=(BlockWaterDrop&& right) noexcept;

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
	BlockWaterDrop(const BlockWaterDrop&) = delete;
	void operator=(const BlockWaterDrop&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_move;		// 移動量
	float m_maxLife;	// 最大寿命
	float m_life;		// 寿命
	float m_firstScale;	// 最初のスケール (x, y, z で統一)
	float m_gravity;	// 重力
};
