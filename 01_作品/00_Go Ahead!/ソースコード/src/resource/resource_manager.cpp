//--------------------------------------------------------------------------------
// 
// リソースのマネージャー (管理者) [resource_manager.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// manager
#include "resource/resource_manager.h"
#include "game_manager/game_manager.h"
// utility
#include "utility/string.h"
#include "utility/file.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
ResourceManager::ResourceManager()
{
	// DO_NOTHING
}

//---------------------------------------------------
// テクスチャの読み込み処理
//---------------------------------------------------
std::string ResourceManager::loadTexture(const json& inTexture)
{
	// パラメータの取り出し
	std::string filename = inTexture[FLAG_TEX_FILENAME];
	int u = inTexture[FLAG_TEX_SUBDIVISION_COUNT_U];
	int v = inTexture[FLAG_TEX_SUBDIVISION_COUNT_V];
	int animInterval = inTexture[FLAG_TEX_ANIMATION_INTERVAL];

	// キーの確認
	const std::string key = filename;
	if (m_textures.count(key) != 0)
	{ // 既にキーが存在する
		return key;
	}

	// 生成
	unique_ptr<ResourceTexture> resTex = make_unique<ResourceTexture>();
	if (!resTex)
	{
		assert(false);
		return "";
	}

	// テクスチャの読み込みと設定
	{
		auto& gm = GM;
		gm.lockCS();

		// デバイスの取得
		Device device = gm.getDevice();

		// テクスチャの読み込み
		Texture texture = nullptr;
		D3DXCreateTextureFromFile(
			device,
			filename.c_str(),
			&texture);

		gm.unlockCS();

		if (!texture)
		{
			assert(false);
			return "";
		}

		// パラメータの設定
		resTex->setTexture(texture);
		resTex->setU(u);
		resTex->setV(v);
		resTex->setAnimInterVal(animInterval);
	}

	// 追加
	m_textures.emplace(key, std::move(resTex));

	return key;
}

//---------------------------------------------------
// モデルの読み込み処理
//---------------------------------------------------
std::string ResourceManager::loadModel(const json& inModel)
{
	// パラメータの取り出し
	std::string filename = inModel[FLAG_MODEL_FILENAME];

	// キーの確認
	const std::string key = filename;
	if (m_models.count(key) != 0)
	{ // 既にキーが存在する
		return key;
	}

	// 生成
	unique_ptr<ResourceModel> resModel = make_unique<ResourceModel>();
	if (!resModel)
	{
		assert(false);
		return "";
	}

	// モデルの読み込みと設定
	{
		auto& gm = GM;
		gm.lockCS();

		// デバイスの取得
		Device device = gm.getDevice();

		// X ファイルの読み込み
		Mesh mesh = nullptr;
		BufferPointer bufMat = nullptr;
		DWORD numMat = 0;
		D3DXLoadMeshFromX(
			filename.c_str(),
			D3DXMESH_SYSTEMMEM,
			device,
			nullptr,
			&bufMat,
			nullptr,
			&numMat,
			&mesh);

		gm.unlockCS();

		// マテリアルデータへのポインタを取得
		MeshMaterial* mat = static_cast<MeshMaterial*>(bufMat->GetBufferPointer());

		// テクスチャのキーの設定
		std::unordered_map<int, std::string> texKeys;
		for (DWORD i = 0; i < numMat; i++)
		{
			// テクスチャの存在確認
			if (!mat[i].pTextureFilename)
			{
				continue;
			}

			// テクスチャのキーの追加
			const std::string texKey = mat[i].pTextureFilename;
			texKeys.emplace(i, texKey);
		}

		// AABB の設定
		Collision::AABB aabb;
		Collision::setAABB(mesh, &aabb);

		// パラメータの設定
		resModel->setMesh(mesh);
		resModel->setBufMat(bufMat);
		resModel->setNumMat(numMat);
		resModel->setTexKeys(texKeys);
		resModel->setAABB(aabb);
	}

	// 追加
	m_models.emplace(key, std::move(resModel));

	return key;
}

//---------------------------------------------------
// マテリアルの読み込み処理
//---------------------------------------------------
std::string ResourceManager::loadMaterial(const json& inMaterial)
{
	// パラメータの取り出し
	const std::string key = inMaterial[FLAG_MAT_KEY];
	GraphicsMaterial material = inMaterial[FLAG_MAT];

	if (m_materials.count(key) != 0)
	{ // 既にキーが存在する
		return key;
	}

	// マテリアルの読み込み
	unique_ptr<ResourceMaterial> resMat = make_unique<ResourceMaterial>();
	if (!resMat)
	{
		assert(false);
		return "";
	}

	// パラメータの設定
	resMat->setMaterial(material);

	// 追加
	m_materials.emplace(key, std::move(resMat));

	return key;
}

//---------------------------------------------------
// テクスチャリストの読み込み
//---------------------------------------------------
void ResourceManager::loadTextures(const std::string& inListFilename)
{
	json j = File::Json::load(inListFilename.c_str());

	for (const auto& tex : j)
	{
		loadTexture(tex);
	}
}

//---------------------------------------------------
// モデルリストの読み込み
//---------------------------------------------------
void ResourceManager::loadModels(const std::string& inListFilename)
{
	json j = File::Json::load(inListFilename.c_str());

	for (const auto& model : j)
	{
		loadModel(model);
	}
}

//---------------------------------------------------
// マテリアルリストの読み込み
//---------------------------------------------------
void ResourceManager::loadmaterials(const std::string& inListFilename)
{
	json j = File::Json::load(inListFilename.c_str());

	for (const auto& mat : j)
	{
		loadMaterial(mat);
	}
}

//---------------------------------------------------
// マテリアルの追加
//---------------------------------------------------
void ResourceManager::addMaterial(const std::string& inKey, ResourceMaterial* inMat)
{
	if (m_materials.count(inKey) != 0)
	{ // 既に存在している
		return;
	}

	unique_ptr<ResourceMaterial> mat(inMat);
	m_materials[inKey] = std::move(mat);
}
