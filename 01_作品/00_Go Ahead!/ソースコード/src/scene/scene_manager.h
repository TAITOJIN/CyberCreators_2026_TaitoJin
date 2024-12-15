//--------------------------------------------------------------------------------
// 
// シーンの管理者 [scene_manager.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_base.h"
#include "scene/scene_load.h"

//===================================================
// シーンの管理者のクラス
//===================================================
class SceneManager
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	SceneManager();
	~SceneManager() = default;

	HRESULT init();		// 初期設定
	void uninit();		// 終了処理
	void update();		// 更新処理
	void draw() const;	// 描画処理
	void drawWithShadow() const;	// 描画処理

	void pushFrontScene(shared_ptr<SceneBase> inNewScene);	// シーンの追加 (先頭に追加)
	void pushBackScene(shared_ptr<SceneBase> inNewScene);	// シーンの追加 (後尾に追加)
	void popFrontScene();									// シーンの削除 (先頭を削除)
	void popBackScene();									// シーンの削除 (後尾を削除)
	void popExceptforActive();								// 現在のシーン以外を全て削除する

	template <typename T> void changeScene();				// シーンの切替
	void onNowLoading(shared_ptr<SceneBase> inNewScene);	// Now Loading

	shared_ptr<SharedData> getSharedData() const { return m_sharedData; }		// 共有データの取得
	shared_ptr<EventManager> getEventManager() const { return m_eventManager; }	// イベントの管理者の取得
	size_t getSceneNum() const { return m_scenes.size(); }						// シーンの数の取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	SceneManager(const SceneManager&) = delete;
	void operator=(const SceneManager&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::list<shared_ptr<SceneBase>> m_scenes;	// シーンのリスト
	shared_ptr<SharedData> m_sharedData;		// 共有データ
	shared_ptr<EventManager> m_eventManager;	// イベントの管理者
	bool m_isFadeInCompleted;					// フェードイン完了 (ロード開始合図)
	bool m_isLoadingCompleted;					// ロード完了 (フェードアウト開始合図)
	bool m_isFadeOutCompleted;					// フェードアウト完了
	shared_ptr<SceneBase> m_waitingScene;		// 待機シーン
};

//---------------------------------------------------
// シーンの切替
//---------------------------------------------------
template<typename T>
inline void SceneManager::changeScene()
{
	static_assert(std::is_base_of<SceneBase, T>::value, "T は SceneBase の派生クラスであること！");

	// イベント名の設定 (NEXT_SCENE_EVENT_0 みたいな形式)
	std::stringstream nextSceneEventName;
	{
		static uint32_t changeCnt = 0;
		nextSceneEventName << EVENTNAME_NEXT_SCENE_EVENT << changeCnt;
	}

	// ローディング画面の追加
	pushFrontScene(make_shared<SceneLoad>());
	if (auto sceneLoad = std::dynamic_pointer_cast<SceneLoad>(m_scenes.front()))
	{
		sceneLoad->setEventName(nextSceneEventName.str());
	}

	// 次のシーンの設定イベントの追加
	m_eventManager->registerEvent(
		nextSceneEventName.str(),
		[this](const std::any& /**/) { onNowLoading(make_shared<T>()); },
		0,
		true,
		true
	);
}
