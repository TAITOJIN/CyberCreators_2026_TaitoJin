//--------------------------------------------------------------------------------
// 
// ライト [light.h]
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
// ライトのクラス
//===================================================
class Light3D : public GameObject
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Light3D();
	~Light3D() override = default;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setLightMatrix();	// ライト空間への投影行列の設定処理

	void setLight(const Light& inLight) { m_light = inLight; }	// ライトの設定
	void setIndx(const uint32_t& inIdx) { m_idx = inIdx; }		// インデックスの設定
	void setMatrix(const Matrix& inMtx) { m_mtxWorld = inMtx; }	// マトリックスの設定

	const Light& getLight() const { return m_light; }		// ライトの取得
	const uint32_t& getIdx() const { return m_idx; }		// インデックスの取得
	const Matrix& getMatrix() const { return m_mtxWorld; }	// マトリックスの取得
	const Vec3& getVLight() const { return m_vLight; }
	const Vec3& getVLook() const { return m_vLook; }
	Vec3 getDir() const { return m_vLight - m_vLook; }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Light3D(const Light3D&) = delete;
	void operator=(const Light3D&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Light m_light;		// ライト
	uint32_t m_idx;		// インデックス
	Matrix m_mtxWorld;	// マトリックス

	Vec3 m_vLight;
	Vec3 m_vLook;
};
