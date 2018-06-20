
#include <windows.h>
#include "EngineH.h"
#include "Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	EngineH engine;
	exGame game;

	// Initializing the game
	game.Initialize(&engine);

	// Running the game
	engine.Run(&game);

	return 0;
}