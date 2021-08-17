#include "ShadowRaycast2D.h"

ShadowRaycast2D::ShadowRaycast2D()
	: iWorldWidth(32)
	, iWorldHeight(24)
	, m_WorldArr(NULL)
{
}

ShadowRaycast2D::~ShadowRaycast2D()
{
	if (m_WorldArr)
		delete m_WorldArr;

	m_WorldArr = NULL;
}

bool ShadowRaycast2D::Init()
{
	m_WorldArr = new Cell[iWorldWidth * iWorldHeight];

	return true;
}

void ShadowRaycast2D::ConvertTileMapToPolyMap(int iStartX, int iStartY, int w, int h, float fBlockWidth, int pitch)
{
	// Clear "PolyMap"
	vecEdges.clear();

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++)
			for (int j = 0; j < 4; j++)
			{
				m_WorldArr[(y + iStartY) * pitch + (x + iStartX)].edge_exist[j] = false;
				m_WorldArr[(y + iStartY) * pitch + (x + iStartX)].edge_id[j] = 0;
			}

	// Iterate through region from top left to bottom right
	for (int x = 1; x < w - 1; x++)
		for (int y = 1; y < h - 1; y++)
		{
			// Create some convenient indices
			int i = (y + iStartY) * pitch + (x + iStartX);		
			int n = (y + iStartY - 1) * pitch + (x + iStartX);	
			int s = (y + iStartY + 1) * pitch + (x + iStartX);	
			int w = (y + iStartY) * pitch + (x + iStartX - 1);	
			int e = (y + iStartY) * pitch + (x + iStartX + 1);	

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
						vecEdges[m_WorldArr[n].edge_id[WEST]].fEndY += fBlockWidth;
						m_WorldArr[i].edge_id[WEST] = m_WorldArr[n].edge_id[WEST];
						m_WorldArr[i].edge_exist[WEST] = true;
					}
					else
					{
						// Northern neighbour does not have one, so create one
						Edge edge;
						edge.fStartX = (iStartX + x) * fBlockWidth; edge.fStartY = (iStartY + y) * fBlockWidth;
						edge.fStartY = edge.fStartX; edge.fEndY = edge.fStartY + fBlockWidth;

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
						vecEdges[m_WorldArr[n].edge_id[EAST]].fEndY += fBlockWidth;
						m_WorldArr[i].edge_id[EAST] = m_WorldArr[n].edge_id[EAST];
						m_WorldArr[i].edge_exist[EAST] = true;
					}
					else
					{
						// Northern neighbour does not have one, so create one
						Edge edge;
						edge.fStartX = (iStartX + x + 1) * fBlockWidth; edge.fStartY = (iStartY + y) * fBlockWidth;
						edge.fEndY = edge.fStartX; edge.fEndY = edge.fStartY + fBlockWidth;

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
						vecEdges[m_WorldArr[w].edge_id[NORTH]].fEndY += fBlockWidth;
						m_WorldArr[i].edge_id[NORTH] = m_WorldArr[w].edge_id[NORTH];
						m_WorldArr[i].edge_exist[NORTH] = true;
					}
					else
					{
						// Western neighbour does not have one, so create one
						Edge edge;
						edge.fStartX = (iStartX + x) * fBlockWidth; edge.fStartY = (iStartY + y) * fBlockWidth;
						edge.fEndY = edge.fStartX + fBlockWidth; edge.fEndY = edge.fStartY;

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
						vecEdges[m_WorldArr[w].edge_id[SOUTH]].fEndY += fBlockWidth;
						m_WorldArr[i].edge_id[SOUTH] = m_WorldArr[w].edge_id[SOUTH];
						m_WorldArr[i].edge_exist[SOUTH] = true;
					}
					else
					{
						// Western neighbour does not have one, so I need to create one
						Edge edge;
						edge.fStartX = (iStartX + x) * fBlockWidth; edge.fStartY = (iStartY + y + 1) * fBlockWidth;
						edge.fEndY = edge.fStartX + fBlockWidth; edge.fEndY = edge.fStartY;

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
