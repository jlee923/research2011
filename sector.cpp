#include "sector.h"

Sector::Sector() {
	
}

Sector::Sector(HalfEdge * edge) {
	sectorEdge = edge;
	vertex = NULL;
	cout << "Made Sector with:" << endl;
	do {
		cout << "\tedge " << (edge)->getID() << endl;
		//cout << "Edge at: " << edge->getID() << endl;
		bounds.push_back(edge->_startPos);
		edge->sector = this;
		edge = edge->forwardEdge;
	} while (edge != sectorEdge);

	color = vector<vec3>(4, NULL);
}


Sector::Sector(vector<Vertex*> corners) {
	// no vertex yet
	vertex = NULL;
	//setLabel(l);

	bounds = corners;
	// define bounds  <== probably don't need: take out later; update accordingly
	/*for (unsigned int i = 0; i < corners.size(); i++) {
		bounds.push_back(*(new Vertex(corners[i])));
	}*/
	// make halfEdge Boundaries 
	// each halfEdge int index corresponds to its starting point vertex int index
	HalfEdge * prev;
	HalfEdge * temp;
	for (unsigned int i = 0; i < bounds.size(); i ++) {	// bounds
		temp = new Boundary(bounds[i]);
		(*temp).setSector(this);
		if (i == 0) {
			sectorEdge = temp;
			prev = temp;
			continue;
		} else {
			prev->setForwardEdge(temp);
			if (i == (bounds.size()-1)) {
				temp->setForwardEdge(sectorEdge);
			}
			prev = temp;
		}
	}

	color = vector<vec3>(4, NULL);
	
}


vector<HalfEdge*> Sector::getBounds() {
	vector<HalfEdge*> temp;
	HalfEdge * check = sectorEdge->getForwardEdge();
	temp.push_back(sectorEdge);
	while (check != sectorEdge) {
		temp.push_back(check);
		check = check->getForwardEdge();
	}
	return temp;
} 	// returns a list of the bounding edges for this sector


vector<Sector> Sector::getNeighborSectors() {
	vector<Sector> neighbors;
	vector<HalfEdge *> myBounds = getBounds();
	for (vector<HalfEdge*>::iterator it = myBounds.begin(); it != myBounds.end(); it++) {
		if ((*it)->canPass() && (*it)->getSibling() != NULL) {
			neighbors.push_back(*((*it)->getSibling()->getSector()));
		}
	}
	return neighbors;
}

