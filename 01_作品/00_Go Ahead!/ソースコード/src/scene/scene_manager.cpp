//--------------------------------------------------------------------------------
// 
// シーンの管理者 [scene_manager.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_manager.h"
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
	constexpr size_t THREAD_POOL_NUM = 4;		// スレッドプールの数
	constexpr int GO_NEXT_SCENE_INTERVVAL = 50;	// 次のシーンまでの確定で待つ時間 (フェードしきる用)

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
SceneManager::SceneManager()
	:  m_sharedData(std::make_shared<SharedData>())
	, m_eventManager(std::make_shared<EventManager>(THREAD_POOL_NUM))
	, m_isFadeInCompleted(false)
	, m_isLoadingCompleted(false)
	, m_isFadeOutCompleted(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT SceneManager::init()
{
	// Now Loading の終了イベントの追加
	m_eventManager->registerEvent(
		EVENTNAME_POP_NOW_LOADING,
		[this](const std::any& /*inData*/)
		{
			GM.cs([this]() { this->m_isFadeOutCompleted = true; });
		},
		0,
		false,
		true
	);

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void SceneManager::uninit()
{
	// 全てのシーンの終了
	while (!m_scenes.empty())
	{
		popBackScene();
	}
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void SceneManager::update()
{
	if (m_scenes.empty())
	{
		return;
	}

	for (auto& scene : m_scenes)
	{
		scene->update();
	}

	auto& gm = GM;
	gm.lockCS();

	if (m_isFadeInCompleted)
	{ // フェードイン完了
		//　ロード画面以外のシーンを全て削除
		popExceptforActive();

		m_isFadeInCompleted = false;
	}

	if (m_isLoadingCompleted)
	{ // ロード完了
		// シーンを後尾に追加
		m_scenes.push_back(std::move(m_waitingScene));

		// フェードアウト開始
		auto scene = std::dynamic_pointer_cast<SceneLoad>(m_scenes.front());
		scene->setFadeState(SceneLoad::STATE::FADE_OUT);

		m_isLoadingCompleted = false;
	}

	if (m_isFadeOutCompleted)
	{ // フェードアウト完了
		// ロード画面破棄
		popFrontScene();

		m_isFadeOutCompleted = false;
	}

	gm.unlockCS();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void SceneManager::draw() const
{
	if (m_scenes.empty())
	{
		return;
	}

	// 逆順で描画する
	for (const auto& scene : std::views::reverse(m_scenes))
	{
		scene->draw();
	}
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void SceneManager::drawWithShadow() const
{
	if (m_scenes.empty())
	{
		return;
	}

	// 逆順で描画する
	for (const auto& scene : std::views::reverse(m_scenes))
	{
		scene->drawWithShadow();
	}
}

//---------------------------------------------------
// シーンの追加 (先頭に追加)
//---------------------------------------------------
void SceneManager::pushFrontScene(shared_ptr<SceneBase> inNewScene)
{
	if (!m_scenes.empty())
	{
		// 現在のシーンを一時停止
		m_scenes.front()->pause();
	}

	// 共有データの設定
	inNewScene->setSharedData(m_sharedData);

	// イベントの管理者の設定
	inNewScene->setEventManager(m_eventManager);

	// シーンの初期設定
	inNewScene->init();

	// シーンを先頭に追加
	m_scenes.push_front(inNewScene);
}

//---------------------------------------------------
// シーンの追加 (後尾に追加)
//---------------------------------------------------
void SceneManager::pushBackScene(shared_ptr<SceneBase> inNewScene)
{
	// 共有データの設定
	inNewScene->setSharedData(m_sharedData);

	// イベントの管理者の設定
	inNewScene->setEventManager(m_eventManager);

	// シーンの初期設定
	inNewScene->init();

	// シーンを後尾に追加
	m_scenes.push_back(inNewScene);

	// 後尾のシーンを一時停止
	m_scenes.back()->pause();
}

//---------------------------------------------------
// シーンの削除 (先頭の削除)
//---------------------------------------------------
void SceneManager::popFrontScene()
{
	if (!m_scenes.empty())
	{
		m_scenes.front()->uninit();
		m_scenes.pop_front();
	}

	if (!m_scenes.empty())
	{
		m_scenes.front()->resume();	// 先頭のシーンを再開
	}
}

//---------------------------------------------------
// シーンの削除 (後尾の削除)
//---------------------------------------------------
void SceneManager::popBackScene()
{
	if (!m_scenes.empty())
	{
		m_scenes.back()->uninit();
		m_scenes.pop_back();
	}
}

//---------------------------------------------------
// 現在のシーン以外を全て削除する
//---------------------------------------------------
void SceneManager::popExceptforActive()
{
	if (m_scenes.empty())
	{
		return;
	}

	size_t size = m_scenes.size();

	for (uint32_t i = 0; i < (size - 1); i++)
	{
		popBackScene();
	}
}

//---------------------------------------------------
// Now Loading
//---------------------------------------------------
void SceneManager::onNowLoading(shared_ptr<SceneBase> inNewScene)
{
	// ゲームの管理者
	auto& gm = GM;

	// フェードイン完了
	gm.cs([this]() { m_isFadeInCompleted = true; });	// ロード開始！

	// 少し待つ
	std::this_thread::sleep_for(std::chrono::milliseconds(GO_NEXT_SCENE_INTERVVAL));

	// ロード画面の裏に新しいシーンを追加
	{
		gm.lockCS();

		// 共有データの設定
		inNewScene->setSharedData(this->getSharedData());

		// イベントの管理者の設定
		inNewScene->setEventManager(this->getEventManager());

		gm.unlockCS();
	}

	// シーンの初期設定
	inNewScene->init();

	{
		gm.lockCS();

		m_waitingScene = std::move(inNewScene);
		m_isLoadingCompleted = true;	// フェードアウト開始！

		gm.unlockCS();
	}
}
