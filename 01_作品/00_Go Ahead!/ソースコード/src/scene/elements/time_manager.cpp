//--------------------------------------------------------------------------------
// 
// 時間の管理者 [time_manager.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
// // elements
#include "scene/elements/time_manager.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float SECONDS_PER_MEASUREMENT = 0.5f;		// FPSを計測する間隔 (秒)
	constexpr float MILLISECONDS_PER_SECOND = 1000.0f;	// ミリ秒
	constexpr int FPS_UPDATE_INTERVAL = 60;				// 更新間隔

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
TimeManager::TimeManager()
	: m_startTime()
	, m_lastFrameTime()
	, m_fpsLastTime()
	, m_scheduledEvents()
	, m_frameCount(0)
	, m_fps(0.0f)
	, m_deltaTime(0.0f)
	, m_time(0.0f)
	, m_timeScale(0.0f)
{
	// リセット
	reset();
}

//---------------------------------------------------
// リセット
//---------------------------------------------------
void TimeManager::reset()
{
	m_startTime = std::chrono::high_resolution_clock::now();
	m_lastFrameTime = m_startTime;
	m_fpsLastTime = std::chrono::high_resolution_clock::now();
	m_scheduledEvents.clear();
	m_frameCount = 0;
	m_fps = 0.0f;
	m_deltaTime = 0.0f;
	m_time = 0.0f;
	m_timeScale = DEF_TIMESCALE;
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void TimeManager::update()
{
	auto currentTime = std::chrono::high_resolution_clock::now();			// 現在の時間を取得
	std::chrono::duration<float> duration = currentTime - m_lastFrameTime;	// 間隔
	m_deltaTime = duration.count() * m_timeScale;							// デルタタイムの設定
	m_time += m_deltaTime;			// 経過時間の加算
	m_lastFrameTime = currentTime;	// 最後の測定した時刻の設定
	m_frameCount++;					// フレームカウントを加算

	// FPS の計測
	{
		auto fpsDuration = currentTime - m_fpsLastTime;
		if (fpsDuration >= std::chrono::milliseconds(static_cast<int>(MILLISECONDS_PER_SECOND * HALF)))
		{ // 0.5 秒経過
			float elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(fpsDuration).count() / MILLISECONDS_PER_SECOND;	// 経過時間を秒に変換
			m_fps = static_cast<float>(m_frameCount) / elapsedTime;	// FPS を計算
			m_fpsLastTime = currentTime;							// FPS を測定した時刻を保存
			m_frameCount = 0;										// フレームカウントをクリア
		}
	}

	// スケジュールされたイベントを実行
	for (auto it = m_scheduledEvents.begin(); it != m_scheduledEvents.end();)
	{
		if (m_time >= it->time)
		{
			it->action();
			it = m_scheduledEvents.erase(it);
		}
		else
		{
			++it;
		}
	}
}
