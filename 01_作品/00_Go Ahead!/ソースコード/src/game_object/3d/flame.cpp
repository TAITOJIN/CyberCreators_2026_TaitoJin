//--------------------------------------------------------------------------------
// 
// 炎 [flame.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/flame.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/calculate.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Flame::Flame()
	: GameObject3D()
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
Flame::Flame(Flame&& right) noexcept
	: GameObject3D(std::move(right))
	, m_sphere(std::move(right.m_sphere))
	, m_pairKey(std::move(right.m_pairKey))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Flame::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject3D::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_pairKey = inParam[FLAMEANDUPDRAFT_PARAM_PAIR_KEY];

		// シーン情報の取得
		auto scene = getScene<SceneBase>();

		// 中心位置
		m_sphere.pos = getPos();

		// 半径
		auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
		Vec3 scale = getScale();
		Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
		size.x *= scale.x;
		size.y *= scale.y;
		size.z *= scale.z;
		m_sphere.radius = (size.x + size.y + size.z) * 0.333f;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Flame::uninit()
{
	// 親クラスの処理
	GameObject3D::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Flame::update()
{
	// 球の当たり判定の位置の更新
	m_sphere.pos = getPos();

	auto scene = getScene();
	auto player = scene->getPlayer();
	if (player)
	{
		Collision::Sphere playerSphere;
		{
			// 中心位置
			playerSphere.pos = player->getPos();

			// 半径
			auto aabb = scene->getResManager()->getResModel(player->getKey()).getAABB();
			Vec3 scale = player->getScale();
			Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
			size.x *= scale.x;
			size.y *= scale.y;
			size.z *= scale.z;
			playerSphere.radius = (size.x + size.y + size.z) * 0.333f;
		}

		// プレイヤー「あっちっち...」
		if (Collision::isCollisionSphereVsSphere(m_sphere, playerSphere))
		{ // 衝突
#if USE_NEW_SHARED_DATA
			// プレイヤーをこんがり焼いた時間の更新
			auto& gm = GM;
			gm.lockCS();
			auto data = scene->getSharedData();
			data->m_timeFlamePlayer += scene->getDeltaTime();
			gm.unlockCS();
#endif
			// あっちっち...
			Vec3 dir = playerSphere.pos - m_sphere.pos;
			D3DXVec3Normalize(&dir, &dir);
			dir *= 100.0f;
			player->getCharacter().applyImpulse(btVector3(dir.x, 20.0, dir.z));
		}
	}

	// 親クラスの処理
	GameObject3D::update();

	// マトリックスの更新
	GameObject3D::updateMtx();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Flame::draw() const
{
	// DO_NOTHING
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void Flame::inspector()
{
	// 親クラスの処理
	GameObject3D::inspector();

	// Flame Config の設定
	if (ImGui::CollapsingHeader("Flame Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// ペアキーの設定
		{
			// 変数宣言
			static char inputBuf[256] = {};

			strncpy(&inputBuf[0], m_pairKey.c_str(), sizeof(inputBuf) - 1);
			inputBuf[sizeof(inputBuf) - 1] = '\0';

			if (ImGui::InputText("pair key", inputBuf, sizeof(inputBuf)))
			{
				auto pairs = getScene<SceneBase>()->getFlameAndUpdraftPairs();

				shared_ptr<Flame> flame;
				pairs[m_pairKey].first = flame;

				m_pairKey = inputBuf;
				pairs[m_pairKey].first = shared_from_this()->downcast<Flame>();
			}
		}

		// スケール適応ボタン
		{
			static constexpr float LOCAL_SAVE_COLOR_R = 1.f / 4.f;
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.8f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.9f, 0.9f));

			bool isRelease = ImGui::Button("APPLY SCALE##flame");
			ImGui::PopStyleColor(3);

			if (isRelease)
			{
				// シーン情報の取得
				auto scene = getScene<SceneBase>();

				// 中心位置
				m_sphere.pos = getPos();

				// 半径
				auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
				Vec3 scale = getScale();
				Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
				size.x *= scale.x;
				size.y *= scale.y;
				size.z *= scale.z;
				m_sphere.radius = (size.x + size.y + size.z) * 0.333f;
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void Flame::save(json& outObject) const
{
	// 親クラスの処理
	GameObject3D::save(outObject);

	outObject[FLAMEANDUPDRAFT_PARAM_PAIR_KEY] = m_pairKey;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
Flame& Flame::operator=(Flame&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		GameObject3D::operator=(std::move(right));

		m_sphere = std::move(right.m_sphere);
	}

	return *this;
}
