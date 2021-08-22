#pragma once

#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

#include <GLFW/glfw3.h>

#include "RenderControl/ShaderManager.h"

#include "DesignPatterns/SingletonTemplate.h"

#include <string>

class CLevelGrid : public CSingletonTemplate<CLevelGrid>
{
	friend CSingletonTemplate<CLevelGrid>;
public:

	unsigned int iWorldWidth;
	unsigned int iWorldHeight;

	CLevelGrid();
	~CLevelGrid();

	void Init(unsigned int iWorldWidth, unsigned int iWorldHeight);

	void PreRender(void);
	void Render(void);
	void PostRender(void);

	void SetLineShader(const std::string& name);

protected:

	std::string sLineShaderName;
	unsigned VAO, VBO, EBO;

	float fLineWidth;

	glm::mat4 transform;
	glm::vec2 vec2Dimensions;
	glm::vec4 vec4Colour;

};