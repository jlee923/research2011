#include "halfedge.h"

HalfEdge::HalfEdge() {
	sibling = NULL;
	forwardEdge = NULL;
	_startPos = NULL;
	//type = NULL;
	sector = NULL;
	id = halfEdgeCount++;
}

// General HalfEdge with uninitialized forward and sibling pointers
// takes in starting position vertex --> default makes boundary edges
HalfEdge::HalfEdge(Vertex* v) {
	/*ColorPicker cp;
	for (int i = 0; i < genus; i++) {
		color = cp.pickNextColor();
		colors.push_back(color);
	}*/
	_startPos = v;
	sibling = NULL;
	forwardEdge = NULL;
	id = halfEdgeCount++;
	//type = 0;
}

/* Specific HalfEdge determined --> type: 0=boundary; 1=path; 2= pole
HalfEdge::HalfEdge(Vertex* v, int typ) {
	_startPos = v;
	sibling = NULL;
	forwardEdge = NULL;
	id = halfEdgeCount++;
	type = typ;
}*/


// General HalfEdge with uninitialized forward and sibling pointers
// takes in # of copies, starting position vertex, and type of HalfEdge
/*HalfEdge::HalfEdge(unsigned int c, Vertex* v, int t) {
	ColorPicker cp;
	//color = cp.pickNextColor();
	_startPos = v;
	copies = c;
	type = t;
	sibling = NULL;
	forwardEdge = NULL;
	id = halfEdgeCount++;
}*/

void HalfEdge::setForwardEdge(HalfEdge *f) {
	cout << "set Forward Edge " << f->getPos(0.0) << endl;
	forwardEdge = f;
}

void HalfEdge::setSameSibling(HalfEdge *s) {
	ColorPicker cp;
	colors.clear();
	s->colors.clear();
	for (int i = 0; i < genus; i++) {
		color = cp.pickNextColor();
		s->colors.push_back(color);
		colors.push_back(color);
	}
	sibling = s;
	s->sibling = this;
}

void HalfEdge::setNextSibling(HalfEdge *s) {
	ColorPicker cp;
	vec3 firstColor;
	colors.clear();
	s->colors.clear();
	for (int i = 0; i < genus; i++) {
		color = cp.pickNextColor();
		if (i < (genus-1)) {
			s->colors.push_back(color);
		} else {
			vector<vec3>::iterator it = s->colors.begin();
			s->colors.insert(it, color);
		}
		colors.push_back(color);
	}
	sibling = s;
	s->sibling = this;
}

void HalfEdge::setPrevSibling(HalfEdge *s) {
	ColorPicker cp;
	vec3 firstColor;
	colors.clear();
	s->colors.clear();
	for (int i = 0; i < genus; i++) {
		color = cp.pickNextColor();
		if (i == 0) {
			s->colors.push_back(color);
		} else {
			vector<vec3>::iterator it = s->colors.end();
			it--;
			s->colors.insert(it, color);
		}
		colors.push_back(color);
	}
	sibling = s;
	s->sibling = this;
}

void HalfEdge::setSibling(HalfEdge *s) {
	vec2 endPos = s->getForwardEdge()->getPos(0.0);
	if (endPos == getPos(0.0) || 
		(endPos[0] == getPos(0.0)[0] && -endPos[1] == getPos(0.0)[1])) {
			setSameSibling(s);
	} else if (endPos[0] < getPos(0.0)[0]) {
		setNextSibling(s);
	} else if (endPos[0] > getPos(0.0)[0]) {
		setPrevSibling(s);
	}

	//sibling = s;
}

// siblings for paths
void HalfEdge::setSameSibling(HalfEdge *s, vector<vec3> color) {
	colors.clear();
	s->colors.clear();
	s->colors = color;
	colors = color;
	sibling = s;
	s->sibling = this;
}

void HalfEdge::setSibling(HalfEdge *s, vector<vec3> color) {
	vec2 endPos = s->getForwardEdge()->getPos(0.0);
	setSameSibling(s, color);
}


