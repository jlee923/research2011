/*
 * The Vertex Class:
 * Contains things about the vertex
 *
 */

#ifndef VERTEX_H_
#define VERTEX_H_
#include "global.h"
#include "sector.h"
#include "halfedge.h"

class Sector;
class HalfEdge;

class Vertex {
public:
	Vertex();
	Vertex(vec2 pos);	// constructs the vertex @ the given position
	Vertex(vec2 pos, unsigned int label);	// constructs the vertex @ the given position with the given name
	
	void setLabel(unsigned int l) {label = l;}
	unsigned int getLabel() {return label;}	// returns the label of this vertex, returns -1 if there is no label
	vec2 getPos() {return pos;};
	void draw();
	void Vertex::draw(vec2* t);
	void draw(vector<vec2> *trans);
	void setPartner(Vertex* ver) {partner = ver;}
	Vertex* getPartner() {return partner;}

	void addEdge(HalfEdge& edge); // adds edges to the neighbor's vector, which specifies all the edges coming out of the vertex
	
protected:
	vec2 pos;
	unsigned int label;
	vector<HalfEdge> neighbors;	// halfedges coming out of this vertex
	Vertex* partner; // every bounding vertex has a corresponding partner from top to bottom
					 // null otherwise
	//Sector * container;
};
#endif