void Sector::draw(vec2 *t) {
	double x = (*t)[0];
	double y = (*t)[1];
	//draw the interior of the sector
	if (bounds.size() < 4) {
		int me = oper;
		vector<vec3> check = color;
		if (color[oper] == NULL) {
			glColor3f(0.2,0.2,.9);
		} else {
			vec3 currCol = color[oper];
			glColor3f(currCol[0], currCol[1], currCol[2]);
		}
		glBegin(GL_POLYGON);
		HalfEdge * point = sectorEdge->getForwardEdge();
		glVertex2d((sectorEdge->getPos(0.0)[0] + x), (sectorEdge->getPos(0.0)[1] + y));
		while (point != sectorEdge) {
			//if (label == 3) {
			//	cout << point->getPos(0.0)[0] << "x" << point->getPos(0.0)[1] << "y" << endl;
			//}
			glVertex2d((point->getPos(0.0)[0]+x),(point->getPos(0.0)[1]+y));
			point = point->getForwardEdge();
		}
		glEnd();
	} 
	// OpenGL only draws primitive polygons, so it can't do convex shapes.. which are necessary,
	// so do it here with triangles
	// VERY IMPORTANT THAT STARTING PATH VERTICES/EDGES ARE NOT MADE SECTOR EDGES
	else {
		vector<HalfEdge*> forward, backward;
		HalfEdge * track = sectorEdge->getForwardEdge();
		forward.push_back(sectorEdge);
		while (track != sectorEdge) {
			forward.push_back(track);
			track = track->getForwardEdge();
		}
		vector<HalfEdge*>::iterator forwardIt = forward.begin();
		vector<HalfEdge*>::reverse_iterator backwardIt = forward.rbegin();
		if (color[oper] == NULL) {
			glColor3f(0.2,0.2,.9);
		} else {
			vec3 currCol = color[oper];
			glColor3f(currCol[0], currCol[1], currCol[2]);
		}

		glBegin(GL_QUAD_STRIP);
		glVertex2d((*forwardIt)->getPos(0.0)[0]+x, (*forwardIt)->getPos(0.0)[1]+y);
		forwardIt++;
		bool back = false; // back = true, use backward; false, use forward
		while((*forwardIt) != (*backwardIt)) {
			if (back) {
				glVertex2d((*backwardIt)->getPos(0.0)[0]+x, (*backwardIt)->getPos(0.0)[1]+y);
				backwardIt++;
				back = false;
			} else {
				glVertex2d((*forwardIt)->getPos(0.0)[0]+x, (*forwardIt)->getPos(0.0)[1]+y);
				forwardIt++;
				back = true;
			}
		}
		glVertex2d((*forwardIt)->getPos(0.0)[0]+x, (*forwardIt)->getPos(0.0)[1]+y);
		if (forward.size()%2 == 1) {
			glVertex2d((*forwardIt)->getPos(0.0)[0]+x, (*forwardIt)->getPos(0.0)[1]+y);
		}
		glEnd();
	}


	

	//draw exterior of sector
	/*glBegin(GL_LINE_LOOP);
	glColor3f(0.0,0.0,0);
	point = sectorEdge->getForwardEdge();
	glVertex2d((sectorEdge->getPos(0.0)[0]+x), (sectorEdge->getPos(0.0)[1]+y));
	while (point != sectorEdge) {
		glVertex2d((point->getPos(0.0)[0]+x), (point->getPos(0.0)[1]+y));
		point = point->getForwardEdge();
	}
	glEnd();*/

	// ask the bounds to draw themselves
	//draw halfedges
	HalfEdge* temp = sectorEdge;
	while (true){
	//for (int i = 0; i < bounds.size(); i++) {
		temp->draw(t);
		temp = temp->getForwardEdge();
		if (temp == sectorEdge) break;
	}

	// draw the vertex
	if (vertex != NULL)
		vertex->draw(t);
}

///////////////////////////////////

vector<HalfEdge*> Sector::getPermiableEdges() {
	vector<HalfEdge*> permEdges;
	if (sectorEdge->canPass()) {
		permEdges.push_back(sectorEdge);
	}
	HalfEdge * temp = sectorEdge->getForwardEdge();
	while(temp != sectorEdge) {
		if (temp->canPass()) {
			permEdges.push_back(temp);
		}
		temp = temp->getForwardEdge();
	}
	return permEdges;
}

void Sector::print() {
	vector<HalfEdge *> myBounds;
	for (vector<HalfEdge *>::iterator it = myBounds.begin(); it != myBounds.end() ; it++ ) {
		cout <<( *it)->getID() << " with sibling " <<( *it)->getSibling()<< endl;
	}
}

HalfEdge * Sector::splitEdge(HalfEdge * me) {
	HalfEdge * sib = me->getSibling();
	HalfEdge * sibFor = sib->getForwardEdge();
	HalfEdge * myFor = me->getForwardEdge();
	
	vec2 mp1 = me->getPos(0.5);
	vec2 mp2 = sib->getPos(0.5);

	HalfEdge * newMyFor, * newSibFor;
	if (mp1 == mp2) {
		// on 2-d plane, vertex is the same point
		//XXX DO A CHECK FOR EDGETYPE?
		Vertex * mid1 = new Vertex(mp1);
		newMyFor = new Boundary(mid1);
		newSibFor = new Boundary(mid1);
	} else {
		// on 2-d plane, vertex is different point
		Vertex * mid1 = new Vertex(mp1);
		Vertex * mid2 = new Vertex(mp2);
		newMyFor = new Boundary(mid1);
		newSibFor = new Boundary(mid2);
	}

	// set forward edges
	me->setForwardEdge(newMyFor);
	newMyFor->setForwardEdge(myFor);
	sib->setForwardEdge(newSibFor);
	newSibFor->setForwardEdge(sibFor);
	newMyFor->setOperChange(me->getOperChange());
	newSibFor->setOperChange(sib->getOperChange());

	// set sectors
	newMyFor->setSector(me->getSector());
	newSibFor->setSector(sib->getSector());

	// set siblings
	me->setSibling(newSibFor);
	sib->setSibling(newMyFor);
	//newMyFor->setSibling(sib);
	//newSibFor->setSibling(me);

	return me;
}


