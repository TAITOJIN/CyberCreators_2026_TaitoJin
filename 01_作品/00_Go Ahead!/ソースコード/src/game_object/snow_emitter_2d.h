//--------------------------------------------------------------------------------
// 
// 雪を生み出す [snow_emitter_2d.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/game_object.h"

//===================================================
// 雪を生み出すのクラス
//===================================================
class SnowEmitter2D : public GameObject
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	SnowEmitter2D();
	~SnowEmitter2D() override = default;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setIsEnabled(const bool& inIs) { m_isEnabled = inIs; }
	const bool& getIsEnabled() const { return m_isEnabled; }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	SnowEmitter2D(const SnowEmitter2D&) = delete;
	void operator=(const SnowEmitter2D&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	bool m_isEnabled;		// 有効か否か
	float m_timeCounter;	// 時間のカウント
	float m_interval;		// インターバル
	float m_intervalMin;	// 間隔の最小値
	float m_intervalMax;	// 間隔の最大数
	int m_numMin;			// 最小数
	int m_numMax;			// 最大数
};
