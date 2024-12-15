//--------------------------------------------------------------------------------
// 
// スポーン地点 [spawn_point.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/spawn_point.h"
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
	constexpr float RANGE_HALF_HEIGHT = 50.0f;					// スポーン地点の上下 50.0f 以内
	constexpr const char* SPAWN_POINT_2_NAME = "spawn_point_2";	// スポーン地点の名前
	constexpr float DEST_DISTANCE = 600.0f;						// 目的の距離

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
SpawnPoint::SpawnPoint()
	: Polygon3D()
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
SpawnPoint::SpawnPoint(SpawnPoint&& right) noexcept
	: Polygon3D(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT SpawnPoint::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = Polygon3D::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_respawnPoint = inParam[RESPAWNPOINT_PARAM_RESPAWNPOINT];
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void SpawnPoint::uninit()
{
	// 親クラスの処理
	Polygon3D::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void SpawnPoint::update()
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
			// リスポーン地点の設定
			player->setRespawnPos(m_respawnPoint);
#if 0
			if (getTag() == SPAWN_POINT_2_NAME)
			{
				getScene()->getCamera()->setDestDistance(DEST_DISTANCE);
			}
#endif
			isCollison = true;
		}
	}
#if 0
	if (!isCollison)
	{
		if (getTag() == SPAWN_POINT_2_NAME)
		{
			auto camera = getScene()->getCamera();
			camera->setDestDistance(camera->getDestDistanceKeep());
		}
	}
#endif

	// 親クラスの処理
	Polygon3D::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void SpawnPoint::draw() const
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
void SpawnPoint::inspector()
{
	// 親クラスの処理
	Polygon3D::inspector();

	// 情報の設定
	if (ImGui::CollapsingHeader("Respawn Point Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// リスポーン地点の設定
		{
			Vec3 pos = m_respawnPoint;
			bool isReturn = ImGuiAddOns::Transform("Respawn Point", &pos);

			if (isReturn)
			{
				// 反映
				m_respawnPoint = pos;
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void SpawnPoint::save(json& outObject) const
{
	// 親クラスの処理
	Polygon3D::save(outObject);

	outObject[RESPAWNPOINT_PARAM_RESPAWNPOINT] = m_respawnPoint;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
SpawnPoint& SpawnPoint::operator=(SpawnPoint&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		Polygon3D::operator=(std::move(right));
	}

	return *this;
}
