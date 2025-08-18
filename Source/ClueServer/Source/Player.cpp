#include "Player.h"

Player::Player(SOCKET connectedSocket) : packetThread(connectedSocket)
{
}

/*virtual*/ Player::~Player()
{
}

bool Player::Initialize()
{
	if (!this->packetThread.Split())
		return false;

	return true;
}

bool Player::Shutdown()
{
	if (!this->packetThread.Join())
		return false;

	return true;
}