bool Sector::isSameSector(HalfEdge * e1, HalfEdge * e2) {
	HalfEdge * ref = e1->getForwardEdge();
	while (ref != e1) {
		if (ref == e2) {
			return true;
		}
		ref = ref->getForwardEdge();
	}
	return false;
}

// given pathEdge (e1) and destination edge (e2) of 2 divided edges on each side of division, divides sector
Sector * Sector::divide(HalfEdge * e1, HalfEdge * e2) {
	Sector * original = e1->getSector();
	Vertex * vert = NULL;
	if (original->getVertex() != NULL) {
		vert = original->getVertex();
		original->setVertex(NULL);
	}
	Vertex * m1 = e1->getForwardEdge()->_startPos;
	Vertex * m2 = e2->getForwardEdge()->_startPos;
	HalfEdge * e1For = e1->getForwardEdge();
	HalfEdge * e2For = e2->getForwardEdge();

	HalfEdge * newE1For = new Path(m1);
	HalfEdge * newE2For = new Path(m2);

	if (e1->getForwardEdge()->getSibling()->getForwardEdge()->iAm() != 0) {
		e1->setForwardEdge(newE1For);
		newE1For->setForwardEdge(e2For);
		e2->setForwardEdge(newE2For);
		newE2For->setForwardEdge(e1For);
		newE1For->setSibling(newE2For);
	} else {
		vector<vec3> colors = e1->getForwardEdge()->getSibling()->getForwardEdge()->getColors();
		vector<vec3> sendCols;
		vec2 endPos = e1->getForwardEdge()->getSibling()->getForwardEdge()->getPos(0.0);
		if (endPos == e1->getPos(1.0) || 
			(endPos[0] == e1->getPos(1.0)[0] && -endPos[1] == e1->getPos(1.0)[1])) {
				sendCols = colors;
		} else if (endPos[0] < e1->getPos(0.0)[0]) {
			// going left
			vector<vec3>::iterator it = colors.begin();
			it++;
			while (it != colors.end()) {				
				sendCols.push_back(*it);
				it++;
			}
			sendCols.push_back(colors.front());
		} else if (endPos[0] > e1->getPos(0.0)[0]) {
			// going right
			for (vector<vec3>::iterator it = colors.begin(); it != colors.end()--; it++) {
				sendCols.push_back(*it);
			}
			vector<vec3>::iterator it = sendCols.begin();
			sendCols.insert(it, colors.back());
		}

		e1->setForwardEdge(newE1For);
		newE1For->setForwardEdge(e2For);
		e2->setForwardEdge(newE2For);
		newE2For->setForwardEdge(e1For);
		newE1For->setSibling(newE2For, sendCols);
	}

	vector<Vertex *> newBounds;
	HalfEdge * currentEdge = e1;
	do {
		newBounds.push_back(currentEdge->_startPos);
		currentEdge->setSector(original);
		currentEdge = currentEdge->getForwardEdge();
	} while (currentEdge != e1);
	e1->getSector()->setBounds(newBounds);
	original->sectorEdge = e1;

	HalfEdge* e2Point = e2;
	while (e2Point->iAm() == 0) {
		e2Point = e2Point->getForwardEdge();
	}
	Sector * returnSect = new Sector(e2Point);
	if (vert != NULL && original->pointInSector(vert->getPos())) {
		original->setVertex(vert);
	} else if (vert != NULL) {
		returnSect->setVertex(vert);
	}

	returnSect->setColors(original->getColors());

	return returnSect;
}


