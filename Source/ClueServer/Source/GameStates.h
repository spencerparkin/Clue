#pragma once

#include "Server.h"

/**
 * 
 */
class SetupGameState : public GameState
{
public:
	SetupGameState();
	virtual ~SetupGameState();

	virtual Result Run(Server::GameData* gameData, std::shared_ptr<GameState>& nextState) override;
};

/**
 * 
 */
class RollForPlayerState : public GameState
{
public:
	RollForPlayerState();
	virtual ~RollForPlayerState();

	virtual Result Run(Server::GameData* gameData, std::shared_ptr<GameState>& nextState) override;
};

/**
 * 
 */
class WaitForPlayerTravelState : public GameState
{
public:
	WaitForPlayerTravelState();
	virtual ~WaitForPlayerTravelState();

	virtual Result Run(Server::GameData* gameData, std::shared_ptr<GameState>& nextState) override;
};