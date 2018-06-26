#include "Game.h"

exGame::exGame()
{
	// Nothing to do here for now
}

exGame::~exGame()
{
	// Nothing to do here for now
}

void exGame::Initialize(exEngineInterface* pEngine)
{
	mEngine = pEngine;

	mBoxColor.SetColor(255, 0, 0);
	mBoxVertex1 = {0, 0};
	mBoxVertex2 = {(float)kViewportWidth, (float)kViewportHeight};
}

const char* exGame::GetWindowName() const
{
	return "Test Game";
}

void exGame::GetClearColor(exColor& color) const
{
	exColor clearColor;
	clearColor.SetColor(128, 128, 128);

	color = clearColor;
}

void exGame::OnEvent(SDL_Event* pEvent)
{

}

void exGame::OnEventsConsumed()
{

}

void exGame::Run(float fDeltaT)
{
	mEngine->DrawBox(mBoxVertex1, mBoxVertex2, mBoxColor, 1);
	
	// Drawing a circle
	mBoxColor.SetColor(0, 255, 0);
	//mEngine->DrawCircle(mBoxVertex1, 40.0f, mBoxColor, 2);
}

