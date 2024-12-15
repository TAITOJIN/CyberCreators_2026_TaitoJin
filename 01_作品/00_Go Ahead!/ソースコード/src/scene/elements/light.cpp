//--------------------------------------------------------------------------------
// 
// ライト [light.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
// // elements
#include "scene/elements/light.h"
// game_manager
#include "game_manager/game_manager.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	const Vec3 DEF_LIGHT_VIEW = Vec3(100.0f, 1000.0f, 100.0f);	// ライトの視点

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Light3D::Light3D()
	: GameObject()
	, m_light()
	, m_idx(0)
	, m_vLight(DEF_LIGHT_VIEW)
	, m_vLook(DEF_VEC3)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Light3D::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject::init(inParam);
	if (FAILED(hr))
	{
		return hr;
	}

	auto& gm = GM;
	gm.lockCS();

	// デバイスの取得
	Device device = gm.getDevice();

	// 初期パラメータの設定
	{
		Vec3 dir = inParam[LIGHT_PARAM_DIR];
		Color diffuse = inParam[LIGHT_PARAM_DIFFUSE];
		uint32_t idx = inParam[LIGHT_PARAM_IDX];

		m_light.Type = D3DLIGHT_DIRECTIONAL;
		m_light.Direction = *D3DXVec3Normalize(&dir, &dir);
		m_light.Diffuse = diffuse;
		m_idx = idx;

		m_light.Range = 1000;
		m_vLight = Vec3(0.0f, 1000.0f, 10.0f);
		m_vLook = D3DXVECTOR3(0, 0, 0);
	}

	// デバイスに設定
	device->SetLight(m_idx, &m_light);
	device->LightEnable(m_idx, TRUE);

	gm.unlockCS();

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Light3D::uninit()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Light3D::update()
{
	// DO_NOTHING

#ifdef _DEBUG
	auto& im = GM.getInputManager();
	auto& keyboard = im.getKeyboard();

	if (keyboard.getPress(DIK_U))
	{
		m_vLight.y += 1.0f;
	}
	if (keyboard.getPress(DIK_J))
	{
		m_vLight.y -= 1.0f;
	}

	if (keyboard.getPress(DIK_H))
	{
		m_vLight.x -= 1.0f;
	}
	if (keyboard.getPress(DIK_L))
	{
		m_vLight.x += 1.0f;
	}

	#if 0
	GM.cs([this]() { std::cout << std::format("y: {}, x: {}", m_vLight.y, m_vLight.x) << std::endl; });
	#endif
#endif // _DEBUG

#if 0
	Vec3 pos = getScene()->getPlayer()->getPos();
	m_vLight = Vec3(pos.x - 100.0f, pos.y + 100.0f, pos.z + -100.0f);
	m_vLook = pos;
#endif
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Light3D::draw() const
{
	// DO_NOTHING
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void Light3D::inspector()
{
	// 親クラスの処理
	GameObject::inspector();

	// Config の設定
	if (ImGui::CollapsingHeader("Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 傾きの設定
		{
			Vec3 dir = m_light.Direction;
			bool isReturn = ImGuiAddOns::Transform("Direction", &dir);

			if (isReturn)
			{
				// 反映
				m_light.Direction = dir;
			}
		}

		// 色の設定
		ImGuiAddOns::Material("Diffuse", m_light.Diffuse);
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void Light3D::save(json& outObject) const
{
	// 親クラスの処理
	GameObject::save(outObject);

	outObject[LIGHT_PARAM_DIR] = m_light.Direction;
	outObject[LIGHT_PARAM_DIFFUSE] = m_light.Diffuse;
	outObject[LIGHT_PARAM_IDX] = m_idx;
}

//---------------------------------------------------
// ライト空間への投影行列の設定処理
//---------------------------------------------------
void Light3D::setLightMatrix()
{
	// ライト空間への投影行列
	static const Vec3 vUp = Vec3(0.0f, 1.0f, 0.0f);
	//D3DXMATRIX mRotation;
	//D3DXMatrixRotationY(&mRotation, 0.01);
	//D3DXVec3TransformCoord(&m_vLight, &m_vLight, &mRotation);
	D3DXMatrixLookAtLH(&m_mtxWorld, &m_vLight, &m_vLook, &vUp);
}
