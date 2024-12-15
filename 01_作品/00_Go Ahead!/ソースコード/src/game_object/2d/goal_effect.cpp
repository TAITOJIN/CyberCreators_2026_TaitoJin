//--------------------------------------------------------------------------------
// 
// ゴールエフェクト [goal_effect.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 2d
#include "game_object/2d/goal_effect.h"
// game_manager
#include "game_manager/game_manager.h"
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
	constexpr int NUM_MESHFIELD_SETIDX = 2;	// 2 個セットで考える
	constexpr int NUM_DEGENERATE_VTX = 2;	// 1 改行につき縮退ポリを作成するのに必要な頂点数

	constexpr float TIME_FIRST = 0.0f;				
	constexpr float TIME_SECOND = TIME_FIRST + 2.0f;
	constexpr float TIME_LAST = TIME_SECOND + 2.0f;
	constexpr float TIME_ZOOM = TIME_LAST + 2.0f;
	constexpr float TIME_GO_RESULT = TIME_ZOOM + 0.6f;

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
GoalEffect::GoalEffect()
	: GameObject()
	, m_vtxBuf(nullptr)
	, m_pos(DEF_VEC3)
	, m_rot(DEF_VEC3)
	, m_size(DEF_VEC3)
	, m_col(DEF_COL)
	, m_idxBuf(nullptr)
	, m_vtxNumX(0U)
	, m_vtxNumY(0U)
	, m_nIdxNum(0U)
	, m_nPrimNum(0U)
	, m_isEffectIng(false)
	, m_wasEffectIng(false)
	, m_done(false)
	, m_effectTime(0.0f)
	, m_ellipseTime(0.0f)
	, m_firstEffect(false)
	, m_secondEffect(false)
	, m_lastEffect(false)
	, m_zoom(false)
	, m_isGoResult(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
GoalEffect::GoalEffect(GoalEffect&& right) noexcept
	: GameObject(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT GoalEffect::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject::init(inParam);
	if (FAILED(hr))
	{
		return hr;
	}

	Vec3 pos = Vec3(SCREEN_WIDTH * HALF, SCREEN_HEIGHT * HALF, 0.0f);
	Vec3 size = Vec3(990.0f, 231.0f, 0.0f);

	Vec2 ellipsePos = Vec2(pos.x, pos.y);
	m_ellipses[EFFECT_CIRCLE_TYPE_PINK_RED] = { ellipsePos, Vec2(0.1f, 0.2f), 0.0f, Color(1.0f, 0.0f, 0.689f, 1.0f), 0.0f };
	m_ellipses[EFFECT_CIRCLE_TYPE_BLUE_LIGHTBLUE] = { ellipsePos, Vec2(0.1f, 0.2f), 0.0f, Color(0.0f, 1.0f, 0.9137f, 1.0f), 0.25f };
	m_ellipses[EFFECT_CIRCLE_TYPE_WHITE_0] = { ellipsePos, Vec2(0.1f, 0.2f), 0.0f, Color(0.941f, 0.941f, 0.941f, 1.0f), 0.5f };

	// 初期パラメータの設定
	{
		m_pos = pos;
		m_rot = Vec3(0.0f, 0.0f, 0.0f);
		m_size = size;

		m_col = Color(1.0f, 1.0f, 1.0f, 0.0f);

		m_texKey = "res\\texture\\GOAL_fill.png";

		m_vtxNumX = 331;
		m_vtxNumY = 77;

		m_time.reset();
	}

	// ブロック数
	uint32_t blockNumX = m_vtxNumX - 1;
	uint32_t blockNumY = m_vtxNumY - 1;

	// ポリゴンの数 => 縦 * 横 * 2【四角形は 2 つの三角形で作られている】 + 4【 1 行につき 4 縮退ポリ】 * (縦の数 - 1【最初の行はカウントしない】)
	m_nPrimNum = blockNumX * blockNumY * 2 + 4 * (blockNumY - 1);

	// インデックスの数 => ポリゴンの数 + 2
	m_nIdxNum = m_nPrimNum + 2;

	// 縦，横のカウント
	uint32_t x_i, y_i;
	x_i = y_i = 0;

	// 頂点情報の作成
	{
		auto& gm = GM; // ゲームの管理者の取得

		// クリティカルセクションに入る
		gm.lockCS();

		// デバイスの取得
		Device device = gm.getDevice();

		// 頂点バッファの生成
		hr = device->CreateVertexBuffer(
			sizeof(VERTEX_2D) * (m_vtxNumX * m_vtxNumY),
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_2D,
			D3DPOOL_MANAGED,
			m_vtxBuf.GetAddressOf(),
			nullptr);
		if (FAILED(hr))
		{
			gm.unlockCS();
			return E_FAIL;
		}

		// インデックスバッファの生成
		hr = device->CreateIndexBuffer(
			sizeof(WORD) * m_nIdxNum,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			m_idxBuf.GetAddressOf(),
			nullptr);
		if (FAILED(hr))
		{
			gm.unlockCS();
			return E_FAIL;
		}

		// クリティカルセクションから離れる
		gm.unlockCS();

		// 原点 (左上)
		Vec2 basePos = Vec2(m_size.x * HALF, m_size.y * HALF);

		// ポリゴン，テクスチャのサイズ
		Vec2 texSize = Vec2(m_size.x / blockNumX, m_size.y / blockNumY);
		Vec2 texCoord = Vec2(1.0f / blockNumX, 1.0f / blockNumY);

		// 頂点バッファをロックし，頂点情報へのポインタを取得
		VERTEX_2D* pVtx;
		hr = m_vtxBuf.Get()->Lock(0U, 0U, reinterpret_cast<void**>(&pVtx), 0U);
		if (FAILED(hr))
		{
			return E_FAIL;
		}

		// 頂点情報の設定
		uint32_t allVtxNum = m_vtxNumX * m_vtxNumY;
		for (uint32_t i_vtx = 0; i_vtx < allVtxNum; ++i_vtx, ++pVtx)
		{
			x_i = i_vtx % (blockNumX + 1);
			y_i = i_vtx / (blockNumX + 1);

			pVtx->pos = Vec3(m_pos.x - basePos.x + texSize.x * x_i,
				m_pos.y - basePos.y + texSize.y * y_i,
				0.0f);
			pVtx->rhw = 1.0f;
			pVtx->col = m_col;
			pVtx->tex = Vec2(texCoord.x * x_i, texCoord.y * y_i);
		}

		// 頂点バッファをアンロック
		hr = m_vtxBuf.Get()->Unlock();
		if (FAILED(hr))
		{
			return E_FAIL;
		}
	}

	// インデックスの設定
	{
		// インデックスバッファをロック
		WORD* pIdx;
		hr = m_idxBuf.Get()->Lock(0U, 0U, reinterpret_cast<void**>(&pIdx), 0U);
		if (FAILED(hr))
		{
			return E_FAIL;
		}

		// インデックスの設定
		for (y_i = 0; y_i < blockNumY; ++y_i)
		{
			int nBaseVtx = y_i * m_vtxNumX;

			for (x_i = 0; x_i < blockNumX; ++x_i)
			{
				pIdx[0] = static_cast<WORD>(nBaseVtx + x_i + m_vtxNumX);
				pIdx[1] = static_cast<WORD>(nBaseVtx + x_i);
				pIdx += NUM_MESHFIELD_SETIDX;
			}

			// 縮退
			if (y_i < blockNumY - 1)
			{
				pIdx[0] = static_cast<WORD>(nBaseVtx + blockNumX);
				pIdx[1] = static_cast<WORD>((y_i + 1) * m_vtxNumX);
				pIdx += NUM_DEGENERATE_VTX;
			}
		}

		// インデックスバッファのアンロック
		hr = m_idxBuf.Get()->Unlock();
		if (FAILED(hr))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void GoalEffect::uninit()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void GoalEffect::update()
{
	m_time.update();

	if (!m_isEffectIng)
	{
		return;
	}

	auto game = getScene<SceneGame>();
	if (!game)
	{
		return;
	}

	float deltaTime = m_time.getDeltaTime();
	m_effectTime += deltaTime;

	bool wasFirst = m_firstEffect;

	if (m_effectTime >= TIME_FIRST && m_effectTime < TIME_SECOND)
	{
		m_firstEffect = true;
		if (!wasFirst && m_firstEffect && false)
		{
			auto playerPos = game->getPlayer()->getPos();
			game->getCamera()->setPos(Vec3(playerPos.x - 100.0f, playerPos.y, playerPos.z - 100.0f), playerPos);
		}

		m_ellipseTime += deltaTime;

		// ブロック数
		uint32_t blockNumX = m_vtxNumX - 1;
		uint32_t blockNumY = m_vtxNumY - 1;

		// 縦，横のカウント
		uint32_t x_i, y_i;
		x_i = y_i = 0;

		// サイズの更新
		if (m_effectTime >= TIME_SECOND - 1.1f)
		{
			m_size.x *= 1.1f;
			m_size.y *= 1.1f;
		}

		// 原点 (左上)
		Vec2 basePos = Vec2(m_size.x * HALF, m_size.y * HALF);

		// ポリゴン，テクスチャのサイズ
		Vec2 texSize = Vec2(m_size.x / blockNumX, m_size.y / blockNumY);

		// 頂点バッファをロックし，頂点情報へのポインタを取得
		VERTEX_2D* pVtx;
		m_vtxBuf.Get()->Lock(0U, 0U, reinterpret_cast<void**>(&pVtx), 0U);

		// 頂点情報の設定
		uint32_t allVtxNum = m_vtxNumX * m_vtxNumY;
		for (uint32_t i_vtx = 0; i_vtx < allVtxNum; i_vtx++, pVtx++)
		{
			Vec2 pos = Vec2(pVtx[0].pos.x, pVtx[0].pos.y);

			for (auto& ellipse : m_ellipses)
			{
				if (m_ellipseTime < ellipse.time)
				{
					continue;
				}
				ellipse.radius.x += 0.001f;

				float x = ellipse.pos.x - pos.x;
				float y = ellipse.pos.y - pos.y;
				float r = (x * x) + (y * y);

				if (r < (ellipse.radius.x * ellipse.radius.x))
				{
					pVtx[0].col = ellipse.col;
				}

				x_i = i_vtx % (blockNumX + 1);
				y_i = i_vtx / (blockNumX + 1);

				pVtx[0].pos = Vec3(
					m_pos.x - basePos.x + texSize.x * x_i,
					m_pos.y - basePos.y + texSize.y * y_i,
					0.0f);
			}
		}

		// 頂点バッファをアンロック
		m_vtxBuf.Get()->Unlock();

	}
	else if (m_effectTime >= TIME_SECOND && !m_isGoResult)
	{
		//m_zoom = false;
		m_isGoResult = true;
		m_done = true;
	}

	if (m_done)
	{
		if (!game->getIsGoal())
		{ // まだゴールしていない
			// ゴール状態に設定
			game->setIsGoal(true);
		}
	}
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void GoalEffect::draw() const
{
	if (!m_isEffectIng)
	{
		return;
	}

	// デバイスの取得
	Device device = GM.getDevice();

	// Z バッファー処理を有効にする
	//device->SetRenderState(D3DRS_ZENABLE, true);

	auto rm = getScene<SceneBase>()->getResManager();

	// テクスチャの取得
	Texture texture = rm->getResTexture(m_texKey).getTexture();

	// 頂点バッファをデータストリームに設定
	device->SetStreamSource(0U, m_vtxBuf.Get(), 0U, sizeof(VERTEX_2D));

	// インデックスバッファをデータストリームに設定
	device->SetIndices(m_idxBuf.Get());

	// 頂点フォーマットの設定
	device->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	device->SetTexture(0U, texture);

	// メッシュフィールドの描画
	device->DrawIndexedPrimitive(
		D3DPT_TRIANGLESTRIP,	// プリミティブの種類
		0,						// 頂点バッファのベースとなる位置
		0U,						// 頂点バッファの1番目の項 ( BaseVertexIndex で示した位置を 0 番目とする) から使用する
		m_vtxNumX * m_vtxNumY,	// MinIndex で示される頂点番号からどの範囲まで頂点バッファを使用するか
		0U,						// 頂点インデックスバッファの開始位置
		m_nPrimNum);			// ポリゴンの数

	// Z バッファー処理を無効にする
	//device->SetRenderState(D3DRS_ZENABLE, false);
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void GoalEffect::inspector()
{
	// 親クラスの処理
	GameObject::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void GoalEffect::save(json& outObject) const
{
	// 親クラスの処理
	GameObject::save(outObject);
#if 0
	outObject[OBJECT2D_PARAM_POS] = m_pos;
	outObject[OBJECT2D_PARAM_ROT] = m_rot;
	outObject[OBJECT2D_PARAM_SIZE] = m_size;
	outObject[OBJECT2D_PARAM_COL] = m_col;
	outObject[OBJECT2D_PARAM_TEXKEY] = m_texKey;
	outObject[PARAM_MESH_VTX_NUM_X] = m_vtxNumX;
	outObject[PARAM_MESH_VTX_NUM_Y] = m_vtxNumY;
#endif
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
GoalEffect& GoalEffect::operator=(GoalEffect&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		GameObject::operator=(std::move(right));
	}

	return *this;
}
