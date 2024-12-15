//--------------------------------------------------------------------------------
// 
// 雪 [snow_2d.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/2d/game_object_2d.h"

//===================================================
// 雪のクラス
//===================================================
class Snow2D : public GameObject2D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Snow2D();
	~Snow2D() override = default;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setMove(const Vec3& inMove) { m_move = inMove; }
	const Vec3& getMove() const { return m_move; }

private:
	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_move;	// 移動量
};
