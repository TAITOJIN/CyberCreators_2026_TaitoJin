//--------------------------------------------------------------------------------
// 
// スレッドプール [thread_pool.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// スレッドプールのクラス
//===================================================
class ThreadPool
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	explicit ThreadPool(const size_t& inNumThreads);
	~ThreadPool();

	template<class T> void enqueue(T&& task);	// 末尾にタスクを追加

private:
	//---------------------------------------------------
	// エイリアス宣言
	//---------------------------------------------------
	using Task = std::function<void()>;	// タスク

	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	ThreadPool(const ThreadPool&) = delete;
	void operator=(const ThreadPool&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::vector<std::thread> m_workers;		// スレッド
	std::queue<Task> m_tasks;				// タスク
	std::mutex m_queueMutex;				// タスクキューへの排他的アクセスを管理 (MEMO: condition_variable で mutex の lock 使うので今回はクリティカルセクションじゃない)
	std::condition_variable m_condition;	// タスクの到着を待つ
	std::atomic<bool> m_stop;				// スレッドプールの停止状態
};

//---------------------------------------------------
// 末尾にタスクを追加
//---------------------------------------------------
template<class T>
inline void ThreadPool::enqueue(T&& task)
{
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);

		if (m_stop.load())
		{
			throw std::runtime_error("停止したスレッドプールにタスクを追加しようとしています...");
		}

		m_tasks.emplace(std::forward<T>(task));	// 引数を転送
	}

	m_condition.notify_one();	// 待機しているスレッドへ、新しいタスクが追加されたことを伝える！
}
