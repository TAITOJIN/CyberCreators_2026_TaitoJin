//--------------------------------------------------------------------------------
// 
// 上昇気流 [updraft.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/updraft.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/calculate.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Updraft::Updraft()
	: GameObject3D()
	, m_rect()
	, m_posY(0.0f)
	, m_height(0.0f)
	, m_isEnabled(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
Updraft::Updraft(Updraft&& right) noexcept
	: GameObject3D(std::move(right))
	, m_pairKey(std::move(right.m_pairKey))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Updraft::init(const json& inParam)
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
		Vec3 pos = getPos();
		m_rect.center = Vec2(pos.x, pos.z);
		m_posY = pos.y;

		// 半径
		auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
		Vec3 scale = getScale();
		Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
		size.x *= scale.x;
		size.y *= scale.y;
		size.z *= scale.z;
		m_rect.halfExtend = Vec2(size.x, size.z);
		m_height = size.y;

		// 向き
		Quaternion rot = getRot();
		Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
		m_rect.rot = rot.y;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Updraft::uninit()
{
	// 親クラスの処理
	GameObject3D::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Updraft::update()
{
	// 中心位置
	Vec3 pos = getPos();
	m_rect.center = Vec2(pos.x, pos.z);
	m_posY = pos.y;

	// 向き
	Quaternion rot = getRot();
	Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
	m_rect.rot = rot.y;

	// 親クラスの処理
	GameObject3D::update();

	// マトリックスの更新
	GameObject3D::updateMtx();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Updraft::draw() const
{
	// DO_NOTHING
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void Updraft::inspector()
{
	// 親クラスの処理
	GameObject3D::inspector();

	// Updraft Config の設定
	if (ImGui::CollapsingHeader("Updraft Config", ImGuiTreeNodeFlags_DefaultOpen))
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

				shared_ptr<Updraft> updraft;
				pairs[m_pairKey].second = updraft;

				m_pairKey = inputBuf;
				pairs[m_pairKey].second = shared_from_this()->downcast<Updraft>();
			}
		}

		// スケール適応ボタン
		{
			static constexpr float LOCAL_SAVE_COLOR_R = 1.f / 4.f;
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.8f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.9f, 0.9f));

			bool isRelease = ImGui::Button("APPLY SCALE##updraft");
			ImGui::PopStyleColor(3);

			if (isRelease)
			{
				// シーン情報の取得
				auto scene = getScene<SceneBase>();

				// 中心位置
				Vec3 pos = getPos();
				m_rect.center = Vec2(pos.x, pos.z);

				// 半径
				auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
				Vec3 scale = getScale();
				Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
				size.x *= scale.x;
				size.y *= scale.y;
				size.z *= scale.z;
				m_rect.halfExtend = Vec2(size.x, size.z);

				// 向き
				Quaternion rot = getRot();
				Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
				m_rect.rot = rot.y;
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void Updraft::save(json& outObject) const
{
	// 親クラスの処理
	GameObject3D::save(outObject);

	outObject[FLAMEANDUPDRAFT_PARAM_PAIR_KEY] = m_pairKey;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
Updraft& Updraft::operator=(Updraft&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		GameObject3D::operator=(std::move(right));

		m_pairKey = std::move(right.m_pairKey);
	}

	return *this;
}
