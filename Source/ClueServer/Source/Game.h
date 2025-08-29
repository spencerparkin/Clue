#pragma once

#include <coroutine>

class Server;

class GameTask
{
public:
	class promise_type;
	using CoroHandle = std::coroutine_handle<promise_type>;

	GameTask(GameTask&& gameTask);
	explicit GameTask(promise_type* promiseType);
	virtual ~GameTask();

	bool IsDone();
	void Resume();

	class promise_type
	{
	public:
		GameTask get_return_object();

		std::suspend_always initial_suspend() noexcept;
		std::suspend_always final_suspend() noexcept;
		std::suspend_always yield_value(int) noexcept;
		void return_void() noexcept;
		void unhandled_exception() noexcept;
	};

private:
	CoroHandle coroHandle;
};

GameTask PlayGame(Server* server);