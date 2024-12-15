//--------------------------------------------------------------------------------
// 
// json のカスタム [custom_json.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// D3DXVECTOR2
//---------------------------------------------------
#pragma region D3DXVECTOR2
void to_json(json& j, const D3DXVECTOR2& v);
void from_json(const json& j, D3DXVECTOR2& v);
#pragma endregion

//---------------------------------------------------
// D3DXVECTOR3
//---------------------------------------------------
#pragma region D3DXVECTOR3
void to_json(json& j, const D3DXVECTOR3& v);
void from_json(const json& j, D3DXVECTOR3& v);
#pragma endregion

//---------------------------------------------------
// D3DXQUATERNION
//---------------------------------------------------
#pragma region D3DXQUATERNION
void to_json(json& j, const D3DXQUATERNION& q);
void from_json(const json& j, D3DXQUATERNION& q);
#pragma endregion

//---------------------------------------------------
// D3DCOLORVALUE
//---------------------------------------------------
#pragma region D3DCOLORVALUE
void to_json(json& j, const D3DCOLORVALUE& c);
void from_json(const json& j, D3DCOLORVALUE& c);
#pragma endregion

//---------------------------------------------------
// D3DXCOLOR
//---------------------------------------------------
#pragma region D3DXCOLOR
void to_json(json& j, const D3DXCOLOR& c);
void from_json(const json& j, D3DXCOLOR& c);
#pragma endregion

//---------------------------------------------------
// D3DMATERIAL9
//---------------------------------------------------
#pragma region D3DMATERIAL9
void to_json(json& j, const D3DMATERIAL9& m);
void from_json(const json& j, D3DMATERIAL9& m);
#pragma endregion
