//--------------------------------------------------------------------------------
// 
// 桜を生み出す [spring_emitter_2d.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/spring_emitter_2d.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/random_generator.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	const std::string SNOW_PARAM = R"({
			"TAG": "cherry_blossom_2d_0",
			"PRIORITY": 13,
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
			"SIZE": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"COL": {
				"r": 1.0,
				"g": 0.7,
				"b": 1.0,
				"a": 1.0
			},
			"TEXKEY": "res\\texture\\none.png",
			"ISTEXANIM": false,
			"SNOW_PARAM_MOVE": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			}
		})";

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
SpringEmitter2D::SpringEmitter2D()
	: GameObject()
	, m_isEnabled(false)
	, m_timeCounter(0)
	, m_interval(0)
	, m_intervalMin(0.0f)
	, m_intervalMax(1.0f)
	, m_numMin(0)
	, m_numMax(1)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT SpringEmitter2D::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject::init(inParam);
	if (FAILED(hr))
	{
		return hr;
	}

	// 初期パラメータの設定
	{
		m_intervalMin = inParam[PARAM_SNOW_GENERATOR_INTERVAL_MIN];
		m_intervalMax = inParam[PARAM_SNOW_GENERATOR_INTERVAL_MAX];
		m_numMin = inParam[PARAM_SNOW_GENERATOR_NUM_MIN];
		m_numMax = inParam[PARAM_SNOW_GENERATOR_NUM_MAX];
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void SpringEmitter2D::uninit()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void SpringEmitter2D::update()
{
	if (m_isEnabled)
	{
		m_timeCounter++;	// カウンターの加算
		if (m_timeCounter >= m_interval)
		{ // インターバル終了
			auto scene = getScene();

			m_timeCounter = 0;	// カウンターリセット

			// 次のインターバルを決める
			m_interval = RandomGenerator::get(m_intervalMin, m_intervalMax);

			// 雪を生成
			int num = RandomGenerator::get<int>(m_numMin, m_numMax);
			for (int i = 0; i < num; i++)
			{
				// 移動量
				Vec3 move = DEF_VEC3;
				{
					// x
					move.x = RandomGenerator::get(0.0f, 2.0f);
					if (RandomGenerator::get(0, 1))
					{
						move.x *= -1.0f;
					}

					// y
					move.y = RandomGenerator::get(3.0f, 10.0f);
				}

				// 位置
				Vec3 pos = DEF_VEC3;
				{
					pos.x = RandomGenerator::get(0.0f, static_cast<float>(SCREEN_WIDTH));
					pos.y = RandomGenerator::get(0.0f, static_cast<float>(100.0f)) * -1.0f;
				}

				// サイズ
				Vec3 size = DEF_VEC3;
				{
					size.x = RandomGenerator::get(3.0f, static_cast<float>(5.0f));
					size.y = size.x;
				}

				// パラメータ
				json param = json::parse(SNOW_PARAM);
				param[PARAM_SNOW_MOVE] = move;
				param[OBJECT2D_PARAM_POS] = pos;
				param[OBJECT2D_PARAM_SIZE] = size;

				// シーンに追加
				scene->addObject(FACTORY_SNOW_2D, param);
			}
		}
	}
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void SpringEmitter2D::draw() const
{
	// DO_NOTHING
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void SpringEmitter2D::inspector()
{
	// 親クラスの処理
	GameObject::inspector();

	// Config の設定
	if (ImGui::CollapsingHeader("Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 間隔の最小値
		{
			float min = m_intervalMin;
			bool isReturn = ImGuiAddOns::Float("interval min", &min);
			if (isReturn)
			{
				m_intervalMin = min;
			}
		}

		// 間隔の最大値
		{
			float max = m_intervalMax;
			bool isReturn = ImGuiAddOns::Float("interval max", &max);
			if (isReturn)
			{
				m_intervalMax = max;
			}
		}

		// 数の最小値
		{
			int min = m_numMin;
			bool isReturn = ImGuiAddOns::Int("num min", &min);
			if (isReturn)
			{
				m_numMin = min;
			}
		}

		// 数の最大値
		{
			int max = m_numMax;
			bool isReturn = ImGuiAddOns::Int("num max", &max);
			if (isReturn)
			{
				m_numMax = max;
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void SpringEmitter2D::save(json& outObject) const
{
	// 親クラスの処理
	GameObject::save(outObject);

	outObject[PARAM_SNOW_GENERATOR_INTERVAL_MIN] = m_intervalMin;
	outObject[PARAM_SNOW_GENERATOR_INTERVAL_MAX] = m_intervalMax;
	outObject[PARAM_SNOW_GENERATOR_NUM_MIN]= m_numMin;
	outObject[PARAM_SNOW_GENERATOR_NUM_MAX] = m_numMax;
}
