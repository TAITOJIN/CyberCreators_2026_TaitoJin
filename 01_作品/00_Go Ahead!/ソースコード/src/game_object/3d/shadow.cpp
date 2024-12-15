//--------------------------------------------------------------------------------
// 
// 影 [shadow.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/shadow.h"
// scene
// // elements
#include "scene/elements/light.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Shadow::Shadow(shared_ptr<Matrix> inMtx)
	: m_mtxWorld(DEF_MTX)
	, m_mtxParent(inMtx)
	, m_pos(DEF_VEC3)
	, m_rot(DEF_VEC3)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
Shadow::Shadow(Shadow&& right) noexcept
	: m_key(std::move(right.m_key))
	, m_mtxWorld(std::move(right.m_mtxWorld))
	, m_mtxParent(std::move(right.m_mtxParent))
	, m_pos(std::move(right.m_pos))
	, m_rot(std::move(right.m_rot))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Shadow::update()
{
#if 1
	// デバイスの取得
	Device device = GM.getDevice();

	Matrix mtxScale, mtxRot, mtxTrans;	// 計算用マトリックス
	Plane plane;						// 平面情報
	Vec4 vecLight;						// ライトの逆方向
	Vec3 pos, nor;						// 平面上の 1 点と法線

	// ライトの逆方向ベクトルを設定
	vecLight = Vec4(-500.0f, 1000.0f, -500.0f, 0.0f);
	pos = DEF_VEC3;
	nor = Vec3(0.0f, 1.0f, 0.0f);

	// 法線と平面上の 1 点から平面情報を作成
	D3DXPlaneFromPointNormal(&plane, &pos, &nor);

	// ライトと平面情報から影行列を作成
	D3DXMatrixShadow(&m_mtxWorld, &vecLight, &plane);

	Vec3 scale = DEF_VEC3;
	Vec3 x = Vec3(m_mtxParent.lock()->_11, m_mtxParent.lock()->_21, m_mtxParent.lock()->_31);
	Vec3 y = Vec3(m_mtxParent.lock()->_12, m_mtxParent.lock()->_22, m_mtxParent.lock()->_32);
	Vec3 z = Vec3(m_mtxParent.lock()->_13, m_mtxParent.lock()->_23, m_mtxParent.lock()->_33);

	scale.x = D3DXVec3Length(&x);
	scale.y = D3DXVec3Length(&y);
	scale.z = D3DXVec3Length(&z);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, scale.x, scale.y, scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, &m_mtxWorld);
#else
	// デバイスの取得
	Device device = GM.getDevice();

	Matrix mtxScale, mtxRot, mtxTrans;	// 計算用マトリックス
	Plane plane;						// 平面情報
	Vec4 vecLight;						// ライトの逆方向
	Vec3 pos, nor;						// 平面上の 1 点と法線

	// ライトの逆方向ベクトルを設定
	vecLight = Vec4(-500.0f, 1000.0f, -500.0f, 0.0f);
	pos = DEF_VEC3;
	nor = Vec3(0.0f, 1.0f, 0.0f);

	// 法線と平面上の 1 点から平面情報を作成
	D3DXPlaneFromPointNormal(&plane, &pos, &nor);

	// ライトと平面情報から影行列を作成
	D3DXMatrixShadow(&m_mtxWorld, &vecLight, &plane);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, m_mtxParent.lock().get());

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, &m_mtxWorld);
#endif
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Shadow::draw(const ResourceModel& inModel) const
{
	// デバイスの取得
	Device device = GM.getDevice();

	// 法線ベクトルを正規化
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	GraphicsMaterial matDef;	// 現在のマテリアル保存用
	MeshMaterial* mat;			// マテリアルデータへのポインタ

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	device->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	mat = static_cast<MeshMaterial*>(inModel.getBufMat()->GetBufferPointer());

	for (DWORD i = 0; i < inModel.getNumMat(); i++)
	{
		// マテリアルの設定
		GraphicsMaterial material = DEF_MAT;
		material.Diffuse = ColVal(0.0f, 0.0f, 0.0f, 1.0f);
		device->SetMaterial(&material);

		// テクスチャの設定
		device->SetTexture(0U, nullptr);

		// モデル (パーツ) の描画
		inModel.getMesh()->DrawSubset(i);
	}

	// 保存していたマテリアルを戻す
	device->SetMaterial(&matDef);

	// 法線ベクトルを元に戻す
	device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
Shadow& Shadow::operator=(Shadow&& right) noexcept
{
	if (this != &right)
	{
		m_key = std::move(right.m_key);
		m_mtxWorld = std::move(right.m_mtxWorld);
		m_mtxParent = std::move(right.m_mtxParent);
		m_pos = std::move(right.m_pos);
		m_rot = std::move(right.m_rot);
	}

	return *this;
}
