//--------------------------------------------------------------------------------
// 
// コイン [coin.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/model.h"
#include "game_object/3d/block/block_shadow.h"

//===================================================
// コインのクラス
//===================================================
class Coin : public Model
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Coin();
	~Coin() override = default;
	Coin(Coin&& right) noexcept;
	Coin& operator=(Coin&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	static void updateRot();	// コインの回転の更新処理

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Coin(const Coin&) = delete;
	void operator=(const Coin&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	static Quaternion m_q;	// みんな同じ回転角
	std::list<weak_ptr<BlockShadow>> m_shadowBlocks;	// 影ブロック
	float m_walkEffectIntervalCounter;	// 歩きエフェクトの間隔
	bool m_isHidden;	// 隠しコイン
};
