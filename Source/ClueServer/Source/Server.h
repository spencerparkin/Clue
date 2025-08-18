#pragma once

#include "BoardGraph.h"
#include "Player.h"
#include "Thread.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <vector>
#include <memory>

class Server : public Clue::Thread
{
public:
	Server(int numPlayers, int port);
	virtual ~Server();

	virtual bool Join() override;
	virtual void Run() override;

private:

	int numPlayers;
	int port;
	SOCKET socket;
	Clue::BoardGraph boardGraph;
	std::vector<std::shared_ptr<Player>> playerArray;
	volatile bool shutdownSignaled;
};