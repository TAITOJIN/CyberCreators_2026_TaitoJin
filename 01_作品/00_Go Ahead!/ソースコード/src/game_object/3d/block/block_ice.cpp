//--------------------------------------------------------------------------------
// 
// 水ブロック [block_water.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_ice.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/collision.h"
#include "utility/calculate.h"
#include "utility/random_generator.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float ATN_CHANGE_COL = 0.1f;			// 色変化の減衰率
	constexpr float DONE_CHANGE_JUDGEMENT = 0.05f;	// この数値の差で変更完了判定にする

	constexpr float ADJUST_MILLSECOND = 0.01f;			// ミリ秒に調整

	constexpr int NUM_PIECES = 5;						// 粉々の数
	constexpr float PIECES_RAND_SCALE_MIN = 0.1f;		// スケール (最小値)
	constexpr float PIECES_RAND_SCALE_MAX = 0.25f;		// スケール (最大値)
	constexpr float PIECES_RAND_LIFE_MIN = 40.0f;		// 寿命 (最小値)
	constexpr float PIECES_RAND_LIFE_MAX = 60.0f;		// 寿命 (最大値)
	constexpr float PIECES_ADJUST_MILLSECOND = 0.01f;	// ミリ秒に補正
	constexpr float PIECES_RAND_MOVE_MIN = -0.6f;		// 移動量 (最小値)
	constexpr float PIECES_RAND_MOVE_MAX = 0.6f;		// 移動量 (最大値)

	constexpr const char* PARAM_SMALL_PIECES = R"({
				"TAG": "pieces_0",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b0001",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "WATER_BLOCK",
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
				"MOVE": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				}
			})";

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockIce::BlockIce()
	: BlockDummy()
	, m_mat(DEF_MAT)
	, m_iceState(ICE_STATE::ICE)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockIce::BlockIce(BlockIce&& right) noexcept
	: BlockDummy(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockIce::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockDummy::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_iceMatKey = inParam[ICE_BLOCK_PARAM_ICE_MAT_KEY];
		m_waterMatKey = inParam[ICE_BLOCK_PARAM_WATER_MAT_KEY];
		m_pairKey = inParam[FLAMEANDUPDRAFT_PARAM_PAIR_KEY];

		auto rm = getScene<SceneBase>()->getResManager();
		m_mat = rm->getResMaterial(m_iceMatKey).getMaterial();
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockIce::uninit()
{
	// 親クラスの処理
	BlockDummy::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockIce::update()
{
	// シーン情報の取得
	auto scene = getScene<SceneBase>();

	// TODO: STATE パターン
	switch (m_iceState)
	{
	case BlockIce::ICE_STATE::ICE:
	{ // 氷状態
		// NOTHING
	} break;
	case BlockIce::ICE_STATE::ICE_TO_WATER:
	{ // 氷から水に変化中
		// マテリアルの取得
		auto rm = scene->getResManager();
		GraphicsMaterial mat = rm->getResMaterial(m_waterMatKey).getMaterial();

		// 目的の色の値
		Color destDiffuse = mat.Diffuse;
		Color destEmissive = mat.Emissive;
		Color diffuse = m_mat.Diffuse;
		Color emissive = m_mat.Emissive;
		diffuse += (destDiffuse - diffuse) * ATN_CHANGE_COL;
		emissive += (destEmissive - emissive) * ATN_CHANGE_COL;
		m_mat.Diffuse = diffuse;
		m_mat.Emissive = emissive;

		Color diffDiffuse = destDiffuse - diffuse;
		Color diffEmissive = destEmissive - emissive;

		if (fabsf(diffDiffuse.r) <= DONE_CHANGE_JUDGEMENT &&
			fabsf(diffDiffuse.g) <= DONE_CHANGE_JUDGEMENT &&
			fabsf(diffDiffuse.b) <= DONE_CHANGE_JUDGEMENT &&
			fabsf(diffDiffuse.a) <= DONE_CHANGE_JUDGEMENT &&
			fabsf(diffEmissive.r) <= DONE_CHANGE_JUDGEMENT &&
			fabsf(diffEmissive.g) <= DONE_CHANGE_JUDGEMENT &&
			fabsf(diffEmissive.b) <= DONE_CHANGE_JUDGEMENT &&
			fabsf(diffEmissive.a) <= DONE_CHANGE_JUDGEMENT)
		{ // 変化終了
			getDummyFlags().setFlag(DUMMY_FLAGS::NO_COLLISION);
			m_iceState = ICE_STATE::WATER;
		}
	} break;
	case BlockIce::ICE_STATE::WATER:
	{ // 水状態
		// 水の当たり判定情報
		Collision::Rectangle waterRect;	// 水の矩形情報
		float waterY = 0.0f;			// 高さの原点
		float waterHeight = 0.0f;		// 高さ
		{
			// 中心位置
			Vec3 pos = getPos();
			waterRect.center = Vec2(pos.x, pos.z);

			// 半径
			auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
			Vec3 scale = getScale();
			Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
			size.x *= scale.x;
			size.y *= scale.y;
			size.z *= scale.z;
			waterRect.halfExtend = Vec2(size.x, size.z);

			// 回転値
			Quaternion rot = getRot();
			Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
			waterRect.rot = euler.y;

			// 高さ (位置)
			waterY = pos.y;

			// 高さ (大きさ)
			waterHeight = size.y;
		}

		bool isTouch = false;	// 触れているか否か
		bool isUnder = false;	// 水面下

		// プレイヤーとの当たり判定
		{
			// プレイヤーの当たり判定情報
			Collision::Rectangle playerRect;	// プレイヤーの矩形の情報
			auto player = scene->getPlayer();	// プレイヤー
			float playerY = 0.0f;				// 高さの原点
			float playerHeight = 0.0f;			// 高さ
			{
				// 中心位置
				Vec3 pos = player->getPos();
				playerRect.center = Vec2(pos.x, pos.z);

				// 半径
				auto aabb = scene->getResManager()->getResModel(player->getKey()).getAABB();
				Vec3 scale = player->getScale();
				Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
				size.x *= scale.x;
				size.y *= scale.y;
				size.z *= scale.z;
				playerRect.halfExtend = Vec2(size.x, size.z);

				// 回転値
				Quaternion rot = player->getRot();
				Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
				playerRect.rot = euler.y;

				// 高さ (位置)
				playerY = pos.y;

				// 高さ (大きさ)
				playerHeight = size.y * HALF;
			}
			bool wasTouch = player->getWasTouchWater();
			//bool wasUnder = player->getWasUnderWater();

			// TODO: 中心点じゃなくて原点渡して高さで調整しているが、本来は原点ではなく原点を高さで補正して中心点を渡すべき

			// プレイヤーと水との当たり判定 (触れているかどうか)
			if (Collision::isCollisionBlockVsBlock(waterRect, playerRect, waterY, playerY, waterHeight, playerHeight))
			{
				isTouch = true;
				player->setIsTouchWater(true);
			}

			// MEMO: こっちは中心点に調整済み？

			// プレイヤーが水にがっつり浸っているか (高さを少し余裕を持たせる)
			if (Collision::isCollisionBlockVsBlock(waterRect, playerRect, waterY, playerY + playerHeight, waterHeight, playerHeight - 1.0f))
			{
				isUnder = true;
				player->setIsUnderWater(true);
			}

			if (!wasTouch && isTouch)
			{ // 着水
				// 水しぶき
				splashOfWater();
			}
		}

		// 押せるブロックとの当たり判定
		for (auto& weakBlock : scene->getPushBlocks())
		{
			auto block = weakBlock.lock();
			if (!block)
			{ // 空
				continue;
			}

			// 押せるブロックの矩形の当たり判定情報
			Collision::Rectangle pushRect = block->getRect();	// 矩形情報
			float pushY = block->getPosY();						// 高さの原点
			float pushHeight = block->getHeight();				// 高さ

			// TODO: 中心点じゃなくて原点渡して高さで調整しているが、本来は原点ではなく原点を高さで補正して中心点を渡すべき

			// ブロックと水との当たり判定 (触れているかどうか)
			if (Collision::isCollisionBlockVsBlock(waterRect, pushRect, waterY, pushY, waterHeight, pushHeight))
			{
				block->setIsTouchWater(true);
			}

			// MEMO: こっちは中心点に調整済み？

			// ブロックが水にがっつり浸っているか (高さを少し余裕を持たせる)
			if (Collision::isCollisionBlockVsBlock(waterRect, pushRect, waterY, pushY + pushHeight, pushHeight, pushHeight - 1.0f))
			{
				block->setIsUnderWater(true);
			}
		}
	} break;
	default:
		assert(false);
		break;
	}

	// 親クラスの処理
	BlockDummy::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockIce::draw() const
{
#if 0
	// 親クラスの処理
	BlockDummy::draw();
#else
	if (getDummyFlags().isFlagSet(BlockDummy::DUMMY_FLAGS::NO_DISP))
	{ // 描画しない
		return;
	}

	if (getDummyFlags().isFlagSet(BlockDummy::DUMMY_FLAGS::DISP_WIRE))
	{
		// ワイヤーフレームを表示
		Renderer::onWireFrame();
	}

	// デバイスの取得
	auto& gm = GM;
	auto& renderer = gm.getRenderer();
	Device device = renderer.getDevice();
	//auto effect = renderer.getCircleShadow();

	//{
	//	Matrix mtxProj, mtxView;
	//	device->GetTransform(D3DTS_PROJECTION, &mtxProj);
	//	device->GetTransform(D3DTS_VIEW, &mtxView);
	//	Matrix* mtxWorld = getMatrix().get();
	//	Matrix wvp = DEF_MTX;
	//	D3DXMatrixMultiply(&wvp, mtxWorld, &mtxView);
	//	D3DXMatrixMultiply(&wvp, &wvp, &mtxProj);
	//	effect->SetMatrix("g_WVP", &wvp);
	//	effect->SetMatrix("g_W", mtxWorld);
	//}

	//effect->Begin(nullptr, 0U);
	//effect->BeginPass(0U);

	// 法線ベクトルを正規化
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// モデル情報の取得
	auto& resModel = getScene<SceneBase>()->getResManager()->getResModel(GameObject3D::getKey());

	GraphicsMaterial matDef;	// 現在のマテリアル保存用
	MeshMaterial* mat;			// マテリアルデータへのポインタ

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, GameObject3D::getMatrix().get());

	// 現在のマテリアルを取得
	device->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	mat = static_cast<MeshMaterial*>(resModel.getBufMat()->GetBufferPointer());

	for (DWORD i = 0; i < resModel.getNumMat(); i++)
	{
		// マテリアルの設定
#if 1
		device->SetMaterial(&m_mat);
#else
		device->SetMaterial(&mat[i].MatD3D);
#endif
		//effect->SetVector("g_Diffuse", reinterpret_cast<D3DXVECTOR4*>(&m_mat[static_cast<int>(i)].mat.Diffuse));

		// テクスチャの設定
		if (mat[i].pTextureFilename != nullptr)
		{ // テクスチャファイルが存在する
			const Texture& tex = getScene<SceneBase>()->getResManager()->getResTexture(resModel.getTexKeys(i)).getTexture();
			device->SetTexture(0U, tex);
			//effect->SetTexture("g_colorTexture", tex);
		}
		else
		{
			device->SetTexture(0U, nullptr);
			//effect->SetTexture("g_colorTexture", nullptr);
		}
		//effect->CommitChanges();
		device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

		// モデル (パーツ) の描画
		resModel.getMesh()->DrawSubset(i);
	}

	// 保存していたマテリアルを戻す
	device->SetMaterial(&matDef);

	// 法線ベクトルを元に戻す
	device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

	//effect->EndPass();
	//effect->End();

	if (getDummyFlags().isFlagSet(BlockDummy::DUMMY_FLAGS::DISP_WIRE))
	{
		// ワイヤーフレームを非表示
		Renderer::offWireFrame();
	}
#endif
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockIce::inspector()
{
	// 親クラスの処理
	BlockDummy::inspector();

	// Ice Config の設定
	if (ImGui::CollapsingHeader("Ice Config", ImGuiTreeNodeFlags_DefaultOpen))
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
					auto blocks = getScene<SceneBase>()->getIceBlocks()[m_pairKey];

					auto it = blocks.begin();
					while (it != blocks.end())
					{
						if (auto block = it->lock())
						{
							if (block == shared_from_this())
							{ // 自分自身
								it = blocks.erase(it);
								break;
							}
							else
							{
								++it;
							}
						}
						else
						{
							it = blocks.erase(it);
						}
					}
				}

				// ペアキーを更新して新しく追加
				m_pairKey = inputBuf;
				auto blocks = getScene<SceneBase>()->getIceBlocks()[m_pairKey];
				blocks.push_back(shared_from_this()->downcast<BlockIce>());
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockIce::save(json& outObject) const
{
	// 親クラスの処理
	BlockDummy::save(outObject);

	outObject[FLAMEANDUPDRAFT_PARAM_PAIR_KEY] = m_pairKey;
	outObject[ICE_BLOCK_PARAM_ICE_MAT_KEY] = m_iceMatKey;
	outObject[ICE_BLOCK_PARAM_WATER_MAT_KEY] = m_waterMatKey;
}

//---------------------------------------------------
// 水しぶき
//---------------------------------------------------
void BlockIce::splashOfWater()
{
	auto scene = getScene();
	Vec3 pos = scene->getPlayer()->getPos();

	// 粉々の生成
	for (int i = 0; i < NUM_PIECES; i++)
	{
		float scale = RandomGenerator::get(PIECES_RAND_SCALE_MIN, PIECES_RAND_SCALE_MAX);

		Vec3 move = DEF_VEC3;
		move.x = RandomGenerator::get(PIECES_RAND_MOVE_MIN, PIECES_RAND_MOVE_MAX);
		move.y = RandomGenerator::get(PIECES_RAND_MOVE_MIN, PIECES_RAND_MOVE_MAX);
		move.z = RandomGenerator::get(PIECES_RAND_MOVE_MIN, PIECES_RAND_MOVE_MAX);

		// パラメータの設定
		json j = json::parse(PARAM_SMALL_PIECES);
		j[FLAG_POS] = pos;
		j[WATERDROP_PARAM_FIRST_SCALE] = scale;
		j[WATERDROP_PARAM_MAX_LIFE] = RandomGenerator::get(PIECES_RAND_LIFE_MIN, PIECES_RAND_LIFE_MAX) * ADJUST_MILLSECOND;
		j[PARAM_BLOCK_SMALL_PIECES] = move;

		// オブジェクトの追加
		scene->addObject(FACTORY_SMALL_PIECES, j);
	}
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockIce& BlockIce::operator=(BlockIce&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockDummy::operator=(std::move(right));
	}

	return *this;
}
