//--------------------------------------------------------------------------------
// 
// 共有データ [shared_data.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
// // elements
#include "scene/elements/shared_data.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
SharedData::SharedData()
	: m_time(0.0f)
	, m_numCoin(0)
#if USE_NEW_SHARED_DATA
	, m_numImpact(0)
	, m_timeFlamePlayer(0.0f)
	, m_isBreakSnowman(false)
	, m_timeOnIce(0.0f)
	, m_timeUnderWater(0.0f)
	, m_timeTouchPush(0.0f)
	, m_maxRotVelocity(btVector3(0.0f, 0.0f, 0.0f))
	, m_isFindCoinZone(false)
	, m_numJump(0)
#endif
{
	// DO_NOTHING
}

//---------------------------------------------------
// リセット
//---------------------------------------------------
void SharedData::reset()
{
	m_time = 0.0f;
	m_numCoin = 0;
#if USE_NEW_SHARED_DATA
	m_numImpact = 0;
	m_timeFlamePlayer = 0.0f;
	m_isBreakSnowman = false;
	m_timeOnIce = 0.0f;
	m_timeUnderWater = 0.0f;
	m_timeTouchPush = 0.0f;
	m_maxRotVelocity = btVector3(0.0f, 0.0f, 0.0f);
	m_isFindCoinZone = false;
	m_numJump = 0;
#endif
}

//---------------------------------------------------
// デストラクタ
//---------------------------------------------------
SharedData::~SharedData()
{
	// DO_NOTHING
}
