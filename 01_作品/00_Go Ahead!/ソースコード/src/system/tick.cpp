//--------------------------------------------------------------------------------
// 
// 時間 [tick.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// system
#include "system/tick.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* Anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float SECONDS_PER_MEASUREMENT = 0.5f;		// FPSを計測する間隔 (秒)
	constexpr float MILLISECONDS_PER_SECOND = 1000.0f;	// ミリ秒
	constexpr int FPS_UPDATE_INTERVAL = 60;				// 更新間隔

} // namespace /* Anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Tick::Tick()
	: _startTime()
	, _lastFrameTime()
	, _fpsMeasurementLastTime()
	, _fpsLimitLastTime()
	, _frameCount(0)
	, _fps(0.0f)
	, _deltaTime(0.0f)
	, _time(0.0f)
{
	// リセット
	Reset();
}

//---------------------------------------------------
// リセット
//---------------------------------------------------
void Tick::Reset()
{
	std::chrono::steady_clock::time_point currTime = std::chrono::high_resolution_clock::now();
	_startTime = currTime;
	_lastFrameTime = _startTime;
	_fpsMeasurementLastTime = currTime;
	_fpsLimitLastTime = currTime;
	_frameCount = 0;
	_fps = 0.0f;
	_deltaTime = 0.0f;
	_time = 0.0f;
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Tick::Update()
{
	_isElapsed = false; // 経過していない
	std::chrono::steady_clock::time_point currTime = std::chrono::high_resolution_clock::now(); // 現在の時間を取得

	// デルタタイムの計測
	{
		std::chrono::duration<float> duration = currTime - _lastFrameTime; // 間隔
		_deltaTime = duration.count(); // デルタタイムの設定
		_lastFrameTime = currTime; // 最後に測定した時刻の設定
		_time += _deltaTime; // 経過時間の加算
	}

	// FPS の計測
	{
		std::chrono::duration<float> duration = currTime - _fpsMeasurementLastTime; // 間隔
		if (duration >= std::chrono::milliseconds(static_cast<int>(MILLISECONDS_PER_SECOND * HALF)))
		{ // 0.5 秒経過
			float elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / MILLISECONDS_PER_SECOND; // 経過時間を秒に変換
			_fps = static_cast<float>(_frameCount) / elapsedTime; // FPS を計算
			_fpsMeasurementLastTime = currTime; // FPS を測定した時刻を保存
			_frameCount = 0; // フレームカウントをクリア
		}
	}

	// FPS の制限
	{
		std::chrono::duration<float> duration = currTime - _fpsLimitLastTime; // 間隔
		if (duration >= std::chrono::milliseconds(static_cast<int>(MILLISECONDS_PER_SECOND / FPS_UPDATE_INTERVAL)))
		{ // 60 fps の 1 フレーム分経過
			_fpsLimitLastTime = currTime;
			_isElapsed = true;

			_frameCount++; // フレームカウントを加算
		}
	}
}
