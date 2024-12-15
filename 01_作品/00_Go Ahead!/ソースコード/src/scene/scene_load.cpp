//--------------------------------------------------------------------------------
// 
// Now Loading のシーン [scene_load.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_load.h"
// game_manager
#include "game_manager/game_manager.h"
// game_object
// // 2d
#include "game_object/2d/game_object_2d.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float FADE_SPEED = 0.05f;	// フェードの測度

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
SceneLoad::SceneLoad()
	: SceneBase()
	, m_fadeState(STATE::NONE)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT SceneLoad::init()
{
	// 親クラスの処理
	HRESULT hr = SceneBase::init();
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// サイズの変更
	resizeObjects(static_cast<uint32_t>(PRIORITY::MAX));

	// 初期パラメータの設定
	{
		// フェードインスタート
		m_fadeState = STATE::FADE_IN;
	}

	// リソースの読み込み
	getResManager()->loadTextures("res\\list\\texture_list_load.json");

	// オブジェクトの配置
	loadObjects("res\\list\\load_set.json");

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void SceneLoad::uninit()
{
	// 親クラスの処理
	SceneBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void SceneLoad::update()
{
	// 親クラスの処理
	SceneBase::update();

	switch (m_fadeState)
	{
	case STATE::NONE:
		return;
		break;
	case STATE::FADE_IN:
	{
		bool isCompleted = false;	// フェードイン完了

		for (auto& object : getObjects(static_cast<uint32_t>(PRIORITY::FADE)))
		{
			auto object2d = std::dynamic_pointer_cast<GameObject2D>(object);
			if (!object2d)
			{
				continue;
			}

			// 不透明にしていく
			Color col = object2d->getCol();
			col.a += FADE_SPEED;
			object2d->setCol(col);

			// 不透明になりきったか否か
			if (object2d->getCol().a >= 1.0f)
			{ // 不透明
				col.a = 1.0f;
				object2d->setCol(col);		// 透明度を保つ
				m_fadeState = STATE::NONE;	// 何もしていない状態にする
				isCompleted = true;			// フェードイン完了
			}
		}

		if (isCompleted)
		{ // フェードイン完了
			// ロード開始
			getEventmanager()->notifyEvent(m_eventName, std::any{});
		}
	} break;
	case STATE::FADE_OUT:
	{
		bool isCompleted = false;	// フェードアウト完了

		for (auto& object : getObjects(static_cast<uint32_t>(PRIORITY::FADE)))
		{
			auto object2d = std::dynamic_pointer_cast<GameObject2D>(object);
			if (!object2d)
			{
				continue;
			}

			// 透明にしていく
			Color col = object2d->getCol();
			col.a -= FADE_SPEED;
			object2d->setCol(col);

			// 透明になりきったか否か
			if (object2d->getCol().a <= 0.0f)
			{ // 透明
				col.a = 0.0f;
				object2d->setCol(col);			// 透明度を保つ
				object2d->setIsDestroy(true);	// 破棄
				m_fadeState = STATE::NONE;		// 何もしていない状態にする
				isCompleted = true;				// フェードアウト完了
			}
		}

		if (isCompleted)
		{ // フェードアウト完了
			// ロード画面終了
			getEventmanager()->notifyEvent(EVENTNAME_POP_NOW_LOADING, std::any{});
		}
	} break;
	default:
		assert(false);
		break;
	}
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void SceneLoad::draw() const
{
	// 親クラスの処理
	SceneBase::draw();
}
