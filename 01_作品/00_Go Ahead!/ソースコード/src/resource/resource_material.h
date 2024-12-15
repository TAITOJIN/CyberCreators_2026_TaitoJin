//--------------------------------------------------------------------------------
// 
// マテリアルのリソース [resource_material.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// マテリアル情報のクラス
//===================================================
class ResourceMaterial
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	ResourceMaterial();
	~ResourceMaterial() = default;

	void setMaterial(const GraphicsMaterial& inMat) { m_material = inMat; }	// マテリアルの設定
	const GraphicsMaterial& getMaterial() const { return m_material; };		// マテリアルの取得

private:
	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	GraphicsMaterial m_material;	// マテリアル
};
