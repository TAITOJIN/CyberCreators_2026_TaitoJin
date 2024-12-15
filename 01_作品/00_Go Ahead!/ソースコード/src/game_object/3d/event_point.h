//--------------------------------------------------------------------------------
// 
// イベント地点 [event_point.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/polygon_3d.h"

//===================================================
// イベント地点のクラス
//===================================================
class EventPoint : public Polygon3D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	EventPoint();
	~EventPoint() override = default;
	EventPoint(EventPoint&& right) noexcept;
	EventPoint& operator=(EventPoint&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	EventPoint(const EventPoint&) = delete;
	void operator=(const EventPoint&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::string m_eventName;	// イベント名
	bool m_done;				// 既に完了
};
