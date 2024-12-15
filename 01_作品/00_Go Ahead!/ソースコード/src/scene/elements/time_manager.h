//--------------------------------------------------------------------------------
// 
// 時間の管理者 [time_manager.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// 時間のクラス
//===================================================
class TimeManager
{
public:
	//---------------------------------------------------
	// public 定数
	//---------------------------------------------------
	static constexpr float DEF_TIMESCALE = 1.0f;	// デフォルト時間経過速度

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	TimeManager();
	~TimeManager() = default;

	void reset();	// リセット
	void update();	// 更新

	void setDeltaTime(const float& inDeltaTime) { m_deltaTime = inDeltaTime; }	// デルタタイムの設定
	void setTime(const float& inTime) { m_time = inTime; }						// 時間の設定
	void setFPS(const float& inFPS) { m_fps = inFPS; }							// FPS の設定
	void setTimeScale(const float& inScale) { m_timeScale = inScale; }			// 時間経過速度の設定
	void addScheduleEvent(const float& inTime, std::function<void()> inAction) { m_scheduledEvents.push_back({ inTime, inAction }); }	// イベントの追加

	const float& getDeltaTime() const { return m_deltaTime; }	// デルタタイムの取得
	const float& getTime() const { return m_time; }				// 時間の取得
	const float& getFPS() const { return m_fps; }				// FPS の取得
	const float& getTimeScale() const { return m_timeScale; }	// 時間経過速度の取得

private:
	//---------------------------------------------------
	// private 構造体
	//---------------------------------------------------
	// イベントの情報
	struct Event
	{
		float time;						// 時間
		std::function<void()> action;	// 行う処理
	};

	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	TimeManager(const TimeManager&) = delete;
	void operator=(const TimeManager&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;		// 計測を開始した時間
	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastFrameTime;	// 最後に測定をした時間
	std::chrono::time_point<std::chrono::high_resolution_clock> m_fpsLastTime;		// FPS を最後に測定した時間
	std::vector<Event> m_scheduledEvents;	// イベント
	int m_frameCount;						// フレームカウント
	float m_fps;							// FPS
	float m_deltaTime;						// デルタタイム
	float m_time;							// 時間
	float m_timeScale;						// 時間経過速度
};