Sector * Sector::undoPath(HalfEdge * pathEdge) {
	Sector * keepSector, * removeSector;
	HalfEdge * pathSib = pathEdge->getSibling();
	HalfEdge * forSib = pathEdge->getForwardEdge()->getSibling();
	HalfEdge * delEdge = forSib->getForwardEdge();
	HalfEdge * delSibEdge = delEdge->getSibling();
	HalfEdge * newPathFor = delEdge->getForwardEdge();

	// sector to use
	unsigned int minLabel = min(pathSib->getSector()->getLabel(), forSib->getSector()->getLabel());
	if (pathSib->getSector()->getLabel() == minLabel) {
		keepSector = pathSib->getSector();
		removeSector = forSib->getSector();
	} else {
		keepSector = forSib->getSector();
		removeSector = pathSib->getSector();
	}

	if (removeSector->getVertex() != NULL) {
		keepSector->setVertex(removeSector->getVertex());
	}

	//Sector * one = delEdge->getSector();
	//Sector * two = delSibEdge->getSector();

	HalfEdge * newPathEdge;
	newPathEdge = pathEdge->getSibling();
	while(newPathEdge->getForwardEdge() != delSibEdge) {
		newPathEdge = newPathEdge->getForwardEdge();
	}

	// handle deleting and merging...
	forSib->setForwardEdge(pathSib);
	newPathEdge->setForwardEdge(newPathFor);

	vector<Vertex *> newBounds;
	newBounds.push_back(newPathEdge->_startPos);
	newPathEdge->setSector(keepSector);
	HalfEdge * setSectorLabels = newPathEdge->getForwardEdge();
	//cout << "failed before while" << endl;
	while (setSectorLabels != newPathEdge) {
		newBounds.push_back(setSectorLabels->_startPos);
		setSectorLabels->setSector(keepSector);
		setSectorLabels = setSectorLabels->getForwardEdge();
	}

	//cout << "failed after while" << endl;

	//delete(delSibEdge);
	//delete(newPathFor);

	//cout << "failed at delete" << endl;

	keepSector->setBounds(newBounds);

	mergeEdges(pathEdge);
	//cout << "end of undo" << endl;
	return removeSector;
}

Sector * Sector::undoPath(HalfEdge * lastPathEdge, HalfEdge * closestInEdge) {
	/*
	HalfEdge * remove1 = closestInEdge->getForwardEdge();
	HalfEdge * remove2 = remove1->getSibling();
	Sector * keepSect = remove2->getSector();
	Sector * removeSect = closestInEdge->getSector();

	HalfEdge * track = remove2;
	while(track->getForwardEdge() != remove2) {
		track = track->getForwardEdge();
	}
	lastPathEdge = track;

	lastPathEdge->setForwardEdge(remove2->getForwardEdge());
	closestInEdge->setForwardEdge(remove1->getForwardEdge());
	*/
	
	HalfEdge * remove1 = lastPathEdge->getForwardEdge();
	HalfEdge * remove2 = closestInEdge->getForwardEdge();
	Sector * keepSect = lastPathEdge->getSector();
	Sector * removeSect = closestInEdge->getSector();

	closestInEdge->setForwardEdge(remove1->getForwardEdge());
	lastPathEdge->setForwardEdge(remove2->getForwardEdge());
	
	vector<Vertex *> newBounds;
	newBounds.push_back(lastPathEdge->_startPos);
	HalfEdge * setSectorLabels = lastPathEdge->getForwardEdge();
	while (setSectorLabels != lastPathEdge) {
		newBounds.push_back(setSectorLabels->_startPos);
		setSectorLabels->setSector(keepSect);
		setSectorLabels = setSectorLabels->getForwardEdge();
	}

	if (removeSect->getVertex() != NULL) {
		keepSect->setVertex(removeSect->getVertex());
	}

	keepSect->setBounds(newBounds);

	return removeSect;

}

