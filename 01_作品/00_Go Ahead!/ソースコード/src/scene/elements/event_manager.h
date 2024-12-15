//--------------------------------------------------------------------------------
// 
// イベントの管理者 [event_manager.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// system
#include "system/thread_pool.h"

//===================================================
// イベントの管理者のクラス (Observer)
//===================================================
class EventManager
{
public:
	//---------------------------------------------------
	// エイリアス宣言
	//---------------------------------------------------
	using EventHandler = std::function<void(std::any)>;
	using HandlerWithPriority = std::tuple<int, EventHandler, bool, bool>;  // 優先度 (数値が低いのから処理), ハンドラ, 一度きりか否か, 非同期か否か

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	explicit EventManager(const size_t& inPoolSize);
	~EventManager() = default;

	// MEMO: registerEvent の中で notify したらダメ！
	//       破棄や再登録でイテレータを変更しているので、
	//       if(key != end()) で変更前のイテレータで比較して破棄された後のデータにアクセスしてバグる危険！
	//
	//       同じイベント名で優先度を設定して登録してください！
	void registerEvent(
		const std::string& inEventName,
		EventHandler inHandler,
		int inPriority = 0,
		bool inIsOnce = false,
		bool inIsAsync = false) { m_handlers[inEventName].emplace(inPriority, inHandler, inIsOnce, inIsAsync); }	// 登録
	void notifyEvent(const std::string& inEventName, std::any inEventData = {});									// 通知

private:
	//---------------------------------------------------
	// 優先度の比較
	//---------------------------------------------------
	struct ComparePriority
	{
		bool operator()(const HandlerWithPriority& a, const HandlerWithPriority& b)
		{
			return std::get<0>(a) < std::get<0>(b);	// 優先度の低い順にソート (低いのが先頭)
		}
	};

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::unordered_map<std::string, std::priority_queue<HandlerWithPriority, std::vector<HandlerWithPriority>, ComparePriority>> m_handlers;	// イベントハンドラ
	ThreadPool m_threadPool;	// スレッドプール
};
