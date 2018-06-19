
#include <windows.h>
#include "EngineH.h"
#include "GameInterface.h"

class exGame : public exGameInterface
{
public:
	virtual void				Initialize(exEngineInterface* pEngine) {}
	virtual const char*			GetWindowName() const { return "HARISH"; }
	virtual void				GetClearColor(exColor& color) const {}
	virtual void				OnEvent(SDL_Event* pEvent) {}
	virtual void				OnEventsConsumed() {}
	virtual void				Run(float fDeltaT) {}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	exGame game;

	EngineH engine;
	engine.Run(&game);

	return 0;
}