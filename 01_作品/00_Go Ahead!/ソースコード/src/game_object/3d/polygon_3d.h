//--------------------------------------------------------------------------------
// 
// 3D ポリゴン [polygon_3d.h]
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
// // 2d
#include "game_object/2d/texture_2d_animation.h"

//===================================================
// 3D ポリゴンのクラス
//===================================================
class Polygon3D : public GameObject3D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Polygon3D();
	~Polygon3D() override = default;
	Polygon3D(Polygon3D&& right) noexcept;
	Polygon3D& operator=(Polygon3D&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setPlain();	// 床の生成
	void setWall();		// 壁の生成

	void setSize(const Vec2& inSize) { m_size = inSize; }	// サイズの設定
	void setCol(const Color& inCol) { m_col = inCol; }		// 色の設定

	const Vec2& getSize() const { return m_size; }	// サイズの取得
	const Color& getColor() const { return m_col; }	// 色の取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Polygon3D(const Polygon3D&) = delete;
	void operator=(const Polygon3D&) = delete;

	bool createVtx();	// 頂点の生成

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	ComPtr<IDirect3DVertexBuffer9> m_vtxBuf;	// 頂点バッファへのポインタ
	Vec2 m_size;								// サイズ
	Color m_col;								// 色
	Vec2 m_center;								// 中心点
	//unique_ptr<Tex2dAnim> m_texAnim;			// テクスチャアニメーション情報
};
