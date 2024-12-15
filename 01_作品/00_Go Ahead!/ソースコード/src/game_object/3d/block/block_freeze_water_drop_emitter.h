//--------------------------------------------------------------------------------
// 
// 水雫ブロックを生み出すが最初はフリーズしている [block_freeze_water_drop_emitter.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/game_object_3d.h"

//===================================================
// 水雫ブロックを生み出すが最初はフリーズしているクラス
//===================================================
class BlockFreezeWaterDropEmitter : public GameObject3D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockFreezeWaterDropEmitter();
	~BlockFreezeWaterDropEmitter() override = default;
	BlockFreezeWaterDropEmitter(BlockFreezeWaterDropEmitter&& right) noexcept;
	BlockFreezeWaterDropEmitter& operator=(BlockFreezeWaterDropEmitter&& right) noexcept;

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
	BlockFreezeWaterDropEmitter(const BlockFreezeWaterDropEmitter&) = delete;
	void operator=(const BlockFreezeWaterDropEmitter&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	float m_setInterval;		// 生み出す間隔
	float m_counterInterval;	// 間隔の記録
	Vec3 m_createPos;			// 生み出す位置
	float m_scale;				// スケール
	float m_msMinLife;			// 最短寿命
	float m_msMaxLife;			// 最長寿命
	float m_gravity;			// 重力
	bool m_isFreeze;			// 氷結中
	std::string m_pairKey;		// 炎や上昇気流などとのペアキー
};
