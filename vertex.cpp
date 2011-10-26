#include "vertex.h"

Vertex::Vertex() {
	
}

Vertex::Vertex(vec2 position) {
	pos = position;
	partner = NULL;
}

Vertex::Vertex(vec2 position, unsigned int l): pos(position[0],position[1]) {
	label = l;
	partner = NULL;
}

void Vertex::addEdge(HalfEdge& edge) {
	neighbors.push_back(edge);
}

void Vertex::draw() {
	glBegin(GL_POINTS);
	glColor3f(0.9,0.2,0.2);
	glVertex2d(pos[0],pos[1]);
	glEnd();
}

void Vertex::draw(vec2* t) {
	glBegin(GL_POINTS);
	glColor3f(0.9,0.2,0.2);
	glVertex2d(pos[0],pos[1]);
	glEnd();
}

void Vertex::draw(vector<vec2>* trans) {
	for (vector<vec2>::iterator it = trans->begin(); it != trans->end(); it++) {
		double x = (*it)[0];
		double y = (*it)[1];
		glBegin(GL_POINTS);
		glColor3f(0.9,0.2,0.2);
		glVertex2d(pos[0]+x,pos[1]+y);
		glEnd();
	}
}