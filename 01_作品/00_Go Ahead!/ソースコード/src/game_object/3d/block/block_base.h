//--------------------------------------------------------------------------------
// 
// ブロックの基底クラス [block_base.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/model.h"

//===================================================
// ブロックの基底クラス
//===================================================
class BlockBase : public Model
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockBase();
	~BlockBase() override = default;
	BlockBase(BlockBase&& right) noexcept;
	BlockBase& operator=(BlockBase&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;		// 保存

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockBase(const BlockBase&) = delete;
	void operator=(const BlockBase&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	// NOTHING
};
