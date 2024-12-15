//--------------------------------------------------------------------------------
// 
// ゲームオブジェクト [game_object.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/game_object_factory.h"

//---------------------------------------------------
// 登録
//---------------------------------------------------
void GameObjectFactory::registerFactory(const std::string& inType, CreateFunc inFunc)
{
	getFactories().emplace(inType, inFunc);
}

//---------------------------------------------------
// 生成
//---------------------------------------------------
std::shared_ptr<GameObject> GameObjectFactory::create(const std::string& inType, const shared_ptr<SceneBase>& inScene, const json& inData)
{
	auto it = getFactories().find(inType);
	if (it == getFactories().end())
	{
		assert(false);
		return nullptr;
	}

	return it->second(inScene, inData);
}

//---------------------------------------------------
// ファクトリーメソッドの取得
//---------------------------------------------------
std::unordered_map<std::string, GameObjectFactory::CreateFunc>& GameObjectFactory::getFactories()
{
	static std::unordered_map<std::string, CreateFunc> map;
	return map;
}
