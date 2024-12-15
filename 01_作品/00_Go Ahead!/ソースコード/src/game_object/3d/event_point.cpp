//--------------------------------------------------------------------------------
// 
// イベント地点 [event_point.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/event_point.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"
// scene
#include "scene/scene_debug.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float RANGE_HALF_HEIGHT = 50.0f;	// スポーン地点の上下 50.0f 以内

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
EventPoint::EventPoint()
	: Polygon3D()
	, m_done(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
EventPoint::EventPoint(EventPoint&& right) noexcept
	: Polygon3D(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT EventPoint::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = Polygon3D::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_eventName = inParam[PARAM_EVENT_NAME];
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void EventPoint::uninit()
{
	// 親クラスの処理
	Polygon3D::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void EventPoint::update()
{
	// スポーン地点の矩形の設定
	Collision::Rectangle spawnPointRect;
	float spawnPointY = 0.0f;
	{
		// 中心点
		Vec3 pos = getPos();
		spawnPointRect.center = Vec2(pos.x, pos.z);
		spawnPointY = pos.y;

		// 半径
		spawnPointRect.halfExtend = getSize() * HALF;

		// 向き
		Quaternion rot = getRot();
		float y = Calculate::quaternionToEulerYXZ(rot).y;
		spawnPointRect.rot = y;
	}

	// プレイヤーの位置取得
	auto player = getScene<SceneBase>()->getPlayer();
	Vec3 playerPos = player->getPos();
	Point2D playerPoint = Vec2(playerPos.x, playerPos.z);

	// MEMO: プレイヤーの原点は中心なので、実際には上の RANGE_HALF_HEIGHT の範囲が狭く、下の範囲が広い。
	// 「モデルサイズの取得等オーバーヘッドや計算コスト、上下の正確性がそこまで必要ない」ことからこのままで行う。
	// プレイヤーのサイズが RANGE_HALF_HEIGHT * 2 以上の場合は RANGE_HALF_HEIGHT か計算方法を変えること。

	// 当たり判定
	bool isCollison = false;
	if (playerPos.y >= spawnPointY - RANGE_HALF_HEIGHT &&
		playerPos.y <= spawnPointY + RANGE_HALF_HEIGHT)
	{ // y
		if (Collision::isCollisionRectVsPoint(spawnPointRect, playerPoint))
		{ // x, z
			if (!m_done)
			{
				auto& gm = GM;
				gm.lockCS();
				getScene()->getEventmanager()->notifyEvent(m_eventName, getScene());
				gm.unlockCS();
			}
			m_done = true;
			isCollison = true;
		}
	}

	if (!isCollison)
	{
		m_done = false;
	}

	// 親クラスの処理
	Polygon3D::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void EventPoint::draw() const
{
	if (!getScene<SceneDebug>())
	{
		return;
	}

	// ワイヤーフレームを表示
	Renderer::onWireFrame();

	// 親クラスの処理
	Polygon3D::draw();

	// ワイヤーフレームを非表示
	Renderer::offWireFrame();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void EventPoint::inspector()
{
	// 親クラスの処理
	Polygon3D::inspector();

	// タグの設定
	{
		// 変数宣言
		static char inputBuf[256] = {};

		strncpy(&inputBuf[0], m_eventName.c_str(), sizeof(inputBuf) - 1);
		inputBuf[sizeof(inputBuf) - 1] = '\0';

		if (ImGui::InputText("event name", inputBuf, sizeof(inputBuf)))
		{
			m_eventName = inputBuf;
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void EventPoint::save(json& outObject) const
{
	// 親クラスの処理
	Polygon3D::save(outObject);

	outObject[PARAM_EVENT_NAME] = m_eventName;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
EventPoint& EventPoint::operator=(EventPoint&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		Polygon3D::operator=(std::move(right));
	}

	return *this;
}