void Sector::mergeEdges(HalfEdge * me) {
	HalfEdge * myFor = me->getForwardEdge();
	HalfEdge * sib = myFor->getSibling();
	HalfEdge * sibFor = sib->getForwardEdge();
	HalfEdge * myNewFor = myFor->getForwardEdge();
	HalfEdge * sibNewFor = sibFor->getForwardEdge();

	me->setForwardEdge(myNewFor);
	sib->setForwardEdge(sibNewFor);

	me->setSibling(sib);

	//delete(myFor);
	//delete(sibFor);
}

HalfEdge * Sector::startPath(Vertex * v, HalfEdge * edge) {
	HalfEdge * pathEdge = Sector::splitEdge(edge)->getForwardEdge()->getSibling();
	
	Vertex * goToVert = edge->getForwardEdge()->_startPos;
	// if sibling is also forward pointer
	bool sameForPointer = false;

	HalfEdge * outEdge = new Path(v);
	HalfEdge * inEdge = new Path(goToVert);

	HalfEdge * edgeFor = edge->getForwardEdge();
	edge->setForwardEdge(inEdge);
	outEdge->setForwardEdge(edgeFor);
	HalfEdge * temp;
	temp = edgeFor;
	// looping around from the out-pointer, if i reach "edge", then only 1 sector
	// if i reach the current vertex, then there exists another path to this vertex (other paths)
	while (temp->_startPos != v) {
		if (temp == edge) {
			sameForPointer = true;
			break;
		}
		temp = temp->getForwardEdge();
	}
	if (sameForPointer) {
		inEdge->setForwardEdge(outEdge);
	} else {
		// false, so temp currently other inEdge
		temp->setForwardEdge(outEdge);
		inEdge->setForwardEdge(temp->getSibling());
	}

	ColorPicker cp;
	vector<vec3> colors;
	for (int i = 0; i < genus; i++) {
		colors.push_back(cp.pickNextColor());
	}
	inEdge->setSameSibling(outEdge, colors);

	vector<Vertex*> newBounds;
	temp = edge->getForwardEdge();
	newBounds.push_back(edge->_startPos);
	while (temp != edge) {
		newBounds.push_back(edge->_startPos);
		temp= temp->getForwardEdge();
	}

	inEdge->setSector(edge->getSector());
	outEdge->setSector(edge->getSector());
	bounds = newBounds;
	sectorEdge = edge;
	return pathEdge;
}

/* ==========================================================================
	Past semester stuff... re-wrote, so probably delete
   ==========================================================================
*/
// want this one
bool Sector::pointInSector(vec2 clickedPt) {
	// For each half-edge on the sector's boundary, check that the clicked point lies in the half-plane determined by that half-edge
	HalfEdge * current = sectorEdge;

	do{

	if (!current->inPlane(clickedPt)) return false;
	current = current->getForwardEdge();

	} while (current!=sectorEdge);

	return true;

}

