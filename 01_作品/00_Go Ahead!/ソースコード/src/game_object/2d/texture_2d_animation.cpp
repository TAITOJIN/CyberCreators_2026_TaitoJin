//--------------------------------------------------------------------------------
// 
// 2D テクスチャアニメーション [texture_2d_animation.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 2d
#include "game_object/2d/texture_2d_animation.h"
// game_manager
#include "game_manager/game_manager.h"

//---------------------------------------------------
// デフォルトコンストラクタ
//---------------------------------------------------
Tex2dAnim::Tex2dAnim()
	: m_counterAnim(0)
	, m_patternAnimU(0)
	, m_patternAnimV(0)
{
	// DO_NOTHING
}

//---------------------------------------------------
// コピーコンストラクタ
//---------------------------------------------------
Tex2dAnim::Tex2dAnim(const Tex2dAnim& right)
	: m_counterAnim(right.m_counterAnim)
	, m_patternAnimU(right.m_patternAnimU)
	, m_patternAnimV(right.m_patternAnimV)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
Tex2dAnim::Tex2dAnim(Tex2dAnim&& right) noexcept 
	: m_counterAnim(std::move(right.m_counterAnim))
	, m_patternAnimU(std::move(right.m_patternAnimU))
	, m_patternAnimV(std::move(right.m_patternAnimV))
{
	// DO_NOTHING
}

//---------------------------------------------------
// デストラクタ
//---------------------------------------------------
Tex2dAnim::~Tex2dAnim()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Tex2dAnim::init()
{
	return S_OK;
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
bool Tex2dAnim::update(const VertexBuffer& inVtx, const ResourceTexture& inResTex)
{
	bool isAnimFin = false;	// アニメーション終了

	// 頂点バッファをロック
	VERTEX_2D* vtx;
	HRESULT hr = inVtx->Lock(0U, 0U, reinterpret_cast<void**>(&vtx), 0);
	assert(SUCCEEDED(hr));

	m_counterAnim++;	// カウンターを加算

	// アニメーション処理の更新
	if (m_counterAnim >= inResTex.getAnimInterVal())
	{
		m_counterAnim = 0;	// カウンターを初期値に戻す
		m_patternAnimU++;	// パターン No. (U) を更新する

		int texU = inResTex.getU();
		int texV = inResTex.getV();

		if (m_patternAnimU > texU - 1)
		{
			m_patternAnimU = 0;	// パターン No. (U) を初期値に戻す
			m_patternAnimV++;	// パターン No. (V) を更新する

			if (m_patternAnimV > texV - 1)
			{
				m_patternAnimV = 0;	// パターン No. (V) を初期値に戻す
				isAnimFin = true;	// アニメーション終了
			}
		}

		// 座標の計算
		float u = 1.0f / static_cast<float>(texU);
		float u1 = static_cast<float>(m_patternAnimU) * u;
		float u2 = static_cast<float>((m_patternAnimU + 1)) * u;
		float v = 1.0f / static_cast<float>(texV);
		float v1 = static_cast<float>(m_patternAnimV) * v;
		float v2 = static_cast<float>((m_patternAnimV + 1)) * v;

		// テクスチャ座標の設定
		vtx[0].tex = Vec2(u1, v1);
		vtx[1].tex = Vec2(u2, v1);
		vtx[2].tex = Vec2(u1, v2);
		vtx[3].tex = Vec2(u2, v2);
	}

	// 頂点バッファをアンロック
	hr = inVtx->Unlock();
	assert(SUCCEEDED(hr));

	return isAnimFin;
}

//---------------------------------------------------
// テクスチャ座標の更新 (移動)
//---------------------------------------------------
void Tex2dAnim::updateTexCoord(const VertexBuffer& inVtx, const Vec2& inMove)
{
	// 頂点バッファをロック
	VERTEX_2D* vtx;
	HRESULT hr = inVtx->Lock(0U, 0U, reinterpret_cast<void**>(&vtx), 0);
	assert(SUCCEEDED(hr));

	// テクスチャ座標の設定
	vtx[0].tex += inMove;
	vtx[1].tex += inMove;
	vtx[2].tex += inMove;
	vtx[3].tex += inMove;

	// 頂点バッファをアンロック
	hr = inVtx->Unlock();
	assert(SUCCEEDED(hr));
}

//---------------------------------------------------
// テクスチャ座標の設定
//---------------------------------------------------
void Tex2dAnim::setTexCoord(
	const VertexBuffer& inVtx,
	const Vec2& inPos1,
	const Vec2& inPos2,
	const Vec2& inPos3,
	const Vec2& inPos4)
{
	// 頂点バッファをロック
	VERTEX_2D* vtx;
	HRESULT hr = inVtx->Lock(0U, 0U, reinterpret_cast<void**>(&vtx), 0);
	assert(SUCCEEDED(hr));

	// テクスチャ座標の設定
	vtx[0].tex = inPos1;
	vtx[1].tex = inPos2;
	vtx[2].tex = inPos3;
	vtx[3].tex = inPos4;

	// 頂点バッファをアンロック
	hr = inVtx->Unlock();
	assert(SUCCEEDED(hr));
}

//---------------------------------------------------
// コピー代入演算子
//---------------------------------------------------
Tex2dAnim& Tex2dAnim::operator=(const Tex2dAnim& right)
{
	if (this != &right)
	{
		m_counterAnim = right.m_counterAnim;
		m_patternAnimU = right.m_patternAnimU;
		m_patternAnimV = right.m_patternAnimV;
	}

	return *this;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
Tex2dAnim& Tex2dAnim::operator=(Tex2dAnim&& right) noexcept
{
	if (this != &right)
	{
		m_counterAnim = std::move(right.m_counterAnim);
		m_patternAnimU = std::move(right.m_patternAnimU);
		m_patternAnimV = std::move(right.m_patternAnimV);
	}

	return *this;
}
