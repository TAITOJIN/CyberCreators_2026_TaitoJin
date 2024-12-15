//--------------------------------------------------------------------------------
// 
// 影 [shadow.h]
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
// resource
#include "resource/resource_model.h"

//===================================================
// 影のクラス
//===================================================
class Shadow
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Shadow(shared_ptr<Matrix> inMtx);
	~Shadow() = default;
	Shadow(Shadow&& right) noexcept;
	Shadow& operator=(Shadow&& right) noexcept;

	void update();									// 更新処理
	void draw(const ResourceModel& inModel) const;	// 描画処理

	void setKey(const std::string& inKey) { m_key = inKey; }				// キーの設定
	void setMtxWorld(const Matrix& inMtx) { m_mtxWorld = inMtx; }			// ワールドマトリックスの設定
	void setMtxParent(shared_ptr<Matrix> inMtx) { m_mtxParent = inMtx; }	// 親マトリックスの設定
	void setPos(const Vec3& inPos) { m_pos = inPos; }						// 位置の設定
	void setRot(const Vec3& inRot) { m_rot = inRot; }						// 向きの設定

	const std::string& getKey() const { return m_key; }						// キーの取得
	const Matrix& getMtxWorld() const { return m_mtxWorld; }				// ワールドマトリックスの取得
	const weak_ptr<Matrix>& getMtxParent() const { return m_mtxParent; }	// 親マトリックスの取得
	const Vec3& getPos() const { return m_pos; }							// 位置の取得
	const Vec3& getRot() const { return m_rot; }							// 向きの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Shadow(const Shadow&) = delete;
	void operator=(const Shadow&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::string m_key;				// キー
	Matrix m_mtxWorld;				// ワールドマトリックス
	weak_ptr<Matrix> m_mtxParent;	// 親のマトリックス
	Vec3 m_pos;						// 位置
	Vec3 m_rot;						// 向き
};
