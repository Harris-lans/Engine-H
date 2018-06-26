#pragma once

#include "EngineInterface.h"
#include "GameInterface.h"
#include "EngineTypes.h"

// Forward declaring classes, types and structs in use 
struct SDL_Window;
typedef void* SDL_GLContext;
typedef	int GLint;
typedef char GLchar;
typedef unsigned int GLuint;
typedef unsigned int GLenum;

#define ATTRIB_POINT_1 0
#define ATTRIB_POINT_2 1
#define countof(x) (sizeof(x) / sizeof(0[x]))

struct GraphicsContext
{
	GLuint mBoxShaderProgram;
	GLuint mCircleShaderProgram;
	GLint mUniformAngle;
	GLuint mVBOPoint;
	GLuint mVAOPoint;
	GLuint mVBOPoint_Circle;
	GLuint mVAOPoint_Circle;
	float mAngle;
};

enum class BUFFER_INDEX : GLuint
{
	BOX = 0,
	LINE_BOX,
	CIRCLE,
	LINE_CIRCLE,
	LINE
};

// Provide an interface to any engine
class EngineH : public exEngineInterface
{
public:
	EngineH();
	~EngineH();

	// Causes all initialization to occur and the main loop to start 
	virtual void				Run(exGameInterface* pGameInterface);

	// Draw a line
	virtual void				DrawLine(const exVector2& v2P1, const exVector2& v2P2, const exColor& color, int nLayer);

	// draw a filled box
	virtual void				DrawBox(const exVector2& v2P1, const exVector2& v2P2, const exColor& color, int nLayer);

	// draw a box outline
	virtual void				DrawLineBox(const exVector2& v2P1, const exVector2& v2P2, const exColor& color, int nLayer);

	// draw a filled circle
	virtual void				DrawCircle(const exVector2& v2Center, float fRadius, const exColor& color, int nLayer);

	// draw a circle outline
	virtual void				DrawLineCircle(const exVector2& v2Center, float fRadius, const exColor& color, int nLayer);

	// load a font, >= 0 upon success, negative upon failure
	virtual int					LoadFont(const char* szFile, int nPTSize);

	// draw text with a given loaded font
	virtual void				DrawText(int nFontID, const exVector2& v2Position, const char* szText, const exColor& color, int nLayer);

	virtual void				DrawUsingShaderProgram(GLuint shaderProgram, GLuint vertexArrayObject, const exVector2& position, const exColor& color, int nLayer, int numberOfVertices);

private:
	// Class Functions
	int Initialize();

	void OnFrame(float fDeltaT);

	void ConsumeEvents();

	void InitializeShaders();

	void InitializeSquareShaders();

	void InitializeCircleShaders();

	static GLuint CompileShader( GLenum eShaderType, const GLchar* pSource );

	static GLuint LinkProgram( GLuint gluVertexShader, GLuint gluFragmentShader );

	void GL_IgnoreError();

private:

	SDL_Window * mWindow;												// It serves as a canvas to out put what is drawn by the GPU
	SDL_GLContext * mGLContext;											// Tracks the contexts of the things this specific instance of the Engine draws 
	exGameInterface* mGame;

	static GraphicsContext gc;
};


