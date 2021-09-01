#include "Renderer.h"
#include <iostream>

void GlClearError()
{
	while (glGetError() != GL_NO_ERROR);
}
static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL error] (" << error << "): " << function <<
			" " << file << ":" << line << std::endl;
		return false;
	}

	return true;
}
