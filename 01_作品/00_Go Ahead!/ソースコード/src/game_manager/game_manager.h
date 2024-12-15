//--------------------------------------------------------------------------------
// 
// ゲームのマネージャー (管理者) [game_manager.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// renderer
#include "renderer/renderer.h"
// scene
#include "scene/scene_manager.h"
// system
#include "system/critical_section.h"
// input
#include "input/input_manager.h"

//---------------------------------------------------
// マクロ定義
//---------------------------------------------------
#ifdef GM
#undef GM
#endif
#define GM (GameManager::getInstance())	// ゲームのインスタンス

//===================================================
// ゲームのマネージャーのクラス (Singleton)
//===================================================
class GameManager
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	HRESULT init(const HINSTANCE& inInstanceHandle, const HWND& inWindowHandle, const BOOL& isWindowed);	// 初期設定
	void uninit();		// 終了処理
	void update();		// 更新処理
	void draw() const;	// 描画処理

	const HWND& getWindowHandle() const { return m_windowHandle; }				// ウィンドウハンドルの取得
	const HINSTANCE& getInstanceHandle() const { return m_instanceHandle; }		// インスタンスハンドルの取得
	Renderer& getRenderer() { return *m_renderer; };							// レンダラーの取得
	InputManager& getInputManager() { return *m_inputManager; };				// 入力処理の管理者の取得
	SceneManager& getSceneManager() const { return *m_sceneManager; }			// シーンの管理者の取得
	Device getDevice() const { return m_renderer->getDevice(); }				// デバイスの取得

	void cs(const std::function<void()>& inFunc);	// クリティカルセクション
	void lockCS() { m_cs->lock(); }					// クリティカルセクションに入る
	void unlockCS() { m_cs->unlock(); }				// クリティカルセクションを離れる

	static GameManager& getInstance() { static GameManager instance; return instance; }	// インスタンスの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	GameManager();
	~GameManager() = default;
	GameManager(const GameManager&) = delete;
	void operator=(const GameManager&) = delete;

	void registerFactoryMethod();	// ゲームオブジェクトの生成メソッドの登録

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	HWND m_windowHandle;						// ウィンドウハンドル
	HINSTANCE m_instanceHandle;					// インスタンスハンドル
	unique_ptr<CriticalSection> m_cs;			// クリティカルセクション
	unique_ptr<InputManager> m_inputManager;	// 入力処理の管理者
	unique_ptr<Renderer> m_renderer;			// レンダラー
	unique_ptr<SceneManager> m_sceneManager;	// シーンの管理者
};
