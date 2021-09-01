#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	
	try
	{
		std::ifstream stream(filepath);
	

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}

}
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result==GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile"<< (type == GL_VERTEX_SHADER?"vertex":"fragment")<<" shader!"<<std::endl;

		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}


	return id;
}

static unsigned int CreateShader(const std::string&vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
		std::cout << "ERROR!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[] = {
	   -0.5f, -0.5f,	
	    0.5f, -0.5f,
		0.5f,  0.5f,
	   -0.5f,  0.5f,
	};

	unsigned int indices[] = {
		0,1,2,
		2,3,0,
	};
	unsigned int vao;
	GLCALL(glGenVertexArrays(1,&vao));
	GLCALL(glBindVertexArray(vao));


	VertexBuffer vb(positions, 4*2*sizeof(float));
	

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	IndexBuffer ib(indices, 6);

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	std::cout << "VERTEX" << std::endl;
	std::cout << source.VertexSource << std::endl;
	std::cout << "FRAGMENT" << std::endl;
	std::cout << source.FragmentSource << std::endl;


	unsigned int shader = CreateShader(source.VertexSource,source.FragmentSource);
	glUseProgram(shader);

	GLCALL(int location = glGetUniformLocation(shader, "u_Color"));
	GLCALL(ASSERT(location != -1));
	GLCALL(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

	glfwSwapInterval(1);

	float r = 0.0f;
	float increment = 0.05f;

	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/*Loop until the user closes window*/
	while (!glfwWindowShouldClose(window))
	{ 		
		glClear(GL_COLOR_BUFFER_BIT);  

		GLCALL(glUseProgram(shader));
		GLCALL(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

		GLCALL(glBindVertexArray(vao));
		//GLCALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		//GLCALL(glEnableVertexAttribArray(0));
		//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

		ib.Bind();

		GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		
		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;

		r += increment;
		/*Swap front and back buffers*/
		glfwSwapBuffers(window);
		/*Poll for and process events*/
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}