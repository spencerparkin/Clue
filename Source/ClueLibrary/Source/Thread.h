#pragma once

#include "Defines.h"
#include <thread>
#include <mutex>
#include <list>

namespace Clue
{
	/**
	 *
	 */
	class CLUE_LIBRARY_API Thread
	{
	public:
		Thread(const std::string& name = "Unnamed Thread");
		virtual ~Thread();

		void SetName(const std::string& name);

		virtual bool Split();
		virtual bool Join();

		bool IsRunning() const;

	protected:

		virtual void Run() = 0;

	private:

		std::string name;
		std::thread* thread;
		volatile bool isRunning;
	};

	/**
	 *
	 */
	template<typename T>
	class ThreadSafeQueue
	{
	public:
		ThreadSafeQueue()
		{
		}

		virtual ~ThreadSafeQueue()
		{
		}

		void Add(T value)
		{
			std::lock_guard lock(this->listMutex);
			this->list.push_back(value);
		}

		bool Remove(T& value)
		{
			if (this->list.size() > 0)
			{
				std::lock_guard lock(this->listMutex);
				if (this->list.size() > 0)
				{
					value = *this->list.begin();
					this->list.pop_front();
					return true;
				}
			}

			return false;
		}

		void Clear()
		{
			std::lock_guard lock(this->listMutex);
			this->list.clear();
		}

		void ClearAndDelete()
		{
			std::lock_guard lock(this->listMutex);
			for (T& value : this->list)
				delete value;
			this->list.clear();
		}

	private:
		std::mutex listMutex;
		std::list<T> list;
	};
}