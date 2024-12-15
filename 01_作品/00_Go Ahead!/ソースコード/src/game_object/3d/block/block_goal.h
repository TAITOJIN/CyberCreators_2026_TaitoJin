//--------------------------------------------------------------------------------
// 
// ブロックのゴール [block_goal.h]
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
// ゴールブロックのクラス
//===================================================
class BlockGoal : public BlockDummy
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockGoal();
	~BlockGoal() override = default;
	BlockGoal(BlockGoal&& right) noexcept;
	BlockGoal& operator=(BlockGoal&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;				// インスペクター
	void save(json& outObject) const override;	// 保存

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockGoal(const BlockGoal&) = delete;
	void operator=(const BlockGoal&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	// NOTHING
};
