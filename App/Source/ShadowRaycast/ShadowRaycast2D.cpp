#include "ShadowRaycast2D.h"
#include "../Scene2D/Map2D.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

#include <GLFW/glfw3.h>
#include "GameControl/Settings.h"

ShadowRaycast2D::ShadowRaycast2D(void)
	: iWorldWidth(32)
	, iWorldHeight(24)
	, fLineWidth(0.5f)
	, vec4Colour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f))
	, transform(1.f)
	, m_WorldArr(NULL)
{
	sLineShaderName = "LineShader";
}

ShadowRaycast2D::~ShadowRaycast2D(void)
{
	if (m_WorldArr)
		delete m_WorldArr;

	m_WorldArr = NULL;
}

void ShadowRaycast2D::GenerateLineVAO(glm::vec2 vStart, glm::vec2 vEnd)
{
	glm::vec2 vStartUVCoord;
	glm::vec2 vEndUVCoord;

	vStartUVCoord = CSettings::GetInstance()->ConvertIndexToUVSpace(vStart);
	vEndUVCoord = CSettings::GetInstance()->ConvertIndexToUVSpace(vEnd);

	float vertices[] = {
		-vStartUVCoord.x, -vStartUVCoord.y, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vEndUVCoord.x, -vEndUVCoord.y, vec4Colour.x, vec4Colour.y, vec4Colour.z,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glLineWidth(fLineWidth);
}

bool ShadowRaycast2D::Init()
{
	CMap2D* cMap = CMap2D::GetInstance();
	std::vector<CObject2D> objArr = cMap->GetMap();

	iWorldWidth = cMap->GetLevelCol();
	iWorldHeight = cMap->GetLevelRow();

	m_WorldArr = new Cell[iWorldWidth * iWorldHeight];

	for (int i = 0; i < objArr.size(); ++i)
	{
		int index = objArr[i].getIndexSpace().x + objArr[i].getIndexSpace().y * cMap->GetLevelRow();
		m_WorldArr[index].exist = true;
	}

	ConvertTileMapToPolyMap(0, 0, iWorldWidth, iWorldHeight, 1, iWorldWidth);

	return true;
}

void ShadowRaycast2D::PreRender(void)
{
	// Use the shader defined for this class
	CShaderManager::GetInstance()->Use(sLineShaderName);
}

void ShadowRaycast2D::Render(void)
{
	for (auto& e : vecEdges)
	{
		GenerateLineVAO(glm::vec2(e.sx, e.sy), glm::vec2(e.ex, e.ey));

		transform = glm::mat4(1.f);
		CShaderManager::GetInstance()->activeShader->setMat4("transform", transform);

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 2);
	}
}

void ShadowRaycast2D::PostRender(void)
{
}

