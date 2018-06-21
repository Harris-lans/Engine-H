#pragma once

#include <string>

class Console
{
public:
	static void Log(const char* text);
	static void LogString(std::string text);
	static void LogOpenGL(unsigned int error);

	// Templating the log function
	template <typename T>
	static void LogType(T text)
	{
		std::string temp(std::to_string(text));
		const char* finalChar = temp.c_str();
		Log(finalChar);
	}
};
