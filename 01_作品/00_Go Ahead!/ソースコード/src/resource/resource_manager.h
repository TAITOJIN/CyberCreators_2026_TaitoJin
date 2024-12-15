//--------------------------------------------------------------------------------
// 
// リソースのマネージャー (管理者) [resource_manager.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// resource
#include "resource/resource_texture.h"
#include "resource/resource_model.h"
#include "resource/resource_material.h"

//===================================================
// リソースの管理者のクラス
//===================================================
class ResourceManager
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	ResourceManager();
	~ResourceManager() = default;

	// 返り値 key
	std::string loadTexture(const json& inTexture);		// テクスチャの読み込み処理
	std::string loadModel(const json& inModel);			// モデルの読み込み処理
	std::string loadMaterial(const json& inMaterial);	// マテリアルの読み込み処理

	void loadTextures(const std::string& inListFilename);	// テクスチャリストの読み込み
	void loadModels(const std::string& inListFilename);		// モデルリストの読み込み
	void loadmaterials(const std::string& inListFilename);	// マテリアルリストの読み込み

	const ResourceTexture& getResTexture(const std::string& inKey) { assert(m_textures.count(inKey) != 0); return *m_textures[inKey]; }	// テクスチャの取得
	const ResourceModel& getResModel(const std::string& inKey) { assert(m_models.count(inKey) != 0); return *m_models[inKey]; }	// モデルの取得
	const ResourceMaterial& getResMaterial(const std::string& inKey) { assert(m_materials.count(inKey) != 0); return *m_materials[inKey]; }	// マテリアルの取得

	std::unordered_map<std::string, unique_ptr<ResourceTexture>>& getTextures() { return m_textures; }		// テクスチャの一覧の取得
	std::unordered_map<std::string, unique_ptr<ResourceModel>>& getModels() { return m_models; }			// モデルの一覧の取得
	std::unordered_map<std::string, unique_ptr<ResourceMaterial>>& getMaterials () { return m_materials; }	// マテリアルの一覧の取得

	void addMaterial(const std::string& inKey, ResourceMaterial* inMat);	// マテリアルの追加

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	ResourceManager(const ResourceManager&) = delete;
	void operator=(const ResourceManager&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::unordered_map<std::string, unique_ptr<ResourceTexture>> m_textures;	// テクスチャ
	std::unordered_map<std::string, unique_ptr<ResourceModel>> m_models;		// モデル
	std::unordered_map<std::string, unique_ptr<ResourceMaterial>> m_materials;	// マテリアル
};
