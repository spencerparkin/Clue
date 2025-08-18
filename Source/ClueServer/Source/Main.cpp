#include "Main.h"
#include "Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "Please pass in the number of players.");
		return -1;
	}

	int numPlayers = ::atoi(argv[1]);
	if (!(2 <= numPlayers && numPlayers <= 6))
	{
		fprintf(stderr, "There can only be 2 to 6 players in the game of Clue.");
		return -1;
	}

	std::shared_ptr<Server> server = std::make_shared<Server>(numPlayers);
	if (!server->Split())
		return -1;

	while (server->IsRunning())
	{
		int key = ::_getch();
		if (key == 'x')
			break;
	}

	server->Join();

	return 0;
}