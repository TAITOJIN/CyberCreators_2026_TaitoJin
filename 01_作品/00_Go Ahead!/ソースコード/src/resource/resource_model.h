//--------------------------------------------------------------------------------
// 
// モデルのリソース [resource_model.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

// utility
#include "Utility/collision.h"

//===================================================
// モデル情報のクラス
//===================================================
class ResourceModel
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	ResourceModel();
	~ResourceModel() = default;

	void setMesh(Mesh inMesh) { m_mesh.Attach(inMesh); inMesh = nullptr; }								// メッシュ (頂点情報) へのポインタの設定
	void setBufMat(BufferPointer inBufMat) { m_bufMat.Attach(inBufMat); inBufMat = nullptr; }			// マテリアルへのポインタの設定
	void setNumMat(const DWORD& inNumMat) { m_numMat = inNumMat; }										// マテリアルの数の設定
	void setTexKeys(const std::unordered_map<int, std::string>& inTexKeys) { m_texKeys = inTexKeys; }	// テクスチャのキーの設定
	void setAABB(const Collision::AABB& inAABB) { m_aabb = inAABB; }									// AABB の設定

	Mesh getMesh() const { return m_mesh.Get(); }				// メッシュ (頂点情報) へのポインタの取得
	BufferPointer getBufMat() const { return m_bufMat.Get(); }	// マテリアルへのポインタの取得
	const DWORD& getNumMat() const { return m_numMat; }			// マテリアルの数の取得
	const std::string& getTexKeys(int inKey) const { assert(m_texKeys.count(inKey) != 0); return m_texKeys[inKey]; }	// テクスチャのキーの取得
	const Collision::AABB& getAABB() const { return m_aabb; }	// AABB の取得

private:
	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	ComPtr<ID3DXMesh> m_mesh;						// メッシュ (頂点情報) へのポインタ
	ComPtr<ID3DXBuffer> m_bufMat;					// マテリアルへのポインタ
	DWORD m_numMat;									// マテリアルの数
	mutable std::unordered_map<int, std::string> m_texKeys;	// テクスチャのキー
	Collision::AABB m_aabb;							// AABB
};
