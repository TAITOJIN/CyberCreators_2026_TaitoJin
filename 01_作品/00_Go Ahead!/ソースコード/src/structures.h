//--------------------------------------------------------------------------------
// 
// 構造体 [structures.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

// 頂点情報 [2D] の構造体
struct VERTEX_2D
{
	D3DXVECTOR3 pos;	// 頂点座標
	float rhw;			// 座標変換用係数 (1.0f で固定)
	D3DCOLOR col;		// 頂点カラー
	D3DXVECTOR2 tex;	// テクスチャ座標
};

// 頂点情報 [3D] の構造体
struct VERTEX_3D
{
	D3DXVECTOR3 pos;	// 頂点座標
	D3DXVECTOR3 nor;	// 法線ベクトル
	D3DCOLOR col;		// 頂点カラー
	D3DXVECTOR2 tex;	// テクスチャ座標
};
