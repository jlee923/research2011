/* 
 * The Halfedge Class:
 * Consider using the half edge data structure + winged edge....
 * Must initialize HalfEdges from the Sector Class
 * Constructor + setSibling + setForwardEdge + setTrans + (doesn't work right now -->) setSector
 */

#ifndef HALFEDGE_H_
#define HALFEDGE_H_
#include "global.h"
#include "sector.h"
#include "colorpicker.h"

enum {_PATH, _BOUNDARY, _POLE, _NOTHING}; // 4 types of halfedges

class Sector;
class HalfEdge;
class Vertex;

extern int genus; // # holes desired 
extern int oper;  // region currently operating in

class HalfEdge {
	/* Edge represented as two HalfEdges
	**	(_startPos on this sibling)
	**	_endPos ------>    	<---- forwardEdge 
	**		|
	**		|	
	**		|				<---- sibling
	**		|
	**		v	
	**			sector
	**		^
	**		|
	**		|				<---- this halfEdge
	**		|
	**	_startPos 
	*/ 
public:
	HalfEdge();
	
	// make a halfEdge without a sibling
	//HalfEdge(Vertex& v);
	// make the general HalfEdge with specified number of copies, starting vertex, and type of HalfEdge; no siblings/forward edges specified
	//HalfEdge(unsigned int c, Vertex * v, int t); 
	// make a halfEdge with a sibling and connect the sibling to this edge
	//HalfEdge(Vertex& start, HalfEdge &sibling, HalfEdge &forward); 
	
	// HalfEdge with initializing vertex
	HalfEdge::HalfEdge(Vertex* v);
	//HalfEdge::HalfEdge(Vertex* v, int typ);
	HalfEdge::~HalfEdge();

	// set halfedge to be equal to another
	HalfEdge& operator = (const HalfEdge& e);

	// set copies
	//void setCopies(unsigned int c) { copies = c;}

	// get the sibling of this edge
	HalfEdge * getSibling() {
		return sibling;
	}
	
	// set the sibling of this edge
	void setSameSibling(HalfEdge *s); // sibling in the same region
	void setNextSibling(HalfEdge *s); // sibling in adjacent region
	void setPrevSibling(HalfEdge *s); // sibling in previous region
	void setSibling(HalfEdge *s);

	// used for paths that want the same colors
	void setSameSibling(HalfEdge *s, vector<vec3> color); // sibling in the same region
	void setNextSibling(HalfEdge *s, vector<vec3> color); // sibling in adjacent region
	void setPrevSibling(HalfEdge *s, vector<vec3> color); // sibling in previous region
	void setSibling(HalfEdge *s, vector<vec3> color);
	
	// retrieve the specified forward edge given the copy number, returning the map containing the int of the forward edge to the mapped HalfEdge
	HalfEdge* getForwardEdge() { 
		return forwardEdge;
	}
	
	// set a given forward edge with ID
	void setForwardEdge(HalfEdge *f);

	// get and set the colors of a particular region
	vec3 getColor(int reg) {return colors[reg];}
	void setColor(vec3 color, int reg) {colors[reg] = color;}

	vector<vec3> getColors() { return colors; }

	// get the coordinate of the edge linearly interrpolated with factor t
	vec2 getPos(double t);

	// Returns the type of this HalfEdge (-1 = Pole, 0 = Path, 1 = Boundary)
	//int getType() {return type;}

	unsigned int getID() {return id;}
	bool pointOnEdge(vec2 clickedPt);
	bool inPlane(vec2 clickedPt);
	
	// return a pointer to the sector that this halfedge belongs to
	Sector * getSector() { return sector; }
	
	// sets the sector that this halfedge belongs to
	void setSector(Sector *s) { sector = s; }
	
	virtual bool canPass() { 
		if (iAm() == _BOUNDARY) return true;//(type == 1) return true; 
		else return false;
	}
	
	// 'virtual' allows subclasses to overwrite this method
	virtual int iAm() { return _NOTHING; }
	
	virtual void draw(vec2 * trans);

	

	// really bad...
	
	friend class Sector;
protected:
	Vertex * _startPos;     // Vertex of general halfedge starting position
	HalfEdge * sibling;     // vector entry correlates to each halfedge copy, which maps to the number of another copy in another halfedge
	HalfEdge * forwardEdge; // vector entry correlates to each halfedge copy, which maps to the number of another copy in another halfedge
	//int type; // 0 = BoundaryEdge, 1 = PathEdge, 2 = PoleEdge
	Sector * sector;
	vec3 color;
	vector<vec3> colors;
	unsigned int id;
	static unsigned int halfEdgeCount;
};

class Path : public HalfEdge {
	public:
		Path(Vertex* v) : HalfEdge(v) {};

		// indicates whether or not a path can cross this edge
		bool canPass() { return false; }
		// indicates what kind of Edge this edge is
		int iAm() { return _PATH; }

		// draws this halfedge with the given translation
		void Path::draw(vec2 *trans) {
			double x = (*trans)[0];
			double y = (*trans)[1];
			glLineWidth(10.0);
			glBegin(GL_LINES);
			glColor3f(colors[oper][0],colors[oper][1],colors[oper][2]);
			glVertex2d(x + getPos(0.0)[0], getPos(0.0)[1] + y);
			glVertex2d(x + getPos(1.0)[0], y + getPos(1.0)[1]);
			//cout << x << " " << y << " abcdefghijklmnop " << _startPos->getPos()[0] << " " << _startPos->getPos()[1] << endl;
			glEnd();
		}
};

class Boundary : public HalfEdge {
	public:
		Boundary(Vertex* v) : HalfEdge(v) {};

		// indicates whether or not a path can cross this edge
		bool canPass() { return true; }
	
		// indicates what kind of Edge this edge is
		int iAm() { return _BOUNDARY; }

		// draws this halfedge with the given translation
		void Boundary::draw(vec2 *trans) {
			double x = (*trans)[0];
			double y = (*trans)[1];
			glLineWidth(5.0);
			glBegin(GL_LINES);
			glColor3f(colors[oper][0],colors[oper][1],colors[oper][2]);
			glVertex2d(x + getPos(0.0)[0], getPos(0.0)[1] + y);
			glVertex2d(x + getPos(1.0)[0], y + getPos(1.0)[1]);
			//cout << x << " " << y << " abcdefghijklmnop " << _startPos->getPos()[0] << " " << _startPos->getPos()[1] << endl;
			glEnd();
		}
};

class Pole: public HalfEdge {
	public:
		Pole(Vertex* v) : HalfEdge(v) {};

		// indicates whether or not a path can cross this edge
		bool canPass() { return false; }
		// indicates what kind of Edge this edge is
		int iAm() { return _POLE; }


		// draws this halfedge with the given translation
		void Pole::draw(vec2 *trans) {
			double x = (*trans)[0];
			double y = (*trans)[1];
			glLineWidth(5.0);
			glBegin(GL_LINES);
			glColor3f(colors[oper][0],colors[oper][1],colors[oper][2]);
			glVertex2d(x + getPos(0.0)[0], getPos(0.0)[1] + y);
			glVertex2d(x + getPos(1.0)[0], y + getPos(1.0)[1]);
			//cout << x << " " << y << " abcdefghijklmnop " << _startPos->getPos()[0] << " " << _startPos->getPos()[1] << endl;
			glEnd();
		}
};

#endif