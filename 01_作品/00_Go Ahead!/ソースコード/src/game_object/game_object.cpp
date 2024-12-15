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
#include "game_object/game_object.h"
// scene
#include "scene/scene_base.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
GameObject::GameObject()
	: m_isDestroy(false)
	, m_priority(static_cast<uint32_t>(SceneBase::BASE_PRIORITY::BLOCK))
{
	// DO_NOTHING
}

//---------------------------------------------------
// コピーコンストラクタ
//---------------------------------------------------
GameObject::GameObject(const GameObject& right)
	: m_isDestroy(right.m_isDestroy)
	, m_priority(right.m_priority)
	, m_scene(right.m_scene)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
GameObject::GameObject(GameObject&& right) noexcept
	: m_isDestroy(std::move(right.m_isDestroy))
	, m_priority(std::move(right.m_priority))
	, m_scene(std::move(right.m_scene))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT GameObject::init(const json& inParam)
{
	m_priority = inParam[GAMEOBJECT_PARAM_PRIORITY];
	m_tag = inParam[GAMEOBJECT_PARAM_TAG];

	return S_OK;
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void GameObject::inspector()
{
	// プライオリティの設定
	{
		int priority = static_cast<int>(m_priority);
		ImGuiAddOns::Int("Priority", &priority);
		if (priority != static_cast<int>(m_priority))
		{
			m_priority = static_cast<uint32_t>(priority);
		}
	}

	// タグの設定
	{
		static char inputBuf[MAX_WORD] = {};

		strncpy(&inputBuf[0], m_tag.c_str(), sizeof(inputBuf) - 1);
		inputBuf[sizeof(inputBuf) - 1] = '\0';

		if (ImGui::InputText("tag", inputBuf, sizeof(inputBuf)))
		{
			m_tag = inputBuf;
		}
		//ImGui::SameLine();
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void GameObject::save(json& outObject) const
{
	outObject[GAMEOBJECT_PARAM_PRIORITY] = m_priority;
	outObject[GAMEOBJECT_PARAM_TAG] = m_tag;
}

//---------------------------------------------------
// デルタタイムの取得
//---------------------------------------------------
const float& GameObject::getDeltaTime()
{
	return m_scene.lock()->getTime()->getDeltaTime();
}

//---------------------------------------------------
// コピー代入演算子
//---------------------------------------------------
GameObject& GameObject::operator=(const GameObject& right)
{
	if (this != &right)
	{
		m_isDestroy = right.m_isDestroy;
		m_priority = right.m_priority;
		m_scene = right.m_scene;
	}

	return *this;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
GameObject& GameObject::operator=(GameObject&& right) noexcept
{
	if (this != &right)
	{
		m_isDestroy = std::move(right.m_isDestroy);
		m_priority = std::move(right.m_priority);
		m_scene = std::move(right.m_scene);
	}

	return *this;
}
