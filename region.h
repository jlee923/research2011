#ifndef REGION_H_
#define REGION_H_
#include "global.h"
#include "vertex.h"
#include "sector.h"

#define _DRAW_HEIGHT 1.7
#define _DRAW_WIDTH 1.8

extern int oper;

class Region {
public:
	Region();
	// regions == genus, trans = transformations
	Region(int regions, vector<vec2> trans);

	void merge(Region rightRegion);
	
	void draw(vec2 trans); // draw the net given a transformation matrix
	map<unsigned int, Sector*> getSectors() {return sectors;}
	vector<HalfEdge*> getHalfEdges();
	// later just want to return vertices
	vector<Vertex*> getVertices();

	void addSector(Sector * s);
	void removeSector(Sector * s);
protected:
	map<unsigned int,Sector*> sectors;
	map<unsigned int,Vertex*> vertices;
	double cy1;
	double cy2;
	unsigned int sectorCount;

	

};
#endif /* MAP_H_ */