#include "EngineH.h"
#include "SDL.h"
#include "GLEW.h"
#include "Output.h"

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
	
	// Normalizing Colors
	exColorF clearColorF;
	exColorF::ToColorF(clearColor, clearColorF);

	glClearColor(clearColorF.mColor[0], clearColorF.mColor[1], clearColorF.mColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Running the game
	mGame->Run(fDeltaT);

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
	InitializeSquareShaders();
	InitializeCircleShaders();

	// Printing the number of errors detected in the OpenGL code
	Console::LogOpenGL(glGetError());
}

void EngineH::InitializeSquareShaders()
{
	// Testing Depth
	glEnable(GL_DEPTH_TEST);

	const GLchar *vert_shader =
		"#version 330\n"
		"layout(location = 0) in vec2 point;\n"
		"uniform mat4 model, view, proj;\n"
		"void main() {\n"
		"    gl_Position = proj * view * model * vec4(point, 0.0, 1.0);\n"
		"}\n";
	const GLchar *frag_shader =
		"#version 330\n"
		"layout(location = 0) out vec4 color;\n"
		"uniform vec3 in_color;\n"
		"void main() {\n"
		"    color = vec4(in_color, 0);\n"
		"}\n";

	// compile and link OpenGL program
	GLuint vert = CompileShader(GL_VERTEX_SHADER, vert_shader);
	GLuint frag = CompileShader(GL_FRAGMENT_SHADER, frag_shader);

	// Storing the compiled shader of the circle in the graphics context
	gc.mBoxShaderProgram = LinkProgram(vert, frag);

	glDeleteShader(frag);
	glDeleteShader(vert);
}

void EngineH::InitializeCircleShaders()
{
	// For drawing a circle we are first drawing a square and then removing all pixels outside the distance(radius) from the center

	// Testing Depth
	glEnable(GL_DEPTH_TEST);

	const GLchar *vert_shader =
		"#version 330\n"
		"layout(location = 0) in vec2 point;\n"
		"layout(location = 1) in vec2 tex;\n"
		"uniform mat4 model, view, proj;\n"
		"out vec2 CircleTexCoords;"
		"void main() {\n"
		"     gl_Position = proj * view * model * vec4(point, 0.0, 1.0);\n"
		"     CircleTexCoords = tex;"
		"}\n";
	const GLchar *frag_shader =
		"#version 330\n"
		"layout(location = 0) out vec4 color;\n"
		"uniform vec3 in_color;\n"
		"in vec2 CircleTexCoords;\n"
		"void main() {\n"
		"float d = distance(CircleTexCoords, vec2(0.0, 0.0));\n"
		"if (d > 1.0)\n"
		"{\n"
		"discard;\n"
		"}\n"
		"color = vec4(in_color, 1.0);\n"
		"}\n";

	// compile and link OpenGL program
	GLuint vert = CompileShader(GL_VERTEX_SHADER, vert_shader);
	GLuint frag = CompileShader(GL_FRAGMENT_SHADER, frag_shader);

	// Storing the compiled shader of the circle in the graphics context
	gc.mCircleShaderProgram = LinkProgram(vert, frag);

	glDeleteShader(frag);
	glDeleteShader(vert);

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
	float width = (v2P2.x - v2P1.x) / 2;
	float height = (v2P2.y - v2P1.y) / 2;

	// Generating the coordinates for the box depending on the given two points
	float SQUARE[8] = {
		-width,  height,
		-width, -height,
		 width,  height,
		 width, -height
	};

	// Preparing the vertex Buffer to draw a square

	// Prepare vertex buffer object (VBO)
	// =================================
	// Generating 1 Buffer and storing it's context
	glGenBuffers(1, &gc.mVBOPoint);
	glBindBuffer(GL_ARRAY_BUFFER, gc.mVBOPoint);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SQUARE), SQUARE, GL_STATIC_DRAW);
	// Resetting OpenGL's selected Buffer, so that this is not the buffer being used when something is drawn (Good Practices)
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Prepare vertex array object (VAO)
	// =================================
	// Generating 1 Vertex Array and storing it's context
	glGenVertexArrays(1, &gc.mVAOPoint);
	glBindVertexArray(gc.mVAOPoint);
	// Linking the Vertex Array to a Array Buffer
	glBindBuffer(GL_ARRAY_BUFFER, gc.mVAOPoint);
	glVertexAttribPointer(ATTRIB_POINT, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ATTRIB_POINT);
	// Resetting OpenGL's selected Buffer and Vertex Array, so that this is not the buffer or vertex array being used when something is drawn (Good Practices)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	DrawUsingShaderProgram(gc.mBoxShaderProgram, color, nLayer, countof(SQUARE)/2);
}

