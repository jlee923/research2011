/*
 * The Group Class:
 * One level below the main environment.
 *
 */

#ifndef GROUP_H_
#define GROUP_H_
#include "global.h"
#include "region.h"


extern int oper;

class Group {
public:
	Group();
	Group(unsigned int genus);
	void draw();
	map<unsigned int,Sector*> getSectors() { return baseRegion.getSectors(); }
	vector<HalfEdge*> getHalfEdges() { return baseRegion.getHalfEdges(); }
	void addSector(Sector * s) { baseRegion.addSector(s); }
	void removeSector(Sector * s) { baseRegion.removeSector(s); }
	
	vec2 translateClick(vec2* clickPoint, vec2* nextRegion);

protected:
	vector<vec2> trans; // contains (x, y) transformations to each net
	Region baseRegion; // Base region centered around (0,0)
	double halfRegionWidth;

};
#endif GROUP_H_