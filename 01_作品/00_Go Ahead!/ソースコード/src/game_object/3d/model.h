//--------------------------------------------------------------------------------
// 
// モデル [model.h]
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

//===================================================
// モデルのクラス
//===================================================
class Model : public GameObject3D
{
public:
	//---------------------------------------------------
	// public メンバ構造体
	//---------------------------------------------------
	struct ModelMaterial
	{
		GraphicsMaterial mat;
		std::string key;
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Model();
	~Model() override = default;
	Model(Model&& right) noexcept;
	Model& operator=(Model&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void drawWithShadow();	// 描画

	void inspector() override;				// インスペクター
	void save(json& outObject) const override;	// 保存

	void setMat(const std::unordered_map<int, ModelMaterial>& inMat) { m_mat = inMat; }	// マテリアルの設定
	auto& getMat() const { return m_mat; }	// マテリアルの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Model(const Model&) = delete;
	void operator=(const Model&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	mutable std::unordered_map<int, ModelMaterial> m_mat;	// マテリアル
};
