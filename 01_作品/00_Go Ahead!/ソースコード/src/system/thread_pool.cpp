//--------------------------------------------------------------------------------
// 
// スレッドプール [thread_pool.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// system
#include "system/thread_pool.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
ThreadPool::ThreadPool(const size_t& inNumThreads) : m_stop(false)
{
	for (size_t i = 0; i < inNumThreads; i++)
	{
		m_workers.emplace_back([this]
		{
			while (true)
			{
				Task task;
				{
					std::unique_lock<std::mutex> lock(this->m_queueMutex);

					// タスクの到着を待機
					this->m_condition.wait(lock, [this]
					{
						return this->m_stop.load() || !this->m_tasks.empty();
					});

					if (this->m_stop.load() && this->m_tasks.empty())
					{ // スレッドプールの停止、かつタスクが空
						return;
					}

					task = std::move(this->m_tasks.front());
					this->m_tasks.pop();
				}

				// タスクの実行
				task();
			}
		});
	}
}

//---------------------------------------------------
// デストラクタ
//---------------------------------------------------
ThreadPool::~ThreadPool()
{
	// スレッドプールの停止
	m_stop.store(true);

	// スレッドプールが停止したことをスレッドたちにお知らせする
	m_condition.notify_all();

	// スレッドが終了するまで待機
	for (auto& worker : m_workers)
	{
		worker.join();
	}
}
