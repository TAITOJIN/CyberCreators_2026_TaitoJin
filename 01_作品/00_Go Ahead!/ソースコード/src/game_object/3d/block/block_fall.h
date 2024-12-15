//--------------------------------------------------------------------------------
// 
// 落ちるブロック [block_fall.h]
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
#include "game_object/3d/block/block_base.h"

//===================================================
// 落ちるブロックのクラス
//===================================================
class BlockFall : public BlockBase
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockFall();
	BlockFall(BlockFall&& right) noexcept;
	~BlockFall() override = default;
	BlockFall& operator=(BlockFall&& right) noexcept;

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
	BlockFall(const BlockFall&) = delete;
	void operator=(const BlockFall&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	// NOTHING
};
