//--------------------------------------------------------------------------------
// 
// 移動するブロック [block_rotate.h]
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
#include "game_object/3d/block/block_base.h"

//===================================================
// 移動するブロックのクラス
//===================================================
class BlockMove : public BlockBase
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockMove();
	~BlockMove() override = default;
	BlockMove(BlockMove&& right) noexcept;
	BlockMove& operator=(BlockMove&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;				// インスペクター
	void save(json& outObject) const override;	// 保存

	void setIsEnabled(const bool& inIsEnabled) { m_isEnabled = inIsEnabled; }	// 有効か否か

	const Vec3& getPoint1() const { return m_point1; }				// 移動する線分の点その 1 の取得
	const Vec3& getPoint2() const { return m_point2; }				// 移動する線分の点その 2 の取得
	const Vec3& getTargetPoint() const { return m_targetPoint; }	// 目的の位置
	const Vec3& getMove() const { return m_move; }					// 移動量の取得
	const float& getAtnSpeed() const { return m_atnSpeed; }			// 速度の補正値を取得
	const bool& getIsEnabled() const { return m_isEnabled; }		// 有効か否か
	const std::string& getPairKey() const { return m_pairKey; }		// ペアのキー

private:
	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	BlockMove(const BlockMove&) = delete;
	void operator=(const BlockMove&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_point1;			// 移動する線分の点その 1
	Vec3 m_point2;			// 移動する線分の点その 2
	Vec3 m_targetPoint;		// 目的の位置
	Vec3 m_move;			// 移動量
	float m_atnSpeed;		// たどり着くまでの速度の補正
	bool m_isEnabled;		// 回転が有効か否か
	std::string m_pairKey;	// ペアのキー
};
