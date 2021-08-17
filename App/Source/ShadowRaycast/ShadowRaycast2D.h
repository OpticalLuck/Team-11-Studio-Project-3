#pragma once
#include <vector>

struct Edge
{
	float fStartX, fStartY;
	float fEndX, fEndY;
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

class ShadowRaycast2D
{
public:
	
	ShadowRaycast2D();
	~ShadowRaycast2D();

	bool Init();

protected:
	Cell* m_WorldArr;
	int iWorldWidth;
	int iWorldHeight;

	std::vector<Edge> vecEdges;

	void ConvertTileMapToPolyMap(int iStartX, int iStartY, int w, int h, float fBlockWidth, int pitch);

	void Raycast();

};