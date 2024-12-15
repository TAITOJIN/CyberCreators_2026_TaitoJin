//--------------------------------------------------------------------------------
// 
// 氷結した水雫ブロック [block_freeze_water_drop.h]
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
// 氷結した水雫ブロックのクラス
//===================================================
class BlockFreezeWaterDrop : public BlockDummy
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockFreezeWaterDrop();
	~BlockFreezeWaterDrop() override = default;
	BlockFreezeWaterDrop(BlockFreezeWaterDrop&& right) noexcept;
	BlockFreezeWaterDrop& operator=(BlockFreezeWaterDrop&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	// setter
	void setIsFreeze(const bool& inIsFreeze) { m_isFreeze = inIsFreeze; }
	void setPairKey(const std::string& inKey) { m_pairKey = inKey; }

	// getter
	const bool& getIsFreeze() const { return m_isFreeze; }
	const std::string& getPairKey() const { return m_pairKey; }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockFreezeWaterDrop(const BlockFreezeWaterDrop&) = delete;
	void operator=(const BlockFreezeWaterDrop&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_move;			// 移動量
	float m_maxLife;		// 最大寿命
	float m_life;			// 寿命
	float m_firstScale;		// 最初のスケール (x, y, z で統一)
	float m_gravity;		// 重力
	bool m_isFreeze;		// 氷結中
	bool m_keep;			// 現在の状態を保持 (save 用)
	std::string m_pairKey;	// 炎や上昇気流などとのペアキー
};
