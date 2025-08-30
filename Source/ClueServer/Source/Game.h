#pragma once

#include <coroutine>
#include "Packet.h"

class Server;
class Player;

class GameTask
{
public:
	class promise_type;
	using CoroHandle = std::coroutine_handle<promise_type>;

	GameTask(GameTask&& gameTask) noexcept;
	explicit GameTask(promise_type* promiseType);
	virtual ~GameTask();

	bool IsDone();
	void Resume();
	bool GotPacketFromPlayer();

	class PacketAwaiter;

	class promise_type
	{
	public:
		promise_type();

		GameTask get_return_object();

		std::suspend_always initial_suspend() noexcept;
		std::suspend_always final_suspend() noexcept;
		PacketAwaiter await_transform(Player* packetNeededFromPlayer) noexcept;
		void return_void() noexcept;
		void unhandled_exception() noexcept;

		std::shared_ptr<Clue::Packet> GetPlayerPacket();
		bool GotPacketFromPlayer();

	private:
		std::shared_ptr<Clue::Packet> playerPacket;
		Player* packetNeededFromPlayer;
	};

	class PacketAwaiter
	{
	public:
		PacketAwaiter(promise_type* promise);
		bool await_ready() const noexcept;
		std::shared_ptr<Clue::Packet> await_resume() const noexcept;
		void await_suspend(std::coroutine_handle<promise_type> coroHandle) const noexcept;
	private:
		promise_type* promise;
	};

private:
	CoroHandle coroHandle;
};

GameTask PlayGame(Server* server);