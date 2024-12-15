//--------------------------------------------------------------------------------
// 
// 共有データ [shared_data.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// 共有データクラス
//===================================================
class SharedData
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	SharedData();
	~SharedData();

	void setTime(const float& inTime) { m_time = inTime; };
	void setNumCoin(const int& inNum) { m_numCoin = inNum; }

	const float& getTime() const { return m_time; }
	const int& getNumCoin() const { return m_numCoin; }

#if USE_NEW_SHARED_DATA
	float m_time;	// 時間
	int m_numCoin;	// コインの数

	int m_numImpact;			// インパクトを発動させた数
	float m_timeFlamePlayer;	// プレイヤーをこんがり焼いた時間
	bool m_isBreakSnowman;		// 雪だるまを破壊したか否か

	float m_timeOnIce;		// 氷の上にいた時間
	float m_timeUnderWater;	// 水中にいた時間

	float m_timeTouchPush;	// 押せるブロックに触れていた時間

	btVector3 m_maxRotVelocity;	// 最大回転速度

	bool m_isFindCoinZone;	// コインゾーンを見つけたか否か

	int m_numJump;	// ジャンプした回数
#endif

	void reset();	// リセット

#if 1
private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	float m_time;	// 時間
	int m_numCoin;	// コインの数
#endif
};
