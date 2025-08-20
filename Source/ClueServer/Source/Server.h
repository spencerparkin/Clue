#pragma once

#include "BoardGraph.h"
#include "Player.h"
#include "Thread.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <vector>
#include <memory>

class GameState;

/**
 * 
 */
class Server : public Clue::Thread
{
public:
	Server(int numPlayers, int port);
	virtual ~Server();

	virtual bool Join() override;
	virtual void Run() override;

	struct GameData
	{
		int whoseTurn;
		int presentDiceRoll;
		Clue::Accusation correctAccusation;
		Clue::BoardGraph boardGraph;
		std::vector<std::shared_ptr<Player>> playerArray;
	};

	GameData* GetGameData();

private:

	std::shared_ptr<GameState> currentState;
	GameData gameData;
	int numPlayers;
	int port;
	SOCKET socket;
	volatile bool shutdownSignaled;
};

/**
 * 
 */
class GameState
{
public:
	GameState();
	virtual ~GameState();

	enum Result
	{
		ContinueState,
		LeaveState,
		HaltMachine
	};

	virtual Result Run(Server::GameData* gameData, std::shared_ptr<GameState>& nextState) = 0;
};