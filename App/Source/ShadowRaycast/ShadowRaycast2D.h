#pragma once
#include <vector>
#include <string>
#include "DesignPatterns/SingletonTemplate.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

struct Edge
{
	float sx, sy;
	float ex, ey;
};

struct Cell
{
	int edge_id[4];
	bool edge_exist[4];
	bool exist = false;
};

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3

class ShadowRaycast2D : public CSingletonTemplate<ShadowRaycast2D>
{
	friend CSingletonTemplate<ShadowRaycast2D>;
public:
	glm::mat4 transform;

	float fLineWidth;

	// Bounding Box colour
	glm::vec4 vec4Colour;

	std::vector<Edge> vecEdges;
	Cell* m_WorldArr;
	int iWorldWidth;
	int iWorldHeight;

	bool Init();

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

protected:
	std::string sLineShaderName;
	unsigned int VAO, VBO;

	ShadowRaycast2D(void);
	~ShadowRaycast2D(void);

	void GenerateLineVAO(glm::vec2 vStart, glm::vec2 vEnd);

	void ConvertTileMapToPolyMap(int iStartX, int iStartY, int w, int h, float fBlockWidth, int pitch);
	void Raycast();

};