Sector * Sector::split(HalfEdge * e1, HalfEdge *  e2) {
	Sector * thisSector = e1->getSector();
	
	vec2 mp1 = e1->getPos(0.5);
	vec2 mp2 = e2->getPos(0.5);
	vec2 mp3 = e1->sibling->getPos(0.5);
	vec2 mp4 = e2->sibling->getPos(0.5);
	
	Vertex * mid1 = new Vertex(mp1);
	Vertex * mid2 = new Vertex(mp2);
	Vertex * mid3 = new Vertex(mp3);
	Vertex * mid4 = new Vertex(mp4);

	HalfEdge * m1 = new HalfEdge(mid1), *t1 = new HalfEdge(mid1), *b1 = new HalfEdge(mid4);
	HalfEdge * m2 = new HalfEdge(mid2), *t2 = new HalfEdge(mid3), *b2 = new HalfEdge(mid2);
	
	HalfEdge * oldE1sibling = e1->sibling;	
	HalfEdge * oldE1forward = e1->forwardEdge;	
	HalfEdge * oldE1sibForward = e1->sibling->forwardEdge;
	
	HalfEdge * oldE2sibling = e2->sibling;
	HalfEdge * oldE2forward = e2->forwardEdge;
	HalfEdge * oldE2sibForward = e2->sibling->forwardEdge;

	cout << _DRAW_WIDTH/(2*genus) << " " << e1->getPos(0.0)[0] << endl;
	if (_DRAW_WIDTH/(2*genus) - e1->getPos(0.0)[0] < 0.0125 &&
			e1->getPos(0.0)[0] == e1->getPos(1.0)[0]) {
		cout << "outEdge1" << endl;
		e1->setNextSibling(t2);
		t1->setNextSibling(oldE1sibling);
	} else if (_DRAW_WIDTH/(2*genus) + e1->getPos(0.0)[0] < 0.0125 &&
			e1->getPos(0.0)[0] == e1->getPos(1.0)[0]) {
		cout << "inEdge1" << endl;
		e1->setPrevSibling(t2);
		t1->setPrevSibling(oldE1sibling);
	} else {
		cout << "inner1" << endl;
		e1->setSameSibling(t2);
		oldE1sibling->setSameSibling(t1);
	}

	if (_DRAW_WIDTH/(2*genus) - e2->getPos(0.0)[0] < 0.0125 &&
			e2->getPos(0.0)[0] == e2->getPos(1.0)[0]) {
		cout << "outEdge2" << endl;
		e2->setNextSibling(b1);
		b2->setNextSibling(oldE2sibling);
	} else if (_DRAW_WIDTH/(2*genus) + e2->getPos(0.0)[0] < 0.0125 &&
			e2->getPos(0.0)[0] == e2->getPos(1.0)[0]) {
		cout << "inEdge2" << endl;
		e2->setPrevSibling(b1);
		b2->setPrevSibling(oldE2sibling);
	} else {
		cout << "inner2" << endl;
		e2->setSameSibling(b1);
		oldE2sibling->setSameSibling(b2);
	}
	
	//e1->setSibling(t2);
	//e1->setSameSibling(t2);
	e1->forwardEdge = m1;
	//oldE1sibling->sibling = t1;
	//oldE1sibling->setSameSibling(t1);
	oldE1sibling->forwardEdge = t2;
	//t1->setSibling(oldE1sibling);
	//t1->setSameSibling(oldE1sibling);
	t1->forwardEdge = oldE1forward;
	//t2->setSibling(e1);
	//t2->setSameSibling(e1);
	t2->forwardEdge = oldE1sibForward;
	//m1->setSibling(m2);
	m1->setSameSibling(m2);
	m1->forwardEdge = b2;
	//m2->setSibling(m1);
	//m2->setSameSibling(m1);
	m2->forwardEdge = t1;
	
	//e2->setSibling(b1);
	//e2->setSameSibling(b1);
	e2->forwardEdge = m2;
	//oldE2sibling->setSibling(b2);
	//oldE2sibling->setSameSibling(b2);
	oldE2sibling->forwardEdge = b1;
	//b1->setSibling(e2);
	//b1->setSameSibling(e2);
	b1->forwardEdge = oldE2sibForward;
	//b2->setSibling(oldE2sibling);
	//b2->setSameSibling(oldE2sibling);
	b2->forwardEdge = oldE2forward;
	
	cout << "e1: " << e1->getID() << " forward: " << e1->getForwardEdge()->getID() << " sibling: " << e1->getSibling()->getID() << endl; 
	cout << "e2: " << e2->getID() << " forward: " << e2->getForwardEdge()->getID() << " sibling: " << e2->getSibling()->getID() << endl; 
	cout << "o1: " << oldE1sibling->getID() << " forward: " << oldE1sibling->getForwardEdge()->getID() << " sibling: " << oldE1sibling->getSibling()->getID() << endl; 
	cout << "o2: " << oldE2sibling->getID() << " forward: " << oldE2sibling->getForwardEdge()->getID() << " sibling: " << oldE2sibling->getSibling()->getID() << endl;
	cout << "t1: " << t1->getID() << " forward: " << t1->getForwardEdge()->getID() << " sibling: " << t1->getSibling()->getID() << endl; 
	cout << "t2: " << t2->getID() << " forward: " << t2->getForwardEdge()->getID() << " sibling: " << t2->getSibling()->getID() << endl; 
	cout << "b1: " << b1->getID() << " forward: " << b1->getForwardEdge()->getID() << " sibling: " << b1->getSibling()->getID() << endl; 
	cout << "b2: " << b2->getID() << " forward: " << b2->getForwardEdge()->getID() << " sibling: " << b2->getSibling()->getID() << endl; 
	cout << "m1: " << m1->getID() << " forward: " << m1->getForwardEdge()->getID() << " sibling: " << m1->getSibling()->getID() << endl; 
	cout << "m2: " << m2->getID() << " forward: " << m2->getForwardEdge()->getID() << " sibling: " << m2->getSibling()->getID() << endl; 
	
	e1->sector->sectorEdge = m1;
	
	vector<Vertex *> newBounds;
	HalfEdge * currentEdge = e1;
	/* TODO: REMOVE THIS DO LOOP AND FIX SECTOR CLASS */
	do {
		newBounds.push_back(currentEdge->_startPos);
		currentEdge->setSector(thisSector);
		currentEdge = currentEdge->getForwardEdge();
	} while (currentEdge != e1);
	e1->sector->setBounds(newBounds);
	
	currentEdge = oldE1sibling;
	thisSector = oldE1sibling->sector;
	newBounds.clear();
	do {
		newBounds.push_back(currentEdge->_startPos);
		currentEdge->setSector(thisSector);
		currentEdge = currentEdge->getForwardEdge();
	} while (currentEdge != oldE1sibling);
	oldE1sibling->sector->setBounds(newBounds);
	
	currentEdge = oldE2sibling;
	thisSector = oldE2sibling->sector;
	newBounds.clear();
	do {
		newBounds.push_back(currentEdge->_startPos);
		currentEdge->setSector(thisSector);
		currentEdge = currentEdge->getForwardEdge();
	} while (currentEdge != oldE2sibling);
	oldE2sibling->sector->setBounds(newBounds);
	return new Sector(m2);
}




