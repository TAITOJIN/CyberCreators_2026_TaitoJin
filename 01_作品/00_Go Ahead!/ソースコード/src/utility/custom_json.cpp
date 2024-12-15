//--------------------------------------------------------------------------------
// 
// json のカスタム [custom_json.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// utility
#include "utility/custom_json.h"

//---------------------------------------------------
// D3DXVECTOR2
//---------------------------------------------------
#pragma region D3DXVECTOR2
void to_json(json& j, const D3DXVECTOR2& v)
{
	j = json{ {"x", v.x}, {"y", v.y} };
}

void from_json(const json& j, D3DXVECTOR2& v)
{
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
}
#pragma endregion

//---------------------------------------------------
// D3DXVECTOR3
//---------------------------------------------------
#pragma region D3DXVECTOR3
void to_json(json& j, const D3DXVECTOR3& v)
{
	j = json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

void from_json(const json& j, D3DXVECTOR3& v)
{
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
	j.at("z").get_to(v.z);
}
#pragma endregion

//---------------------------------------------------
// D3DXQUATERNION
//---------------------------------------------------
#pragma region D3DXQUATERNION
void to_json(json& j, const D3DXQUATERNION& q)
{
	j = json{ {"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w} };
}

void from_json(const json& j, D3DXQUATERNION& q)
{
	j.at("x").get_to(q.x);
	j.at("y").get_to(q.y);
	j.at("z").get_to(q.z);
	j.at("w").get_to(q.w);
}
#pragma endregion

//---------------------------------------------------
// D3DCOLORVALUE
//---------------------------------------------------
#pragma region D3DCOLORVALUE
void to_json(json& j, const D3DCOLORVALUE& c)
{
	j = json{ {"r", c.r}, {"g", c.g}, {"b", c.b}, {"a", c.a} };
}

void from_json(const json& j, D3DCOLORVALUE& c)
{
	j.at("r").get_to(c.r);
	j.at("g").get_to(c.g);
	j.at("b").get_to(c.b);
	j.at("a").get_to(c.a);
}
#pragma endregion

//---------------------------------------------------
// D3DXCOLOR
//---------------------------------------------------
#pragma region D3DXCOLOR
void to_json(json& j, const D3DXCOLOR& c)
{
	j = json{ {"r", c.r}, {"g", c.g}, {"b", c.b}, {"a", c.a} };
}

void from_json(const json& j, D3DXCOLOR& c)
{
	j.at("r").get_to(c.r);
	j.at("g").get_to(c.g);
	j.at("b").get_to(c.b);
	j.at("a").get_to(c.a);
}
#pragma endregion

//---------------------------------------------------
// D3DMATERIAL9
//---------------------------------------------------
#pragma region D3DMATERIAL9
void to_json(json& j, const D3DMATERIAL9& m)
{
	j = json{ {"Diffuse", m.Diffuse}, {"Ambient", m.Ambient}, {"Specular", m.Specular}, {"Emissive", m.Emissive}, {"Power", m.Power} };
}

void from_json(const json& j, D3DMATERIAL9& m)
{
	j.at("Diffuse").get_to(m.Diffuse);
	j.at("Ambient").get_to(m.Ambient);
	j.at("Specular").get_to(m.Specular);
	j.at("Emissive").get_to(m.Emissive);
	j.at("Power").get_to(m.Power);
}
#pragma endregion
