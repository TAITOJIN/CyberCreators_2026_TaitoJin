//--------------------------------------------------------------------------------
// 
// static と dynamic の状態変化をするブロック [block_change_state.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/block/block_base.h"

//===================================================
// 状態変化するブロックのクラス
//===================================================
class BlockChangeState : public BlockBase
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockChangeState();
	~BlockChangeState() override = default;
	BlockChangeState(BlockChangeState&& right) noexcept;
	BlockChangeState& operator=(BlockChangeState&& right) noexcept;

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
	BlockChangeState(const BlockChangeState&) = delete;
	void operator=(const BlockChangeState&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	// NOTHING
};
