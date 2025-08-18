#include "Main.h"
#include "Player.h"
#include <stdio.h>
#include <string>
#include <conio.h>

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "Please provide an IP address as the first argument, and a port as the second.");
		return -1;
	}

	std::string ipAddr(argv[1]);
	int port = ::atoi(argv[2]);

	Player player(ipAddr, port);
	if (!player.Split())
		return -1;

	printf("Clue bot running.\n");
	printf("Press 'x' to exit.\n");
	while (player.IsRunning())
	{
		int key = ::_getch();
		if (key == 'x')
			break;
	}

	player.Join();
	return 0;
}