void EngineH::DrawLine(const exVector2& v2P1, const exVector2& v2P2, const exColor& color, int nLayer)
{

}


void EngineH::DrawLineBox(const exVector2& v2P1, const exVector2& v2P2, const exColor& color, int nLayer)
{

}

void EngineH::DrawCircle(const exVector2& v2Center, float fRadius, const exColor& color, int nLayer)
{
	// Generating the coordinates for the circle depending on the given two points
	float CIRCLE[16] = {
		-fRadius,  fRadius, -1.0f,  1.0f,
		-fRadius, -fRadius, -1.0f, -1.0f,
		 fRadius,  fRadius,  1.0f,  1.0f,
		 fRadius, -fRadius,  1.0f, -1.0f
	};

	// Preparing the vertex Buffer to draw a circle

	// Prepare vertex buffer object (VBO)
	// =================================
	// Generating 1 Buffer and storing it's context
	glGenBuffers(1, &gc.mVBOPoint);
	glBindBuffer(GL_ARRAY_BUFFER, gc.mVBOPoint);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CIRCLE), CIRCLE, GL_STATIC_DRAW);
	// Resetting OpenGL's selected Buffer, so that this is not the buffer being used when something is drawn (Good Practices)
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Prepare vertex array object (VAO)
	// =================================
	// Generating 1 Vertex Array and storing it's context
	glGenVertexArrays(1, &gc.mVAOPoint);
	glBindVertexArray(gc.mVAOPoint);
	// Linking the Vertex Array to a Array Buffer
	glBindBuffer(GL_ARRAY_BUFFER, gc.mVAOPoint);
	glVertexAttribPointer(ATTRIB_POINT, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ATTRIB_POINT);
	// Resetting OpenGL's selected Buffer and Vertex Array, so that this is not the buffer or vertex array being used when something is drawn (Good Practices)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	DrawUsingShaderProgram(gc.mCircleShaderProgram, color, nLayer, countof(CIRCLE)/2);
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

void EngineH::DrawUsingShaderProgram(GLuint shaderProgram, const exColor& color, int nLayer, int numberOfVertices)
{
	// The three matrices we need
	exMatrix4 orthographicProjection;
	exMatrix4 model;
	exMatrix4 view;

	// Projection matrix
	exMatrix4::exOrthographicProjectionMatrix(&orthographicProjection, 800.0f, 600.0f, -100.0f, 100.0f);

	// View matrix (really just an identity matrix, maybe optimize this call?)
	exMatrix4::exMakeTranslationMatrix(&view, exVector2(0.0f, 0.0f));

	// Positions of the uniforms
	int view_mat_location;
	int proj_mat_location;
	int model_mat_location;

	glUseProgram(shaderProgram);

	exColorF colorf;
	exColorF::ToColorF(color, colorf);
	int in_color_vec3_location;
	in_color_vec3_location = glGetUniformLocation(shaderProgram, "in_color");
	glUniform3fv(in_color_vec3_location, 1, &colorf.mColor[0]);

	// Position of the object
	exMatrix4::exMakeTranslationMatrix(&model, exVector2(50.0f, 50.0f));
	// This is the "layer", cast from int to float
	model.m43 = (float)nLayer;

	view_mat_location = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.ToFloatPtr());
	proj_mat_location = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, orthographicProjection.ToFloatPtr());
	model_mat_location = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model.ToFloatPtr());

	glBindVertexArray(gc.mVAOPoint);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, numberOfVertices);
	glBindVertexArray(0);
	glUseProgram(0);
}

