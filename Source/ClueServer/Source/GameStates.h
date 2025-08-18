#pragma once

#include "Server.h"

class SetupGameState : public GameState
{
public:
	SetupGameState();
	virtual ~SetupGameState();

	virtual bool Run(Server::GameData* gameData, std::shared_ptr<GameState>& nextState) override;
};