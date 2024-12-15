//--------------------------------------------------------------------------------
// 
// 上昇気流 [updraft.h]
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
// utility
#include "utility/collision.h"

//===================================================
// 上昇気流のクラス
//===================================================
class Updraft : public GameObject3D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Updraft();
	~Updraft() override = default;
	Updraft(Updraft&& right) noexcept;
	Updraft& operator=(Updraft&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	// setter
	void setPairKey(const std::string& inKey) { m_pairKey = inKey; }
	void setIsEnabled(const bool& inIsEnabled) { m_isEnabled = inIsEnabled; }

	// getter
	const Collision::Rectangle& getRect() const { return m_rect; }
	const float& getPosY() const { return m_posY; }
	const float& getHeight() const { return m_height; }
	const std::string& getPairKey() const { return m_pairKey; }
	const bool& getIsEnabled() const { return m_isEnabled; }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Updraft(const Updraft&) = delete;
	void operator=(const Updraft&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Collision::Rectangle m_rect;	// 矩形
	float m_posY;					// y 座標
	float m_height;					// 高さ
	std::string m_pairKey;			// 炎とのペアキー
	bool m_isEnabled;				// 上昇気流が有効か否か
};
