//--------------------------------------------------------------------------------
// 
// 消えるブロック [block_disappear.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_disappear.h"
// game_manager
#include "game_manager/game_manager.h"
// scene
#include "scene/scene_debug.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"

//---------------------------------------------------
// static メンバ変数
//---------------------------------------------------
//std::list<BlockDisappear*> BlockDisappear::m_blockDisappearList;	// 消えるブロックのリスト

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockDisappear::BlockDisappear()
	: BlockBase()
	, m_firstAppearState(false)
	, m_isAppear(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockDisappear::BlockDisappear(BlockDisappear&& right) noexcept
	: BlockBase(std::move(right))
	, m_firstAppearState(std::move(right.m_firstAppearState))
	, m_isAppear(std::move(right.m_isAppear))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockDisappear::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockBase::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_firstAppearState = inParam[FLAG_FIRST_APPEAR];
		m_isAppear = m_firstAppearState;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockDisappear::uninit()
{
	// 親クラスの処理
	BlockBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockDisappear::update()
{
#if 0
	auto gameManager = GM;							// ゲームの管理者
	float deltaTime = gameManager->getDeltaTime();	// デルタタイム
	auto scene = gameManager->getScene();			// シーン
	bool isEdit = false;							// 編集中かどうか

	// 編集中かどうかの設定
	if (auto sceneDebug = dynamic_cast<SceneDebug*>(scene))
	{
		isEdit = sceneDebug->getIsEdit();
	}

	if (!isEdit)
	{

	}
#endif

	// 親クラスの処理
	BlockBase::update();

#if 0
	if (getIsDestroy())
	{
		// 消えるブロック一覧から削除
		auto itr = std::find(m_blockDisappearList.begin(), m_blockDisappearList.end(), this);
		assert(itr != m_blockDisappearList.end());
		m_blockDisappearList.erase(itr);
	}
#endif
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockDisappear::draw() const
{
	if (!m_isAppear)
	{
		return;
	}

	// 親クラスの処理
	BlockBase::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockDisappear::inspector()
{
	// 親クラスの処理
	BlockBase::inspector();

	// 情報の設定
	if (ImGui::CollapsingHeader("Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 最初の出現状態
		{
			bool prevIsAppear = m_firstAppearState;
			ImGuiAddOns::ToggleButton("firstAppearState", &m_firstAppearState);
			ImGui::SameLine();
			ImGui::Text("isAppear (first)");

			if (prevIsAppear != m_firstAppearState)
			{
				m_isAppear = m_firstAppearState;

				int collisionFlags = btCollisionObject::CF_NO_CONTACT_RESPONSE;
				if (m_isAppear)
				{
					collisionFlags = btCollisionObject::CF_STATIC_OBJECT;
				}
				getRigitBody().setCollisionFlags(collisionFlags);
			}
		}

		// 現在の出現状態
		{
			bool prevIsAppear = m_isAppear;
			ImGuiAddOns::ToggleButton("isAppear", &m_isAppear);
			ImGui::SameLine();
			ImGui::Text("isAppear (now)");

			if (prevIsAppear != m_isAppear)
			{
				int collisionFlags = btCollisionObject::CF_NO_CONTACT_RESPONSE;
				if (m_isAppear)
				{
					collisionFlags = btCollisionObject::CF_STATIC_OBJECT;
				}
				getRigitBody().setCollisionFlags(collisionFlags);
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockDisappear::save(json& outObject) const
{
	// 親クラスの処理
	BlockBase::save(outObject);

	outObject[FLAG_FIRST_APPEAR] = m_firstAppearState;
}

#if 0
//---------------------------------------------------
// 出現状態の切替
//---------------------------------------------------
void BlockDisappear::switchAppearState()
{
	for (auto& block : m_blockDisappearList)
	{
		block->m_isAppear = !block->m_isAppear;

		auto rigidBody = block->getRigitBody();

		if (rigidBody == nullptr)
		{
			continue;
		}

		int collisionFlags = btCollisionObject::CF_NO_CONTACT_RESPONSE;
		if (block->m_isAppear)
		{
			collisionFlags = btCollisionObject::CF_STATIC_OBJECT;
			
		}
		rigidBody->setCollisionFlags(collisionFlags);
	}
}

//---------------------------------------------------
// リストの初期化
//---------------------------------------------------
void BlockDisappear::initList()
{
	m_blockDisappearList.clear();
}
#endif

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockDisappear& BlockDisappear::operator=(BlockDisappear&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockBase::operator=(std::move(right));

		m_firstAppearState = std::move(right.m_firstAppearState);
		m_isAppear = std::move(right.m_isAppear);
	}

	return *this;
}
