//--------------------------------------------------------------------------------
// 
// 音源ブロック [block_sound.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_sound.h"
// utility
#include "utility/collision.h"
#include "utility/random_generator.h"
// scene
#include "scene/scene_game.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float HALF_SIZE = 10.0f;	// 半分サイズ

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockSound::BlockSound()
	: GameObject3D()
	, m_scale(0.0f)
	, m_volume(0.0f)
	, m_isCollision(false)
	, m_label(CSoundManager::LABEL::BGM_BONFIRE)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockSound::BlockSound(BlockSound&& right) noexcept
	: GameObject3D(std::move(right))
	, m_scale(std::move(right.m_scale))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockSound::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject3D::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_scale = inParam[WATERDROP_EMITTER_PARAM_SCALE];
		std::string label = inParam[PARAM_SOUND_SOURCE_LABEL];
		if (label == "BONFIRE")
		{
			m_label = CSoundManager::LABEL::BGM_BONFIRE;
		}
		else if (label == "WATERFALL")
		{
			m_label = CSoundManager::LABEL::BGM_WATERFALL;
		}
		else
		{
			m_label = CSoundManager::LABEL::BGM_BONFIRE;
		}

		setScale(Vec3(m_scale, m_scale, m_scale));
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockSound::uninit()
{
	// 親クラスの処理
	GameObject3D::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockSound::update()
{
	auto scene = getScene<SceneBase>();
	//float deltaTime = scene->getDeltaTime();

	// リスナーとの当たり判定	TODO: Collision に点と球の当たり判定を追加する
	{
		Vec3 listener = scene->getPlayer()->getPos();	// リスナーの位置
		Vec3 source = getPos();							// 音源の位置
		float radius = HALF_SIZE * m_scale;				// 半径

		Vec3 length = listener - source;
		float r = (length.x * length.x) + (length.y * length.y) + (length.z * length.z);

		float mulRadius = radius * radius;
		if (r < mulRadius)
		{ // 衝突
			auto game = getScene<SceneGame>();
			if (game)
			{
				static float MAX_VOLUME = 2.0f;
				float volume = MAX_VOLUME - (MAX_VOLUME * r / mulRadius);
				game->setSoundSourceInfo(m_label, true, volume);
			}
		}
	}

	// 親クラスの処理
	GameObject3D::update();

	// マトリックスの更新
	GameObject3D::updateMtx();

	if (auto mtxParent = m_mtxParent.lock())
	{
		auto mtxWorld = getMatrix();
		D3DXMatrixMultiply(mtxWorld.get(), mtxWorld.get(), mtxParent.get());
	}
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockSound::draw() const
{
	// DO_NOTHING
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockSound::inspector()
{
	// 親クラスの処理
	GameObject3D::inspector();

	// 情報の設定
	if (ImGui::CollapsingHeader("Emitter Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 大きさの設定
		{
			float scale = m_scale;
			bool isReturn = ImGuiAddOns::Float("source scale", &scale);

			if (isReturn)
			{
				m_scale = scale;
				setScale(Vec3(scale, scale, scale));
			}
		}

		// ラベル
		{
			static std::vector<const char*> items;
			items.clear();
			items.emplace_back("BONFIRE");
			items.emplace_back("WATERFALL");

			int current = 0;
			if (m_label == CSoundManager::LABEL::BGM_WATERFALL)
			{
				current = 1;
			}

			if (ImGui::Combo("Select Label", &current, items.data(), items.size()))
			{
				m_label = CSoundManager::LABEL::BGM_BONFIRE;
				if (current == 1)
				{
					m_label = CSoundManager::LABEL::BGM_WATERFALL;
				}
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockSound::save(json& outObject) const
{
	// 親クラスの処理
	GameObject3D::save(outObject);

	outObject[WATERDROP_EMITTER_PARAM_SCALE] = m_scale;

	if (m_label == CSoundManager::LABEL::BGM_BONFIRE)
	{
		outObject[PARAM_SOUND_SOURCE_LABEL] = "BONFIRE";
	}
	else if (m_label == CSoundManager::LABEL::BGM_WATERFALL)
	{
		outObject[PARAM_SOUND_SOURCE_LABEL] = "WATERFALL";
	}
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockSound& BlockSound::operator=(BlockSound&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		GameObject3D::operator=(std::move(right));

		m_scale = std::move(right.m_scale);
	}

	return *this;
}
