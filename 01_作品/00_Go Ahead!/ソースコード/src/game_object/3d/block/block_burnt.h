//--------------------------------------------------------------------------------
// 
// 焦げたブロック [block_burnt.h]
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
// 焦げたブロックのクラス
//===================================================
class BlockBurnt : public BlockDummy
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockBurnt();
	~BlockBurnt() override = default;
	BlockBurnt(BlockBurnt&& right) noexcept;
	BlockBurnt& operator=(BlockBurnt&& right) noexcept;

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
	BlockBurnt(const BlockBurnt&) = delete;
	void operator=(const BlockBurnt&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	float m_timeCounter;	// 経過カウンター
	float m_startSmallTime;	// 縮小開始の時間
};
