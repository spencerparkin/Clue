#include "Server.h"
#include "GameStates.h"
#include <stdio.h>
#include <format>

using namespace Clue;

Server::Server(int numPlayers, int port)
{
	this->numPlayers = numPlayers;
	this->port = port;
	this->socket = INVALID_SOCKET;
	this->shutdownSignaled = false;
}

/*virtual*/ Server::~Server()
{
}

/*virtual*/ bool Server::Join()
{
	if (this->socket != INVALID_SOCKET)
	{
		::closesocket(this->socket);
		this->socket = INVALID_SOCKET;
	}

	this->shutdownSignaled = true;

	return Thread::Join();
}

/*virtual*/ void Server::Run()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA startupData;
	if (::WSAStartup(version, &startupData) != 0)
		return;

	addrinfo hints;
	::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	char portStr[32];
	sprintf_s(portStr, sizeof(portStr), "%d", this->port);
	addrinfo* addressInfo = nullptr;
	int result = ::getaddrinfo("127.0.0.1", portStr, &hints, &addressInfo);
	if (result != 0)
		return;

	if (addressInfo->ai_protocol != IPPROTO_TCP)
		return;

	this->socket = ::socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol);
	if (socket == INVALID_SOCKET)
		return;

	result = ::bind(this->socket, addressInfo->ai_addr, (int)addressInfo->ai_addrlen);
	if (result == SOCKET_ERROR)
		return;

	freeaddrinfo(addressInfo);

	result = ::listen(this->socket, this->numPlayers);
	if (result == SOCKET_ERROR)
		return;

	while (this->gameData.playerArray.size() < this->numPlayers)
	{
		printf("Waiting for player %d of %d to connect...\n", int(this->gameData.playerArray.size()) + 1, this->numPlayers);

		SOCKET connectedSocket = ::accept(this->socket, nullptr, nullptr);
		if (connectedSocket == INVALID_SOCKET)
			return;

		std::shared_ptr<Player> player = std::make_shared<Player>(connectedSocket);
		this->gameData.playerArray.push_back(player);
	}

	for (std::shared_ptr<Player>& player : this->gameData.playerArray)
		player->Setup();

	// The server runs as a simple state machine.
	this->currentState = std::make_shared<SetupGameState>();
	while (!this->shutdownSignaled)
	{
		if (!this->currentState.get())
			break;

		std::shared_ptr<GameState> nextState;
		if (this->currentState->Run(&this->gameData, nextState))
			this->currentState = nextState;
	}

	for (std::shared_ptr<Player>& player : this->gameData.playerArray)
		player->Shutdown();

	::WSACleanup();
}

Server::GameData* Server::GetGameData()
{
	return &this->gameData;
}

GameState::GameState()
{
}

/*virtual*/ GameState::~GameState()
{
}