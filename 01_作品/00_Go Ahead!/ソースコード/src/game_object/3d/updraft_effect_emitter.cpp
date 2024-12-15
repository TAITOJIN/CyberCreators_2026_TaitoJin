//--------------------------------------------------------------------------------
// 
// 上昇気流のエフェクトを生み出す [updraft_effect_emitter.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/updraft_effect_emitter.h"
// utility
#include "utility/random_generator.h"
// scene
#include "scene/scene_base.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr const char* WATER_DROP_PARAM = R"({
				"TAG": "updraft_effect_emitter",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b0001",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "UPDRAFT_EFFECT",
						"MAT_NAME": 0
					}
				],
				"OFFSET": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"POS": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"ROT": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				},
				"DROP_GRAVITY": 10.0
			})";
	constexpr float ADJUST_MILLSECOND = 0.01f;	// ミリ秒に補正

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
UpdraftEffectEmitter::UpdraftEffectEmitter()
	: GameObject3D()
	, m_setInterval(0.0f)
	, m_counterInterval(0.0f)
	, m_createPos(DEF_VEC3)
	, m_scale(0.0f)
	, m_msMinLife(0.0f)
	, m_msMaxLife(0.0f)
	, m_gravity(0.0f)
	, m_isEnabled(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
UpdraftEffectEmitter::UpdraftEffectEmitter(UpdraftEffectEmitter&& right) noexcept
	: GameObject3D(std::move(right))
	, m_setInterval(std::move(right.m_setInterval))
	, m_counterInterval(std::move(right.m_counterInterval))
	, m_createPos(std::move(right.m_createPos))
	, m_scale(std::move(right.m_scale))
	, m_msMinLife(std::move(right.m_msMinLife))
	, m_msMaxLife(std::move(right.m_msMaxLife))
	, m_gravity(std::move(right.m_gravity))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT UpdraftEffectEmitter::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject3D::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	m_setInterval = inParam[WATERDROP_EMITTER_PARAM_SETINTERVAL];
	m_createPos = inParam[WATERDROP_EMITTER_PARAM_POS];
	m_scale = inParam[WATERDROP_EMITTER_PARAM_SCALE];
	m_msMinLife = inParam[WATERDROP_EMITTER_PARAM_MS_MIN_LIFE];
	m_msMaxLife = inParam[WATERDROP_EMITTER_PARAM_MS_MAX_LIFE];
	m_gravity = inParam[WATERDROP_EMITTER_PARAM_GRAVITY];
	m_pairKey = inParam[FLAMEANDUPDRAFT_PARAM_PAIR_KEY];

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void UpdraftEffectEmitter::uninit()
{
	// 親クラスの処理
	GameObject3D::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void UpdraftEffectEmitter::update()
{
	if (m_isEnabled)
	{
		auto scene = getScene<SceneBase>();
		float deltaTime = scene->getDeltaTime();

		m_counterInterval += deltaTime;
		if (m_counterInterval >= m_setInterval)
		{
			// リセット
			m_counterInterval = 0.0f;

			// 生み出す位置の更新
			Matrix mtxWorld = *getMatrix();
			m_createPos = Vec3(mtxWorld._41, mtxWorld._42, mtxWorld._43);

			// パラメータの設定
			json j = json::parse(WATER_DROP_PARAM);
			j[FLAG_POS] = m_createPos;
			j[WATERDROP_PARAM_FIRST_SCALE] = m_scale;
			j[WATERDROP_PARAM_MAX_LIFE] = RandomGenerator::get<float>(m_msMinLife, m_msMaxLife) * ADJUST_MILLSECOND;
			j[WATERDROP_PARAM_GRAVITY] = m_gravity;

			// オブジェクトの追加
			scene->addObject(FACTORY_WATERDROP_BLOCK, j);
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
void UpdraftEffectEmitter::draw() const
{
	// DO_NOTHING
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void UpdraftEffectEmitter::inspector()
{
	// 親クラスの処理
	GameObject3D::inspector();

	// 情報の設定
	if (ImGui::CollapsingHeader("Emitter Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// ペアキーの設定
		{
			// 変数宣言
			static char inputBuf[256] = {};

			strncpy(&inputBuf[0], m_pairKey.c_str(), sizeof(inputBuf) - 1);
			inputBuf[sizeof(inputBuf) - 1] = '\0';

			if (ImGui::InputText("pair key", inputBuf, sizeof(inputBuf)))
			{
				// もともとの所属から削除
				{
					auto emitters = getScene<SceneBase>()->getUpdraftEffectEmitters()[m_pairKey];

					auto it = emitters.begin();
					while (it != emitters.end())
					{
						if (auto emitter = it->lock())
						{
							if (emitter == shared_from_this())
							{ // 自分自身
								it = emitters.erase(it);
								break;
							}
							else
							{
								++it;
							}
						}
						else
						{
							it = emitters.erase(it);
						}
					}
				}

				// ペアキーを更新して新しく追加
				m_pairKey = inputBuf;
				auto emitters = getScene<SceneBase>()->getUpdraftEffectEmitters()[m_pairKey];
				emitters.push_back(shared_from_this()->downcast<UpdraftEffectEmitter>());
			}
		}

		// 間隔の設定
		{
			float interval = m_setInterval;
			bool isReturn = ImGuiAddOns::Float("Interval", &interval);

			if (isReturn)
			{
				m_setInterval = interval;
			}
		}

		// 位置の設定
		{
			Vec3 pos = m_createPos;
			bool isReturn = ImGuiAddOns::Transform("drop pos", &pos);

			if (isReturn)
			{
				m_createPos = pos;
			}
		}

		// 大きさの設定
		{
			float scale = m_scale;
			bool isReturn = ImGuiAddOns::Float("drop scale", &scale);

			if (isReturn)
			{
				m_scale = scale;
				setScale(Vec3(scale, scale, scale));
			}
		}

		// 重力
		{
			float gravity = m_gravity;
			bool isReturn = ImGuiAddOns::Float("drop gravity", &gravity);

			if (isReturn)
			{
				m_gravity = gravity;
			}
		}

		// 最短寿命
		{
			float life = m_msMinLife;
			bool isReturn = ImGuiAddOns::Float("ms min life", &life);

			if (isReturn)
			{
				m_msMinLife = life;
			}
		}

		// 最長寿命
		{
			float life = m_msMaxLife;
			bool isReturn = ImGuiAddOns::Float("ms max life", &life);

			if (isReturn)
			{
				m_msMaxLife = life;
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void UpdraftEffectEmitter::save(json& outObject) const
{
	// 親クラスの処理
	GameObject3D::save(outObject);

	outObject[WATERDROP_EMITTER_PARAM_SETINTERVAL] = m_setInterval;
	outObject[WATERDROP_EMITTER_PARAM_POS] = m_createPos;
	outObject[WATERDROP_EMITTER_PARAM_SCALE] = m_scale;
	outObject[WATERDROP_EMITTER_PARAM_MS_MIN_LIFE] = m_msMinLife;
	outObject[WATERDROP_EMITTER_PARAM_MS_MAX_LIFE] = m_msMaxLife;
	outObject[WATERDROP_EMITTER_PARAM_GRAVITY] = m_gravity;
	outObject[FLAMEANDUPDRAFT_PARAM_PAIR_KEY] = m_pairKey;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
UpdraftEffectEmitter& UpdraftEffectEmitter::operator=(UpdraftEffectEmitter&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		GameObject3D::operator=(std::move(right));

		m_setInterval = std::move(right.m_setInterval);
		m_counterInterval = std::move(right.m_counterInterval);
		m_createPos = std::move(right.m_createPos);
		m_scale = std::move(right.m_scale);
		m_msMinLife = std::move(right.m_msMinLife);
		m_msMaxLife = std::move(right.m_msMaxLife);
		m_gravity = std::move(right.m_gravity);
	}

	return *this;
}
