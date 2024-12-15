//--------------------------------------------------------------------------------
// 
// 上昇気流のエフェクトを生み出す [updraft_effect_emitter.h]
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
// 上昇気流のエフェクトを生み出すクラス
//===================================================
class UpdraftEffectEmitter : public GameObject3D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	UpdraftEffectEmitter();
	~UpdraftEffectEmitter() override = default;
	UpdraftEffectEmitter(UpdraftEffectEmitter&& right) noexcept;
	UpdraftEffectEmitter& operator=(UpdraftEffectEmitter&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	// setter
	void setIsEnabled(const bool& inIsEnabled) { m_isEnabled = inIsEnabled; }
	void setPairKey(const std::string& inPairKey) { m_pairKey = inPairKey; }

	// getter
	const bool& getIsEnabled() const { return m_isEnabled; }
	const std::string& getPairKey() const { return m_pairKey; }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	UpdraftEffectEmitter(const UpdraftEffectEmitter&) = delete;
	void operator=(const UpdraftEffectEmitter&) = delete;

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
	bool m_isEnabled;			// 有効
	std::string m_pairKey;		// ペアのキー
};
