//--------------------------------------------------------------------------------
// 
// 炎 [flame.h]
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
// 炎のクラス
//===================================================
class Flame : public GameObject3D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Flame();
	~Flame() override = default;
	Flame(Flame&& right) noexcept;
	Flame& operator=(Flame&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setPairKey(const std::string& inKey) { m_pairKey = inKey; }	// ペアキーの設定

	const Collision::Sphere& getSphere() const { return m_sphere; }	// 球の当たり判定の取得
	const std::string& getPairKey() const { return m_pairKey; }		// ペアキーの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Flame(const Flame&) = delete;
	void operator=(const Flame&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Collision::Sphere m_sphere;	// 球の当たり判定の情報
	std::string m_pairKey;		// 上昇気流とのペアキー
};
