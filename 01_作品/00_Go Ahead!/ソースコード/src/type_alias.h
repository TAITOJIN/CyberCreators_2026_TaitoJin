//--------------------------------------------------------------------------------
// 
// エイリアス宣言 [type_alias.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

// Json
#if 1
using json = nlohmann::json;
#else
using json = nlohmann::ordered_json;
#endif
using json_validator = nlohmann::json_schema::json_validator;

// DirectX 9
using Vec2 = D3DXVECTOR2;
using Vec3 = D3DXVECTOR3;
using Vec4 = D3DXVECTOR4;
using Matrix = D3DXMATRIX;
using Quaternion = D3DXQUATERNION;
using Color = D3DXCOLOR;
using ColVal = D3DCOLORVALUE;
using Device = LPDIRECT3DDEVICE9;
using Texture = LPDIRECT3DTEXTURE9;
using VertexBuffer = LPDIRECT3DVERTEXBUFFER9;
using IndexBuffer = LPDIRECT3DINDEXBUFFER9;
using Mesh = LPD3DXMESH;
using BufferPointer = LPD3DXBUFFER;
using MeshMaterial = D3DXMATERIAL;
using GraphicsMaterial = D3DMATERIAL9;
using Light = D3DLIGHT9;
using Viewport = D3DVIEWPORT9;
using Surface = LPDIRECT3DSURFACE9;
using Plane = D3DXPLANE;

//---------------------------------------------------
// using 宣言
//---------------------------------------------------
// Microsoft
// // WRL
using Microsoft::WRL::ComPtr;

// memory
using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;
using std::make_unique;
using std::make_shared;
