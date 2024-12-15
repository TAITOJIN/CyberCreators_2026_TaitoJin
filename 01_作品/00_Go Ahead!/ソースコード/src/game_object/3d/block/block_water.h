//--------------------------------------------------------------------------------
// 
// 水ブロック [block_water.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/block/block_dummy.h"

//===================================================
// 水ブロックのクラス
//===================================================
class BlockWater : public BlockDummy
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockWater();
	~BlockWater() override = default;
	BlockWater(BlockWater&& right) noexcept;
	BlockWater& operator=(BlockWater&& right) noexcept;

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
	BlockWater(const BlockWater&) = delete;
	void operator=(const BlockWater&) = delete;

	void splashOfWater();	// 水しぶき

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	bool m_wasCollision;	// 前フレームに衝突していた
	bool m_isSetCoolTime;	// クールタイムを設ける (浮かび上がったとき、少し沈んでからまた浮かび上がりたいので)
	float m_coolTime;		// クールタイム
};
