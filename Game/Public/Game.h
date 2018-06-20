#include "GameInterface.h"
#include "EngineInterface.h"

class exGame : public exGameInterface
{
public:
	exGame();
	~exGame();

	virtual void				Initialize(exEngineInterface* pEngine) override;
	virtual const char*			GetWindowName() const override;
	virtual void				GetClearColor(exColor& color) const override;
	virtual void				OnEvent(SDL_Event* pEvent) override;
	virtual void				OnEventsConsumed() override;
	virtual void				Run(float fDeltaT) override;

private:
	exEngineInterface *			mEngine;

	exColor						mBoxColor;
	exVector2					mBoxVertex1;
	exVector2					mBoxVertex2;
};
