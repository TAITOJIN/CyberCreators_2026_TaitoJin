//--------------------------------------------------------------------------------
// 
// 押せるブロック [block_push.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/block/block_base.h"
// utility
#include "utility/collision.h"

//===================================================
// 押せるブロックのクラス
//===================================================
class BlockPush : public BlockBase
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockPush();
	~BlockPush() override = default;
	BlockPush(BlockPush&& right) noexcept;
	BlockPush& operator=(BlockPush&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setIsTouchWater(const bool& inIsTouch) { m_isTouchWater = inIsTouch; }	// 水に触れているか設定
	void setIsUnderWater(const bool& inIsUnder) { m_isUnderWater = inIsUnder; }	// 水中にいるか設定

	const bool& getIsTouhWater() const { return m_isTouchWater; }	// 水に触れているかの取得
	const bool& getIsUnderWater() const { return m_isUnderWater; }	// 水中にいるかの取得
	const bool& getIsTouchFlame() const { return m_isTouchFlame; }	// 炎に触れたかどうかの取得
	const Collision::Rectangle& getRect() const { return m_rect; }	// 矩形の当たり判定の取得
	const float& getPosY() const { return m_posY; }					// y 座標の取得
	const float& getHeight() const { return m_height; }				// 高さの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockPush(const BlockPush&) = delete;
	void operator=(const BlockPush&) = delete;

	void reset();	// リセット

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_firstScale;				// 最初の大きさ
	Vec3 m_oldPos;					// 過去の位置
	Vec3 m_move;					// 移動量
	bool m_isTouchWater;			// 水に触れている
	bool m_isUnderWater;			// 水中にいる (頭までしっかり入っているかどうか)
	bool m_wasTouchWater;			// 水に触れていた
	bool m_isTouchFlame;			// 炎に触れた
	Collision::Sphere m_sphere;		// 球の当たり判定
	Collision::Rectangle m_rect;	// 矩形の当たり判定
	float m_posY;					// y 座標
	float m_height;					// 高さ
};