// returns the position of the particular halfedge, interpolated with the t value
vec2 HalfEdge::getPos(double t = 0.0) {
	vec2 _endPos;
	if (t == 0.0) 
		return _startPos->getPos();
	if (getForwardEdge()!=NULL) {
		_endPos = getForwardEdge()->getPos(0.0);
	} else {
		_endPos[0] = 0.0;
		_endPos[1] = 0.0;
	}
	vec2 interPos = (_startPos->getPos())*(1-t)+_endPos*(t);
	return interPos;
}

// Sets two halfedges equal
HalfEdge& HalfEdge::operator =(const HalfEdge& e) {
	_startPos = e._startPos;
	sibling = e.sibling;
	forwardEdge = e.forwardEdge;
	//type = e.type;
	//copies = e.copies;
	sector = e.sector;
	return *this;
}

// draws this halfedge with the given translation
void HalfEdge::draw(vec2 *trans) {
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

bool HalfEdge::pointOnEdge(vec2 clickedPt) {

	// Returns true if the distance of the clicked point to the line segment determined by the half-edge is less than 0.0125

	vec2 startPt = _startPos->getPos();
	vec2 endPt = forwardEdge->_startPos->getPos();
	//cout << "startpos: " << startPt << " endPt : " << endPt << endl;
	// Return false if the clicked point lies outside the box determined by the end points of the line segment
	if (endPt[0] == startPt[0]){ return (abs(clickedPt[0] - endPt[0]) < 0.0125) && clickedPt[1] >= min(startPt[1], endPt[1]) && 
		clickedPt[1] <= max(startPt[1], endPt[1]);}
	if (endPt[1] == startPt[1]){ return (abs(clickedPt[1] - endPt[1]) < 0.0125) && clickedPt[0] >= min(startPt[0], endPt[0]) && 
	clickedPt[0] <= max(startPt[0], endPt[0]);}

	if (clickedPt[0] <= min(startPt[0], endPt[0]) || 
		clickedPt[0] >= max(startPt[0], endPt[0]) || 
		clickedPt[1] <= min(startPt[1], endPt[1]) || 
		clickedPt[1] >= max(startPt[1], endPt[1])) 
		return false;



	double m = ((endPt[1] - startPt[1])/(endPt[0] - startPt[0]));
	double n = -(1/m);

	// Drop a perpendicular from clickedPt to the line segment defined by startPt and endPt.  Let (x,y) be the intersection point

	double x = (clickedPt[1] - startPt[1] + startPt[0]*m - clickedPt[0]*n)/(m-n);
	double y = startPt[1] + m*(x-startPt[0]);

	return (sqrt (pow (clickedPt[0] - x, 2) + pow (clickedPt[1] - y, 2)) < 0.0125);
 
}


bool HalfEdge::inPlane(vec2 clickedPt) {

	// Returns true iff the clicked point lies in the half-plane determined by the half edge (i.e. the clicked point must be "on the left" of the half-edge)

	vec2 startPt = _startPos->getPos();
	vec2 endPt = forwardEdge->_startPos->getPos();


	if (endPt[0] == startPt[0]) {
		if (endPt[1] > startPt[1]) return (clickedPt[0] < endPt[0]);
		else return (clickedPt[0] > endPt[0]);

	}

	double m = ((endPt[1] - startPt[1])/(endPt[0] - startPt[0]));

	if (endPt[0] < startPt[0])  return clickedPt[1] < (m*(clickedPt[0]-startPt[0]) + startPt[1]);
	else return clickedPt[1] > (m*(clickedPt[0]-startPt[0]) + startPt[1]);

}

HalfEdge::~HalfEdge() {
	delete _startPos;     // Vertex of general halfedge starting position
	delete &color;
	delete &colors;
	delete &id;
}

unsigned int HalfEdge::halfEdgeCount = 0;