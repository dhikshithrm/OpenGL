#pragma once
#include<GL/glew.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCALL(x) GlClearError();\
	x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
void GlClearError();
static bool GLLogCall(const char* function, const char* file, int line);