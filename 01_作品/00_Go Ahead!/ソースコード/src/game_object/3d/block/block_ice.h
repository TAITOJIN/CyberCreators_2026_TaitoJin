//--------------------------------------------------------------------------------
// 
// 氷ブロック [block_ice.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/block/block_dummy.h"

//===================================================
// 氷ブロックのクラス
//===================================================
class BlockIce : public BlockDummy
{
public:
	//---------------------------------------------------
	// 状態
	//---------------------------------------------------
	enum class ICE_STATE : uint32_t
	{
		ICE = 0U,
		ICE_TO_WATER,
		WATER
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockIce();
	~BlockIce() override = default;
	BlockIce(BlockIce&& right) noexcept;
	BlockIce& operator=(BlockIce&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	// setter
	void setIceState(const ICE_STATE& inIceState) { m_iceState = inIceState; }
	void setIceMatKey(const std::string& inKey) { m_iceMatKey = inKey; }
	void setWaterMatKey(const std::string& inKey) { m_waterMatKey = inKey; }
	void setPairKey(const std::string& inKey) { m_pairKey = inKey; }

	// getter
	const ICE_STATE& getIceState() const { return m_iceState; }
	const std::string& getPairKey() const { return m_pairKey; }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockIce(const BlockIce&) = delete;
	void operator=(const BlockIce&) = delete;

	void splashOfWater();	// 水しぶき

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::string m_iceMatKey;	// 氷ブロックのマテリアルのキー
	std::string m_waterMatKey;	// 水ブロックのマテリアルのキー
	GraphicsMaterial m_mat;		// マテリアル
	ICE_STATE m_iceState;		// アイス状態
	std::string m_pairKey;		// 炎や上昇気流などとのペアキー
};