void ShadowRaycast2D::ConvertTileMapToPolyMap(int sx, int sy, int w, int h, float fBlockWidth, int pitch)
{
	// Clear "PolyMap"
	vecEdges.clear();

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++)
			for (int j = 0; j < 4; j++)
			{
				m_WorldArr[(y + sy) * pitch + (x + sx)].edge_exist[j] = false;
				m_WorldArr[(y + sy) * pitch + (x + sx)].edge_id[j] = 0;
			}

	// Iterate through region from top left to bottom right
	for (int x = 1; x < w - 1; x++)
		for (int y = 1; y < h - 1; y++)
		{
			// Create some convenient indices
			int i = (y + sy) * pitch + (x + sx);			// This
			int n = (y + sy - 1) * pitch + (x + sx);		// Northern Neighbour
			int s = (y + sy + 1) * pitch + (x + sx);		// Southern Neighbour
			int w = (y + sy) * pitch + (x + sx - 1);	// Western Neighbour
			int e = (y + sy) * pitch + (x + sx + 1);	// Eastern Neighbour

			// If this cell exists, check if it needs edges
			if (m_WorldArr[i].exist)
			{
				// If this cell has no western neighbour, it needs a western edge
				if (!m_WorldArr[w].exist)
				{
					// It can either extend it from its northern neighbour if they have
					// one, or It can start a new one.
					if (m_WorldArr[n].edge_exist[WEST])
					{
						// Northern neighbour has a western edge, so grow it downwards
						vecEdges[m_WorldArr[n].edge_id[WEST]].ey += fBlockWidth;
						m_WorldArr[i].edge_id[WEST] = m_WorldArr[n].edge_id[WEST];
						m_WorldArr[i].edge_exist[WEST] = true;
					}
					else
					{
						// Northern neighbour does not have one, so create one
						Edge edge;
						edge.sx = (sx + x) * fBlockWidth; edge.sy = (sy + y) * fBlockWidth;
						edge.ex = edge.sx; edge.ey = edge.sy + fBlockWidth;

						// Add edge to Polygon Pool
						int edge_id = vecEdges.size();
						vecEdges.push_back(edge);

						// Update tile information with edge information
						m_WorldArr[i].edge_id[WEST] = edge_id;
						m_WorldArr[i].edge_exist[WEST] = true;
					}
				}

				// If this cell dont have an eastern neignbour, It needs a eastern edge
				if (!m_WorldArr[e].exist)
				{
					// It can either extend it from its northern neighbour if they have
					// one, or It can start a new one.
					if (m_WorldArr[n].edge_exist[EAST])
					{
						// Northern neighbour has one, so grow it downwards
						vecEdges[m_WorldArr[n].edge_id[EAST]].ey += fBlockWidth;
						m_WorldArr[i].edge_id[EAST] = m_WorldArr[n].edge_id[EAST];
						m_WorldArr[i].edge_exist[EAST] = true;
					}
					else
					{
						// Northern neighbour does not have one, so create one
						Edge edge;
						edge.sx = (sx + x + 1) * fBlockWidth; edge.sy = (sy + y) * fBlockWidth;
						edge.ex = edge.sx; edge.ey = edge.sy + fBlockWidth;

						// Add edge to Polygon Pool
						int edge_id = vecEdges.size();
						vecEdges.push_back(edge);

						// Update tile information with edge information
						m_WorldArr[i].edge_id[EAST] = edge_id;
						m_WorldArr[i].edge_exist[EAST] = true;
					}
				}

				// If this cell doesnt have a northern neignbour, It needs a northern edge
				if (!m_WorldArr[n].exist)
				{
					// It can either extend it from its western neighbour if they have
					// one, or It can start a new one.
					if (m_WorldArr[w].edge_exist[NORTH])
					{
						// Western neighbour has one, so grow it eastwards
						vecEdges[m_WorldArr[w].edge_id[NORTH]].ex += fBlockWidth;
						m_WorldArr[i].edge_id[NORTH] = m_WorldArr[w].edge_id[NORTH];
						m_WorldArr[i].edge_exist[NORTH] = true;
					}
					else
					{
						// Western neighbour does not have one, so create one
						Edge edge;
						edge.sx = (sx + x) * fBlockWidth; edge.sy = (sy + y) * fBlockWidth;
						edge.ex = edge.sx + fBlockWidth; edge.ey = edge.sy;

						// Add edge to Polygon Pool
						int edge_id = vecEdges.size();
						vecEdges.push_back(edge);

						// Update tile information with edge information
						m_WorldArr[i].edge_id[NORTH] = edge_id;
						m_WorldArr[i].edge_exist[NORTH] = true;
					}
				}

				// If this cell doesnt have a southern neignbour, It needs a southern edge
				if (!m_WorldArr[s].exist)
				{
					// It can either extend it from its western neighbour if they have
					// one, or It can start a new one.
					if (m_WorldArr[w].edge_exist[SOUTH])
					{
						// Western neighbour has one, so grow it eastwards
						vecEdges[m_WorldArr[w].edge_id[SOUTH]].ex += fBlockWidth;
						m_WorldArr[i].edge_id[SOUTH] = m_WorldArr[w].edge_id[SOUTH];
						m_WorldArr[i].edge_exist[SOUTH] = true;
					}
					else
					{
						// Western neighbour does not have one, so I need to create one
						Edge edge;
						edge.sx = (sx + x) * fBlockWidth; edge.sy = (sy + y + 1) * fBlockWidth;
						edge.ex = edge.sx + fBlockWidth; edge.ey = edge.sy;

						// Add edge to Polygon Pool
						int edge_id = vecEdges.size();
						vecEdges.push_back(edge);

						// Update tile information with edge information
						m_WorldArr[i].edge_id[SOUTH] = edge_id;
						m_WorldArr[i].edge_exist[SOUTH] = true;
					}
				}

			}

		}
}
