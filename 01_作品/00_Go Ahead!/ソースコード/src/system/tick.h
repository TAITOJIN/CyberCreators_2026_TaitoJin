/**********************************************************//**
 * @file tick.h
 * @brief 時間
 *
 * @author Taito Jin
 * @date since 2024/11/10
 **************************************************************/
#pragma once

/** @brief 時間 */
class Tick
{
public:
	/** @brief コンストラクタ */
	Tick();

	/** @brief デストラクタ */
	~Tick() = default;

	/** @brief リセット */
	void Reset();

	/** @brief 更新 */
	void Update();

	/**
	 * @brief デルタタイムの取得
	 * @return デルタタイム
	 */
	float GetDeltaTime() const { return _deltaTime; }

	/**
	 * @brief 時間の取得
	 * @return 時間
	 */
	float GetTime() const { return _time; }

	/**
	 * @brief FPS の取得
	 * @return FPS
	 */
	float GetFPS() const { return _fps; }

	/**
	 * @brief フレームカウントの取得
	 * @return 60 FPS のうち 1 フレーム分が経過したか
	 */
	bool GetIsElapsed() const { return _isElapsed; }

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _startTime; //!< 計測を開始した時間
	std::chrono::time_point<std::chrono::high_resolution_clock> _lastFrameTime; //!< 最後に測定をした時間
	std::chrono::time_point<std::chrono::high_resolution_clock> _fpsMeasurementLastTime; //!< FPS の計測を最後にした時間
	std::chrono::time_point<std::chrono::high_resolution_clock> _fpsLimitLastTime; //!< FPS の制限の計測を最後にした時間
	float _fps; //!< FPS
	float _deltaTime; //!< デルタタイム
	float _time; //!< 時間
	int _frameCount; //!< フレームカウント
	bool _isElapsed; //!< 経過したか
};
