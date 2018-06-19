#include "EngineH.h"
#include "SDL.h"
#include "GLEW.h"

GraphicsContext EngineH::gc;

EngineH::EngineH()
{

	mWindow = nullptr;
	mGLContext = nullptr;
	mGame = nullptr;
}

EngineH::~EngineH()
{

}

int EngineH::Initialize()
{
	SDL_Init(SDL_INIT_VIDEO);

	const char* szWindowName = mGame->GetWindowName();

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	mWindow = SDL_CreateWindow((szWindowName != nullptr) ? szWindowName : "EngineH",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, kViewportWidth, kViewportHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);

	//exAssert(mWindow != nullptr);

	SDL_GLContext glcontext = SDL_GL_CreateContext(mWindow);

	glewExperimental = GL_TRUE;
	GLenum res = glewInit();

	// glewInit will cause an error, ignore it
	GL_IgnoreError();

	if (res != GLEW_OK)
	{
		// glewGetErrorString( res )
		//exAssert(false);
	}

	// this makes our buffer swap synchronized with the monitor's vertical refresh
	SDL_GL_SetSwapInterval(1);

	return 0;
}

const float SQUARE[] = {
	-1.0f,  1.0f,
	-1.0f, -1.0f,
	1.0f,  1.0f,
	1.0f, -1.0f
};

const float MS2SEC = (1/1000.0f);

void EngineH::Run(exGameInterface* pGame)
{
	// Attaching the engine to a game
	mGame = pGame;

	Initialize();
	InitializeShaders();

	const float fRateRequired = (1 / 60.0f);			// 60 FPS

	unsigned int uLastTicks = SDL_GetTicks();

	while (1)
	{
		unsigned int uNowTicks = SDL_GetTicks();
		unsigned int uFrameTicks = uNowTicks - uLastTicks;

		float fDeltaT = uFrameTicks * MS2SEC;

		if (fDeltaT < fRateRequired)
		{
			// maybe sleep?

			continue;
		}

		OnFrame(fDeltaT);

		uLastTicks = uNowTicks;
	}
}

void EngineH::OnFrame(float fDeltaT)
{
	ConsumeEvents();

	// Getting clear color from the game and clearing all existing renders
	exColor clearColor;
	mGame->GetClearColor(clearColor);

	exColorF clearColorF;
	exColorF::ToColorF(clearColor, clearColorF);

	glClearColor(clearColorF.mColor[0], clearColorF.mColor[1], clearColorF.mColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	mGame->Run(fDeltaT);

	// Rotation attributes for the Box
	gc.mAngle += 1.0f * fDeltaT;
	if (gc.mAngle > 2.0f * M_PI)
		gc.mAngle -= 2.0f * M_PI;


	glUseProgram(gc.mProgram);
	glUniform1f(gc.mUniformAngle, gc.mAngle);
	glBindVertexArray(gc.mVAOPoint);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, countof(SQUARE) / 2);
	glBindVertexArray(0);
	glUseProgram(0);

	SDL_GL_SwapWindow(mWindow);
}

void EngineH::ConsumeEvents()
{
	SDL_PumpEvents();

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			exit(0);
		}

		mGame->OnEvent(&event);
	}

	mGame->OnEventsConsumed();
}

void EngineH::InitializeShaders()
{
	const GLchar *vert_shader =
		"#version 330\n"
		"layout(location = 0) in vec2 point;\n"
		"uniform float angle;\n"
		"void main() {\n"
		"    mat2 rotate = mat2(cos(angle), -sin(angle),\n"
		"                       sin(angle), cos(angle));\n"
		"    gl_Position = vec4(0.75 * rotate * point, 0.0, 1.0);\n"
		"}\n";
	const GLchar *frag_shader =
		"#version 330\n"
		"out vec4 color;\n"
		"void main() {\n"
		"    color = vec4(1.0, 0.15, 0.15, 0);\n"
		"}\n";

	// compile and link OpenGL program
	GLuint vert = CompileShader(GL_VERTEX_SHADER, vert_shader);
	GLuint frag = CompileShader(GL_FRAGMENT_SHADER, frag_shader);

	gc.mProgram = LinkProgram(vert, frag);
	gc.mUniformAngle = glGetUniformLocation(gc.mProgram, "angle");

	glDeleteShader(frag);
	glDeleteShader(vert);

	// prepare vertex buffer object (VBO)
	glGenBuffers(1, &gc.mVBOPoint);
	glBindBuffer(GL_ARRAY_BUFFER, gc.mVBOPoint);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SQUARE), SQUARE, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// prepare vertex array object (VAO)
	glGenVertexArrays(1, &gc.mVAOPoint);
	glBindVertexArray(gc.mVAOPoint);
	glBindBuffer(GL_ARRAY_BUFFER, gc.mVAOPoint);
	glVertexAttribPointer(ATTRIB_POINT, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ATTRIB_POINT);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint EngineH::CompileShader(GLenum eShaderType, const GLchar * pSource)
{
	GLuint shader = glCreateShader(eShaderType);
	glShaderSource(shader, 1, &pSource, NULL);
	glCompileShader(shader);

	GLint param;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &param);

	if (!param)
	{
		GLchar log[4096];
		glGetShaderInfoLog(shader, sizeof(log), NULL, log);
		//printf("error: %s: %s\n", (eShaderType == GL_FRAGMENT_SHADER) ? "frag" : "vert", (char*)log);
		//exAssert(false);
	}

	return shader;
}


GLuint EngineH::LinkProgram(GLuint gluVertexShader, GLuint gluFragmentShader)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, gluVertexShader);
	glAttachShader(program, gluFragmentShader);
	glLinkProgram(program);

	GLint param;
	glGetProgramiv(program, GL_LINK_STATUS, &param);

	if (!param)
	{
		GLchar log[4096];
		glGetProgramInfoLog(program, sizeof(log), NULL, log);
		//printf("error: link: %s\n", (char*)log);
		//exAssert(false);
	}

	return program;
}

void EngineH::GL_IgnoreError()
{
	glGetError();
}


void EngineH::DrawBox(const exVector2& v2P1, const exVector2& v2P2, const exColor& color, int nLayer)
{

}

void EngineH::DrawLine(const exVector2& v2P1, const exVector2& v2P2, const exColor& color, int nLayer)
{

}


void EngineH::DrawLineBox(const exVector2& v2P1, const exVector2& v2P2, const exColor& color, int nLayer)
{

}

void EngineH::DrawCircle(const exVector2& v2Center, float fRadius, const exColor& color, int nLayer)
{

}

void EngineH::DrawLineCircle(const exVector2& v2Center, float fRadius, const exColor& color, int nLayer)
{

}

int	EngineH::LoadFont(const char* szFile, int nPTSize)
{
	return -1;
}

void EngineH::DrawText(int nFontID, const exVector2& v2Position, const char* szText, const exColor& color, int nLayer)
{

}