//--------------------------------------------------------------------------------
// 
// 2D オブジェクト [game_object_2d.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/game_object.h"
// // 2d
#include "game_object/2d/texture_2d_animation.h"

//===================================================
// オブジェクト 2D のクラス
//===================================================
class GameObject2D : public GameObject
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	GameObject2D();
	~GameObject2D() override = default;
	GameObject2D(GameObject2D&& right) noexcept;
	GameObject2D& operator=(GameObject2D&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setPos(const Vec3& inPos) { m_pos = inPos; }							// 位置の設定
	void setRot(const Vec3& inRot) { m_rot = inRot; }							// 向きの設定
	void setSize(const Vec3& inSize) { m_size = inSize; }						// サイズの設定
	void setCol(const Color& inCol) { m_col = inCol; }							// 色の設定
	void setTexKey(const std::string& inTexKey) { m_texKey = inTexKey; }		// テクスチャのキーの設定
	void setIsTexAnim(const bool& inIsTexAnim) { m_isTexAnim = inIsTexAnim; }	// テクスチャアニメーションをするか設定

	const VertexBuffer& getVtxBuf() const { return m_vtxBuf.Get(); }	// 頂点バッファへのポインタの取得
	const Vec3& getPos() const { return m_pos; }						// 位置の取得
	const Vec3& getRot() const { return m_rot; }						// 向きの取得
	const Vec3& getSize() const { return m_size; }						// サイズの取得
	const Color& getCol() const { return m_col; }						// 色の取得
	const std::string& getTexKey() const { return m_texKey; }			// テクスチャのキーの取得
	const bool& getIsTexAnim() const { return m_isTexAnim; }			// テクスチャアニメーションをするか取得

	void updateTexAnim();																				// テクスチャアニメーション
	void setTexCoord(const Vec2& inPos1, const Vec2& inPos2, const Vec2& inPos3, const Vec2& inPos4);	// テクスチャ座標の設定
	void initVtx(const int& inPatternNumU, const int& inPatternNumV);	// 頂点の初期設定
	void updateTexCoord(const Vec2& inMove);	// テクスチャ座標の更新 (移動)

private:
	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	ComPtr<IDirect3DVertexBuffer9> m_vtxBuf;	// 頂点バッファへのポインタ
	Vec3 m_pos;									// 位置
	Vec3 m_rot;									// 向き
	Vec3 m_size;								// サイズ
	Color m_col;								// 色
	std::string m_texKey;						// 使用するテクスチャのキー
	unique_ptr<Tex2dAnim> m_texAnim;			// テクスチャアニメーション情報
	bool m_isTexAnim;							// テクスチャアニメーションをするか否か
};