Sector * Sector::merge(HalfEdge * m1) {
	Sector * toBeKept = m1->sector;
	Sector * toBeTossed = m1->sibling->sector;
	HalfEdge * m2 = m1->sibling;
	HalfEdge * b2 = m1->forwardEdge;
	HalfEdge * b1 = b2->sibling->forwardEdge;
	HalfEdge * e2 = b1->sibling;

	HalfEdge * t1 = m2->forwardEdge;
	HalfEdge * t2 = t1->sibling->forwardEdge;
	HalfEdge * e1 = t2->sibling;
	
	e1->forwardEdge = t1->forwardEdge;
	e1->setSibling(t1->sibling);
	e2->forwardEdge = b2->forwardEdge;
	e2->setSibling(b2->sibling);
	
	toBeKept->sectorEdge = e1;
	
	HalfEdge * currentEdge = e1;
	vector<Vertex *> newBounds;
	/* TODO: REMOVE THIS DO LOOP AND FIX SECTOR CLASS */

	do {
		newBounds.push_back(currentEdge->_startPos);
		currentEdge->sector = toBeKept;
		currentEdge = currentEdge->getForwardEdge();
	} while (currentEdge != e1);
	toBeKept->setBounds(newBounds);


	if(t2->sector->sectorEdge == t2) { t2->sector->sectorEdge = e1->sibling; }
	if(b1->sector->sectorEdge == b1) { b1->sector->sectorEdge = e2->sibling; }

	delete m1;
	delete m2;
	delete t1;
	delete t2;
	delete b1;
	delete b2;
	/*m1->~HalfEdge();//m1 = NULL;
	m2->~HalfEdge();//m2 = NULL;
	t1->~HalfEdge();//t1 = NULL;
	t2->~HalfEdge();//t2 = NULL;
	b1->~HalfEdge();//b1 = NULL;
	b2->~HalfEdge();//b2 = NULL;*/
	return toBeKept;
}

