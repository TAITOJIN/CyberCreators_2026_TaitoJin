//--------------------------------------------------------------------------------
// 
// 水車 [water_wheel.h]
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
#include "game_object/3d/model.h"

//===================================================
// 水車のクラス
//===================================================
class WaterWheel : public Model
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	WaterWheel();
	~WaterWheel() override = default;
	WaterWheel(WaterWheel&& right) noexcept;
	WaterWheel& operator=(WaterWheel&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setIsEnabled(const bool& inIsEnabled) { m_isEnabled = inIsEnabled; }	// 有効か否か

	const Vec3& getAxis() const { return m_axis; }				// 回転する軸の取得
	const float& getSpeedRot() const { return m_speedRot; }		// 回転する速度の取得
	const bool& getIsEnabled() const { return m_isEnabled; }	// 有効か否か
	const std::string& getPairKey() const { return m_pairKey; }	// ペアのキー

private:
	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	WaterWheel(const WaterWheel&) = delete;
	void operator=(const WaterWheel&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_axis;			// 回転する軸
	float m_speedRot;		// 回転する速度
	bool m_isEnabled;		// 回転が有効か否か
	bool m_firstIsEnabled;	// 最初回転が有効か否か (save 用)
	std::string m_pairKey;	// ペアのキー
	float m_timeEnd;		// 回転終了する時間
	float m_timeCounter;	// 時間のカウント
	bool m_isEndless;		// ずっと回る
};
