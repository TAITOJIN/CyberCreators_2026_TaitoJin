//--------------------------------------------------------------------------------
// 
// スポーン地点 [spawn_point.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/polygon_3d.h"

//===================================================
// スポーン地点のクラス
//===================================================
class SpawnPoint : public Polygon3D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	SpawnPoint();
	~SpawnPoint() override = default;
	SpawnPoint(SpawnPoint&& right) noexcept;
	SpawnPoint& operator=(SpawnPoint&& right) noexcept;

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
	SpawnPoint(const SpawnPoint&) = delete;
	void operator=(const SpawnPoint&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_respawnPoint;	// リスポーン地点
};
