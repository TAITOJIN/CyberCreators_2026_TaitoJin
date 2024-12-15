//--------------------------------------------------------------------------------
// 
// モデル [model.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/model.h"
// // // character
#include "game_object/3d/character/player.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Model::Model()
	: GameObject3D()
	, m_mat()
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
Model::Model(Model&& right) noexcept
	: GameObject3D(std::move(right))
	, m_mat(std::move(right.m_mat))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Model::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject3D::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		// リソースマネージャーの取得
		auto rm = getScene<SceneBase>()->getResManager();

		for (const auto& mat : inParam[FLAG_MAT])
		{
			const int LOCAL_MAT_NAME = mat[FLAG_MAT_NAME];
			const std::string LOCAL_MAT_KEY = mat[FLAG_MAT_KEY];

			m_mat[LOCAL_MAT_NAME].key = LOCAL_MAT_KEY;
			m_mat[LOCAL_MAT_NAME].mat = rm->getResMaterial(LOCAL_MAT_KEY).getMaterial();
		}
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Model::uninit()
{
	m_mat.clear();

	// 親クラスの処理
	GameObject3D::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Model::update()
{
	// 親クラスの処理
	GameObject3D::update();

	// マトリックスの更新
	GameObject3D::updateMtx();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Model::draw() const
{
#if ENABLED_SHADOW_MAP
	auto& gm = GM;
	// デバイスの取得
	const Device& device = gm.getDevice();
	auto effect = gm.getRenderer().getEffect();

	effect->BeginPass(0U);
	{
		Matrix mtxProj, mtxView;
		device->GetTransform(D3DTS_PROJECTION, &mtxProj);
		device->GetTransform(D3DTS_VIEW, &mtxView);
		Matrix* mtxWorld = getMatrix().get();
		Matrix mtxLight = getScene()->getLight()->getMatrix();
		Matrix wlp = DEF_MTX;
		D3DXMatrixMultiply(&wlp, mtxWorld, &mtxLight);
		D3DXMatrixMultiply(&wlp, &wlp, &mtxProj);
		effect->SetMatrix("mWLP", &wlp);
	}
	effect->CommitChanges();

	// 法線ベクトルを正規化
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// モデル情報の取得
	auto& resModel = getScene<SceneBase>()->getResManager()->getResModel(GameObject3D::getKey());

	GraphicsMaterial matDef;	// 現在のマテリアル保存用
	MeshMaterial* mat;			// マテリアルデータへのポインタ

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, GameObject3D::getMatrix().get());

	// 現在のマテリアルを取得
	device->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	mat = static_cast<MeshMaterial*>(resModel.getBufMat()->GetBufferPointer());

	for (DWORD i = 0; i < resModel.getNumMat(); i++)
	{
		// マテリアルの設定
#if 1
		device->SetMaterial(&m_mat[static_cast<int>(i)].mat);
#else
		device->SetMaterial(&mat[i].MatD3D);
#endif

		if (mat[i].pTextureFilename != nullptr)
		{
			const Texture& tex = getScene<SceneBase>()->getResManager()->getResTexture(resModel.getTexKeys(i)).getTexture();
			device->SetTexture(0U, tex);
		}
		else
		{
			effect->SetTexture(0U, nullptr);
		}

		effect->CommitChanges();

		// モデル (パーツ) の描画
		resModel.getMesh()->DrawSubset(i);
	}

	// 保存していたマテリアルを戻す
	device->SetMaterial(&matDef);

	// 法線ベクトルを元に戻す
	device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

	effect->EndPass();
#else
	// デバイスの取得
	auto& gm = GM;
	auto& renderer = gm.getRenderer();
	Device device = renderer.getDevice();

	// 法線ベクトルを正規化
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// モデル情報の取得
	auto& resModel = getScene<SceneBase>()->getResManager()->getResModel(GameObject3D::getKey());

	GraphicsMaterial matDef;	// 現在のマテリアル保存用
	MeshMaterial* mat;			// マテリアルデータへのポインタ

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, GameObject3D::getMatrix().get());

	// 現在のマテリアルを取得
	device->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	mat = static_cast<MeshMaterial*>(resModel.getBufMat()->GetBufferPointer());

	for (DWORD i = 0; i < resModel.getNumMat(); i++)
	{
		// マテリアルの設定
#if 1
		device->SetMaterial(&m_mat[static_cast<int>(i)].mat);
#else
		device->SetMaterial(&mat[i].MatD3D);
#endif
		// テクスチャの設定
		if (mat[i].pTextureFilename != nullptr)
		{ // テクスチャファイルが存在する
			const Texture& tex = getScene<SceneBase>()->getResManager()->getResTexture(resModel.getTexKeys(i)).getTexture();
			device->SetTexture(0U, tex);
			//effect->SetTexture("g_colorTexture", tex);
		}
		else
		{
			device->SetTexture(0U, nullptr);
			//effect->SetTexture("g_colorTexture", nullptr);
		}
		//effect->CommitChanges();
		device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

		// モデル (パーツ) の描画
		resModel.getMesh()->DrawSubset(i);
	}

	// 保存していたマテリアルを戻す
	device->SetMaterial(&matDef);

	// 法線ベクトルを元に戻す
	device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
#endif
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Model::drawWithShadow()
{
	auto& gm = GM;
	const Device& device = gm.getDevice();
	auto effect = gm.getRenderer().getEffect();

	{
		D3DXMATRIX mTexSpace;
		ZeroMemory(&mTexSpace, sizeof(D3DXMATRIX));
		mTexSpace._11 = 0.5;
		mTexSpace._22 = -0.5;
		mTexSpace._33 = 1;
		mTexSpace._41 = 0.5;
		mTexSpace._42 = 0.5;
		mTexSpace._44 = 1;

		Matrix mtxProj, mtxView;
		device->GetTransform(D3DTS_PROJECTION, &mtxProj);
		device->GetTransform(D3DTS_VIEW, &mtxView);

		Matrix* mtxWorld = getMatrix().get();

		auto scene = getScene();
		auto light = scene->getLight();
		Matrix mtxLight = light->getMatrix();
		Vec3 dir = light->getDir();

		Vec3 eye = scene->getCamera()->getPosV();

		Matrix wcp = DEF_MTX;
		D3DXMatrixMultiply(&wcp, mtxWorld, &mtxView);
		D3DXMatrixMultiply(&wcp, &wcp, &mtxProj);
		effect->SetMatrix("mWCP", &wcp);

		Matrix wlp = DEF_MTX;
		D3DXMatrixMultiply(&wlp, mtxWorld, &mtxLight);
		D3DXMatrixMultiply(&wlp, &wlp, &mtxProj);
		effect->SetMatrix("mWLP", &wlp);

		Matrix wlpt = DEF_MTX;
		D3DXMatrixMultiply(&wlpt, &wlp, &mTexSpace);
		effect->SetMatrix("mWLPT", &wlpt);

		effect->SetMatrix("mW", mtxWorld);

#if 0
		effect->SetVector("vLightDir", reinterpret_cast<D3DXVECTOR4*>(&dir));
#else
		Vec4 dir4 = Vec4(dir.x, dir.y, dir.z, 0.0f);
		effect->SetVector("vLightDir", &dir4);
#endif
#if 0
		effect->SetVector("vEye", reinterpret_cast<D3DXVECTOR4*>(&eye));
#else
		Vec4 eye4 = Vec4(eye.x, eye.y, eye.z, 0.0f);
		effect->SetVector("vEye", &eye4);
#endif
	}

	// 法線ベクトルを正規化
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// モデル情報の取得
	auto& resModel = getScene<SceneBase>()->getResManager()->getResModel(GameObject3D::getKey());

	GraphicsMaterial matDef;	// 現在のマテリアル保存用
	MeshMaterial* mat;			// マテリアルデータへのポインタ

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, GameObject3D::getMatrix().get());

	// 現在のマテリアルを取得
	device->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	mat = static_cast<MeshMaterial*>(resModel.getBufMat()->GetBufferPointer());

	for (DWORD i = 0; i < resModel.getNumMat(); i++)
	{
		// マテリアルの設定
#if 1
		device->SetMaterial(&m_mat[static_cast<int>(i)].mat);
#else
		device->SetMaterial(&mat[i].MatD3D);
#endif
		effect->SetValue("Diffuse", &m_mat[static_cast<int>(i)].mat.Diffuse, sizeof(FLOAT) * 4);
		effect->SetValue("Ambient", &m_mat[static_cast<int>(i)].mat.Ambient, sizeof(FLOAT) * 4);
		effect->SetValue("Specular", &m_mat[static_cast<int>(i)].mat.Specular, sizeof(FLOAT) * 4);
		effect->SetValue("Emissive", &m_mat[static_cast<int>(i)].mat.Emissive, sizeof(FLOAT) * 4);
		effect->SetValue("Power", &m_mat[static_cast<int>(i)].mat.Power, sizeof(FLOAT));

		if (mat[i].pTextureFilename != nullptr)
		{
			const Texture& tex = getScene<SceneBase>()->getResManager()->getResTexture(resModel.getTexKeys(i)).getTexture();
			effect->SetTexture("DecalTexture", tex);
		}
		else
		{
			effect->SetTexture("DecalTexture", nullptr);
		}

		effect->CommitChanges();

		// モデル (パーツ) の描画
		resModel.getMesh()->DrawSubset(i);
	}

	// 保存していたマテリアルを戻す
	device->SetMaterial(&matDef);

	// 法線ベクトルを元に戻す
	device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void Model::inspector()
{
	// 親クラスの処理
	GameObject3D::inspector();

	// マテリアルの設定
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto& mat : m_mat)
		{
			ImGui::Text("material name: %d", mat.first);

			auto& material = mat.second.mat;

			// マテリアルの種類
			{
				// マテリアルのリスト
				auto& materialList = getScene<SceneBase>()->getResManager()->getMaterials();

				static std::vector<const char*> items;
				items.clear();
				for (const auto& resMat : materialList)
				{
					items.emplace_back(resMat.first.c_str());
				}

				// 現在のマテリアルのキーの取得
				int i = 0;
				for (auto itr = items.begin(); itr != items.end(); ++itr)
				{
					if ((*itr) != mat.second.key)
					{
						++i;
						continue;
					}
					break;
				}

				if (ImGui::Combo("Select Material", &i, items.data(), items.size()))
				{
					// マテリアルのキーの取得
					int j = 0;
					for (auto itr = items.begin(); itr != items.end(); ++itr)
					{
						if (i != j)
						{
							++j;
							continue;
						}
						break;
					}

					// マテリアルのキーの設定
					mat.second.key = items[j];

					// マテリアルの設定
					material = materialList.find(items[j])->second->getMaterial();
				}
			}

			// カラーピッカーを表示
			ImGuiAddOns::Material("Diffuse", material.Diffuse);

			ImGui::Separator();
			ImGuiAddOns::Material("Ambient", material.Ambient);

			ImGui::Separator();
			ImGuiAddOns::Material("Specular", material.Specular);

			ImGui::Separator();
			ImGuiAddOns::Material("Emissive", material.Emissive);

			ImGui::Separator();
			ImGui::SliderFloat("Power", &material.Power, 0.f, 128.f);
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void Model::save(json& outObject) const
{
	// 親クラスの処理
	GameObject3D::save(outObject);

	for (const auto& mat : m_mat)
	{
		json modelMaterial;
		modelMaterial[FLAG_MAT_NAME] = mat.first;
		modelMaterial[FLAG_MAT_KEY] = mat.second.key;

		outObject[FLAG_MAT].push_back(modelMaterial);
	}
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
Model& Model::operator=(Model&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		GameObject3D::operator=(std::move(right));

		m_mat = std::move(right.m_mat);
	}

	return *this;
}