Sector * Sector::split(HalfEdge * pathEdge, HalfEdge * e1, HalfEdge * e2) {

	/**** TODO: AFTER IMPLEMENTATION OF PATH EDGES SET PATH EDGE'S PATH POINTER ****/
	if (e1 == pathEdge->forwardEdge->sibling) {
		e1 = e1->forwardEdge;
	} else if (e1 != pathEdge->forwardEdge->sibling->forwardEdge) {
		//error entry edge not touching the path edge
		cout << " should be returning Null" << endl;
	//	return NULL;
	} 
	
	Sector * thisSector = e1->sector;
	vec2 mp1 = e1->getPos(0.0);
	vec2 mp2 = e2->getPos(0.5);
	vec2 mp3 = e2->sibling->getPos(0.5);
	
	HalfEdge * m1 = new HalfEdge(new Vertex(mp1)), * m2 = new HalfEdge(new Vertex(mp2)), 
			* b1 = new HalfEdge(new Vertex(mp3)), * b2 = new HalfEdge(new Vertex(mp2));
		
	HalfEdge * oldE2sibling = e2->sibling;
	HalfEdge * oldE2forward = e2->forwardEdge;
	HalfEdge * oldE2sibForward = e2->sibling->forwardEdge;
	
 	pathEdge->forwardEdge->sibling->forwardEdge = m1;
	m1->forwardEdge = b2;
	m1->setSibling(m2);
	m2->forwardEdge = e1;
	b2->forwardEdge = oldE2forward;
	b2->setSibling(oldE2sibling);
	e2->forwardEdge = m2;
	e2->setSibling(b1);
	oldE2sibling->forwardEdge = b1;
	oldE2sibling->setSibling(b2);
	b1->forwardEdge = oldE2sibForward;
	
	cout << "e1: " << e1->getID() << " forward: " << e1->getForwardEdge()->getID() << " sibling: " << e1->getSibling()->getID() << endl; 
	cout << "e2: " << e2->getID() << " forward: " << e2->getForwardEdge()->getID() << " sibling: " << e2->getSibling()->getID() << endl; 
	cout << "o2: " << oldE2sibling->getID() << " forward: " << oldE2sibling->getForwardEdge()->getID() << " sibling: " << oldE2sibling->getSibling()->getID() << endl;
	cout << "b1: " << b1->getID() << " forward: " << b1->getForwardEdge()->getID() << " sibling: " << b1->getSibling()->getID() << endl; 
	cout << "b2: " << b2->getID() << " forward: " << b2->getForwardEdge()->getID() << " sibling: " << b2->getSibling()->getID() << endl; 
	cout << "m1: " << m1->getID() << " forward: " << m1->getForwardEdge()->getID() << " sibling: " << m1->getSibling()->getID() << endl; 
	cout << "m2: " << m2->getID() << " forward: " << m2->getForwardEdge()->getID() << " sibling: " << m2->getSibling()->getID() << endl; 
	
	
	vector<HalfEdge *> newBounds;
	HalfEdge * currentEdge = e1;
	/* TODO: REMOVE THIS DO LOOP AND FIX SECTOR CLASS *
	do {
		newBounds.push_back(currentEdge);
		currentEdge->setSector(thisSector);
		currentEdge = currentEdge->getForwardEdge();
	} while (currentEdge != e1);
	e1->sector->setBounds(newBounds);
	cout << " here" << endl;
	currentEdge = oldE2sibling;
	thisSector = oldE2sibling->sector;
	cout << "Sector " << thisSector->getLabel() << endl;
	newBounds.clear();
	
	do {
		newBounds.push_back(currentEdge);
		currentEdge->setSector(thisSector);
		currentEdge = currentEdge->getForwardEdge();
	} while (currentEdge != oldE2sibling);
	oldE2sibling->sector->setBounds(newBounds);
	*/
	return new Sector(m1);
}
