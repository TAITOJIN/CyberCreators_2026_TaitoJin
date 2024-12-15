//--------------------------------------------------------------------------------
// 
// イベントの管理者 [event_manager.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
// // elements
#include "scene/elements/event_manager.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
EventManager::EventManager(const size_t& inPoolSize) : m_threadPool(inPoolSize)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 通知
//---------------------------------------------------
void EventManager::notifyEvent(const std::string& inEventName, std::any inEventData)
{
	if (m_handlers.find(inEventName) != m_handlers.end())
	{
		auto& handlerQueue = m_handlers[inEventName];
		std::priority_queue<HandlerWithPriority, std::vector<HandlerWithPriority>, ComparePriority> tempQueue;

		while (!handlerQueue.empty())
		{
			auto [priority, handler, isOnce, isAsync] = handlerQueue.top();
			handlerQueue.pop();

			if (isAsync)
			{ // 非同期
				m_threadPool.enqueue([handler, inEventData] {
					handler(inEventData);
				});
			}
			else
			{
				handler(inEventData);
			}

			if (!isOnce)
			{ // 一度きりじゃない
				tempQueue.emplace(priority, handler, isOnce, isAsync);	// 再登録
			}
		}

		m_handlers[inEventName] = std::move(tempQueue);
	}
}
