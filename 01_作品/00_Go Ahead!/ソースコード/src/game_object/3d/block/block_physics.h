//--------------------------------------------------------------------------------
// 
// 物理挙動が特にあるブロック [block_physics.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/block/block_blocks.h"

//===================================================
// 物理挙動が特にあるブロックのクラス
//===================================================
class BlockPhysics : public BlockBase
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockPhysics();
	~BlockPhysics() override = default;
	BlockPhysics(BlockPhysics&& right) noexcept;
	BlockPhysics& operator=(BlockPhysics&& right) noexcept;

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
	BlockPhysics(const BlockPhysics&) = delete;
	void operator=(const BlockPhysics&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	// NOTHING
};
