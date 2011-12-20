/*
 * The Sector Class:
 * One level below the Net class.
 *
 */

#ifndef SECTOR_H_
#define SECTOR_H_

#define _DRAW_HEIGHT 1.7
#define _DRAW_WIDTH 1.8

#include "global.h"
#include "halfedge.h"
#include "vertex.h"

enum { UPPERLEFT, UPPERRIGHT, LOWERRIGHT, LOWERLEFT}; // 4 regions of each sector

class HalfEdge;
class Vertex;

extern int genus;
extern int oper;

class Sector {
	/**  ------->		|
	 **					|	Boundary Edges are the set of Halfedges 	
	 **					|	pointing in the counter clockwise direction
	 **		sector		v
	 **		fill:color
	 **	^	contains 
	 **	|	[0-1 vertices]
	 **	|
	 **	|		<--------
	 **/
public:
	Sector();
	Sector(HalfEdge *edge);
	// construct sector with the given corner points
	Sector(vector<Vertex*> corners); 

	// do i need bounds?? Probably not!!
	void setBounds(vector<Vertex *> newBounds) {bounds = newBounds;}

	vector<HalfEdge*> getBounds(); 	// returns a list of the bounding edges for this sector
	vector<HalfEdge*> getPermiableEdges();	// returns a list of the bounded edges in this sector that can be passed
	
	// indicates whether or not this sector has a vertex placed in it and return pointer to it
	Vertex * getVertex() { return vertex; }

	// should call this from region, so that we can keep track of vertices
	void setVertex(Vertex * v) { vertex = v; }
								
	vector<Sector> getNeighborSectors(); // returns a list of the neighboring sectors that are accessable 

	void setSectorEdge(HalfEdge * newHead) { sectorEdge = newHead; }
	
	void draw(vec2 *t);	// draw the sector given a transformation matrix
	
	void fill(vec3 c, int reg) {color[reg] = c;}	// floods this sector with the 'color'
	
	void setColors(vector<vec3> col) { color = col; };
	void setColor(vec3 col, int op) { color[op] = col; };
	vec3 getColor(int reg) { return color[reg];}	// returns the 'color' for this sector
	vector<vec3> getColors() { return color; };
	
	void setLabel(unsigned int l) { label = l;} // label of this sector

	void print();
	
	int getLabel() {return label;}

	// splits a given edge and it's sibling.. forward pointers are new (me and sib are kept in back)
	// returns me
	static HalfEdge * splitEdge(HalfEdge * me);

	// merges a given edge and it's forward edge (also siblings)
	// deletes the forward edges
	static void mergeEdges(HalfEdge * me);

	// shouldn't need this... just do a check on the individual sector ids
	static bool isSameSector(HalfEdge * e1, HalfEdge * e2);

	// given pathEdge (e1) and destination edge (e2) of 2 divided edges on each side of division, divides sector
	static Sector * divide(HalfEdge * e1, HalfEdge * e2);
	// given pathEdge, locate edges to be deleted (forward->sibling->forward + forward->sibling->forward->sibling)
	// return Sector to remove
	// general undoPath (from boundary edge to boundary edge)
	static Sector * undoPath (HalfEdge * pathEdge);
	// undoPath specific for termination of a path (goes to a vertex) inputs are same edges that were provided for divide
	static Sector * undoPath (HalfEdge * lastPathEdge, HalfEdge * closestInEdge);

	// starts a path from the vertex, returns the pathedge
	HalfEdge * startPath(Vertex * v, HalfEdge * edge);

	static Sector * merge(HalfEdge * m1);

	static Sector * split(HalfEdge * e1, HalfEdge * e2);
	
	static Sector * split(HalfEdge * pathEdge, HalfEdge * e1, HalfEdge * e2);
	
	bool pointInSector(vec2 clickedPt); 

protected:
	vector<vec3> color;
	unsigned int label; // label for this sector
	vector<unsigned int> copies; // the translated labels for this sector
	HalfEdge * sectorEdge; // sector head halfEdge
	Vertex * vertex; // for the vertices we need to wire up
	vector<Vertex *> bounds; // pointers to vertex bounds
};
#endif