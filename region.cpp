#include "region.h"


Region::Region() {

}

Region::Region(int regions, vector<vec2> trans) {
	Sector *ul, *ur, *ll, *lr, *bottomul, *bottomur, *bottomll, *bottomlr;
	sectorCount = 0;
	/*===========================================================================================*/
	// Top half i.e. the outside of the manifold
	
	double cx = 0;
	double cy = (.05 + _DRAW_HEIGHT/4);
	
	vector<Vertex*> v; // tracks all the initial bounding vertices upon creation
	vector<vec2> bnds; // desired bounds in vec2 format
	vector<Vertex*> bVer; // ONLY the bounds for given sector in vertex format
	vector<Vertex*>::iterator itVer; // iterator used for traversing through the known vertices
									 // itVer != itVec

	v.push_back(new Vertex(vec2(cx, cy + _DRAW_HEIGHT/4)));								//5
	v.push_back(new Vertex(vec2(cx - _DRAW_WIDTH/(2*regions), cy + _DRAW_HEIGHT/4)));	//1
	v.push_back(new Vertex(vec2(cx - _DRAW_WIDTH/(2*regions), cy)));					//2
	v.push_back(new Vertex(vec2(cx - _DRAW_WIDTH/(4*regions), cy)));					//3
	v.push_back(new Vertex(vec2(cx, cy + _DRAW_HEIGHT/8)));								//4
	bVer.push_back(new Vertex(vec2(cx, cy + _DRAW_HEIGHT/4)));							//5
	bVer.push_back(new Vertex(vec2(cx - _DRAW_WIDTH/(2*regions), cy + _DRAW_HEIGHT/4)));//1
	bVer.push_back(new Vertex(vec2(cx - _DRAW_WIDTH/(2*regions), cy)));					//2
	bVer.push_back(new Vertex(vec2(cx - _DRAW_WIDTH/(4*regions), cy)));					//3
	bVer.push_back(new Vertex(vec2(cx, cy + _DRAW_HEIGHT/8)));							//4
	ul = new Sector(bVer);
	ul->setLabel(sectorCount++);
	sectors[ul->getLabel()]= ul;
	bVer.clear();
	bnds.clear();
	//v.clear();

	bnds.push_back(vec2(cx + _DRAW_WIDTH/(2*regions), cy + _DRAW_HEIGHT/4));	//10
	bnds.push_back(vec2(cx, cy + _DRAW_HEIGHT/4));								//6
	bnds.push_back(vec2(cx, cy + _DRAW_HEIGHT/8));								//7
	bnds.push_back(vec2(cx + _DRAW_WIDTH/(4*regions), cy));						//8
	bnds.push_back(vec2(cx + _DRAW_WIDTH/(2*regions), cy));						//9
	for (vector<vec2>::iterator itVec = bnds.begin(); itVec != bnds.end(); itVec++) {
		for (itVer = v.begin(); itVer != v.end(); itVer++) {
			if (itVec[0] == itVer[0]->getPos()) {
				bVer.push_back(itVer[0]);
				break;
			}
		}
		if (itVer == v.end()) {
			bVer.push_back(new Vertex(itVec[0]));
			v.push_back(bVer.back());
		}
	}
	ur = new Sector(bVer);
	ur->setLabel(sectorCount++);
	sectors[ur->getLabel()]=ur;
	bVer.clear();
	bnds.clear();
	//v.clear();
	
	bnds.push_back(vec2(cx + _DRAW_WIDTH/(2*regions), cy));						//14
	bnds.push_back(vec2(cx + _DRAW_WIDTH/(4*regions), cy));						//15
	bnds.push_back(vec2(cx, cy - _DRAW_HEIGHT/8));								//11
	bnds.push_back(vec2(cx, cy - _DRAW_HEIGHT/4));								//12
	bnds.push_back(vec2(cx + _DRAW_WIDTH/(2*regions), cy - _DRAW_HEIGHT/4));	//13
	for (vector<vec2>::iterator itVec = bnds.begin(); itVec != bnds.end(); itVec++) {
		for (itVer = v.begin(); itVer != v.end(); itVer++) {
			if (itVec[0] == itVer[0]->getPos()) {
				bVer.push_back(itVer[0]);
				break;
			}
		}
		if (itVer == v.end()) {
			bVer.push_back(new Vertex(itVec[0]));
			v.push_back(bVer.back());
		}
	}
	lr = new Sector(bVer);
	lr->setLabel(sectorCount++);
	sectors[lr->getLabel()] = lr;
	bVer.clear();	
	bnds.clear();
	//v.clear();
	
	bnds.push_back(vec2(cx - _DRAW_WIDTH/(4*regions), cy));						//17
	bnds.push_back(vec2(cx - _DRAW_WIDTH/(2*regions), cy));						//18
	bnds.push_back(vec2(cx - _DRAW_WIDTH/(2*regions), cy - _DRAW_HEIGHT/4));	//19
	bnds.push_back(vec2(cx, cy - _DRAW_HEIGHT/4));								//20
	bnds.push_back(vec2(cx, cy - _DRAW_HEIGHT/8));								//16
	for (vector<vec2>::iterator itVec = bnds.begin(); itVec != bnds.end(); itVec++) {
		for (itVer = v.begin(); itVer != v.end(); itVer++) {
			if (itVec[0] == itVer[0]->getPos()) {
				bVer.push_back(itVer[0]);
				break;
			}
		}
		if (itVer == v.end()) {
			bVer.push_back(new Vertex(itVec[0]));
			v.push_back(bVer.back());
		}
	}
	ll = new Sector(bVer);
	ll->setLabel(sectorCount++);
	sectors[ll->getLabel()] = ll;
	bVer.clear();
	bnds.clear();
	//v.clear();

	/*===========================================================================================*/
	// Bottom half i.e. the inside of the manifold

	cx = 0;
	cy = -(.05 + _DRAW_HEIGHT/4);

	bnds.push_back(vec2(cx, cy + _DRAW_HEIGHT/4));								//5
	bnds.push_back(vec2(cx - _DRAW_WIDTH/(2*regions), cy + _DRAW_HEIGHT/4));	//1
	bnds.push_back(vec2(cx - _DRAW_WIDTH/(2*regions), cy));						//2
	bnds.push_back(vec2(cx - _DRAW_WIDTH/(4*regions), cy));						//3
	bnds.push_back(vec2(cx, cy + _DRAW_HEIGHT/8));								//4
	for (vector<vec2>::iterator itVec = bnds.begin(); itVec != bnds.end(); itVec++) {
		for (itVer = v.begin(); itVer != v.end(); itVer++) {
			if (itVec[0] == itVer[0]->getPos()) {
				bVer.push_back(itVer[0]);
				break;
			}
		}
		if (itVer == v.end()) {
			bVer.push_back(new Vertex(itVec[0]));
			v.push_back(bVer.back());
		}
	}
	bottomul = new Sector(bVer);
	bottomul->setLabel(sectorCount++);
	sectors[bottomul->getLabel()]= bottomul;
	// Set up partner vertices?
	bVer.clear();
	bnds.clear();
	//v.clear();

	bnds.push_back(vec2(cx + _DRAW_WIDTH/(2*regions), cy + _DRAW_HEIGHT/4));	//10
	bnds.push_back(vec2(cx, cy + _DRAW_HEIGHT/4));								//6
	bnds.push_back(vec2(cx, cy + _DRAW_HEIGHT/8));								//7
	bnds.push_back(vec2(cx + _DRAW_WIDTH/(4*regions), cy));						//8
	bnds.push_back(vec2(cx + _DRAW_WIDTH/(2*regions), cy));						//9
	for (vector<vec2>::iterator itVec = bnds.begin(); itVec != bnds.end(); itVec++) {
		for (itVer = v.begin(); itVer != v.end(); itVer++) {
			if (itVec[0] == itVer[0]->getPos()) {
				bVer.push_back(itVer[0]);
				break;
			}
		}
		if (itVer == v.end()) {
			bVer.push_back(new Vertex(itVec[0]));
			v.push_back(bVer.back());
		}
	}
	bottomur = new Sector(bVer);
	bottomur->setLabel(sectorCount++);
	sectors[bottomur->getLabel()]=bottomur;
	// Partner Vertices?
	bVer.clear();
	bnds.clear();
	//v.clear();
	
	bnds.push_back(vec2(cx+_DRAW_WIDTH/(2*regions),cy));					//14
	bnds.push_back(vec2(cx+_DRAW_WIDTH/(4*regions),cy));					//15
	bnds.push_back(vec2(cx,cy-_DRAW_HEIGHT/8));							//11
	bnds.push_back(vec2(cx,cy-_DRAW_HEIGHT/4));							//12
	bnds.push_back(vec2(cx+_DRAW_WIDTH/(2*regions),cy-_DRAW_HEIGHT/4));	//13
	for (vector<vec2>::iterator itVec = bnds.begin(); itVec != bnds.end(); itVec++) {
		for (itVer = v.begin(); itVer != v.end(); itVer++) {
			if (itVec[0] == itVer[0]->getPos()) {
				bVer.push_back(itVer[0]);
				break;
			}
		}
		if (itVer == v.end()) {
			bVer.push_back(new Vertex(itVec[0]));
			v.push_back(bVer.back());
		}
	}
	bottomlr = new Sector(bVer);	
	bottomlr->setLabel(sectorCount++);
	sectors[bottomlr->getLabel()] = bottomlr;
	// Partner Vertices?
	bVer.clear();
	bnds.clear();
	//v.clear();
	
	bnds.push_back(vec2(cx-_DRAW_WIDTH/(4*regions),cy));					//17
	bnds.push_back(vec2(cx-_DRAW_WIDTH/(2*regions),cy));					//18
	bnds.push_back(vec2(cx-_DRAW_WIDTH/(2*regions),cy-_DRAW_HEIGHT/4));	//19
	bnds.push_back(vec2(cx,cy-_DRAW_HEIGHT/4));							//20
	bnds.push_back(vec2(cx,cy-_DRAW_HEIGHT/8));							//16
	for (vector<vec2>::iterator itVec = bnds.begin(); itVec != bnds.end(); itVec++) {
		for (itVer = v.begin(); itVer != v.end(); itVer++) {
			if (itVec[0] == itVer[0]->getPos()) {
				bVer.push_back(itVer[0]);
				break;
			}
		}
		if (itVer == v.end()) {
			bVer.push_back(new Vertex(itVec[0]));
			v.push_back(bVer.back());
		}
	}
	bottomll = new Sector(bVer);
	bottomll->setLabel(sectorCount++);
	sectors[bottomll->getLabel()] = bottomll;
	// Partner Vertices?
	bVer.clear();
	bnds.clear();
	v.clear(); // don't need all vertices any more
	
	/*===========================================================================================*/
	// Wire up siblings
	vector<HalfEdge *> ulBounds = ul->getBounds();
	vector<HalfEdge *> urBounds = ur->getBounds();
	vector<HalfEdge *> llBounds = ll->getBounds();
	vector<HalfEdge *> lrBounds = lr->getBounds();
	
	ulBounds[4]->setSameSibling(urBounds[1]);
	urBounds[3]->setSameSibling(lrBounds[0]);
	lrBounds[2]->setSameSibling(llBounds[3]);
	llBounds[0]->setSameSibling(ulBounds[2]);
	

	vector<HalfEdge *> bottomulBounds = bottomul->getBounds();
	vector<HalfEdge *> bottomurBounds = bottomur->getBounds();
	vector<HalfEdge *> bottomllBounds = bottomll->getBounds();
	vector<HalfEdge *> bottomlrBounds = bottomlr->getBounds();
	
	bottomulBounds[4]->setSameSibling(bottomurBounds[1]);
	bottomurBounds[3]->setSameSibling(bottomlrBounds[0]);
	bottomlrBounds[2]->setSameSibling(bottomllBounds[3]);
	bottomllBounds[0]->setSameSibling(bottomulBounds[2]);
	

	// Connect top and bottom
	
	ulBounds[0]->setSameSibling(bottomllBounds[2]);
	ulBounds[3]->setSameSibling(bottomllBounds[4]);

	urBounds[0]->setSameSibling(bottomlrBounds[3]);
	urBounds[2]->setSameSibling(bottomlrBounds[1]);

	llBounds[2]->setSameSibling(bottomulBounds[0]);
	llBounds[4]->setSameSibling(bottomulBounds[3]);

	lrBounds[1]->setSameSibling(bottomurBounds[2]);
	lrBounds[3]->setSameSibling(bottomurBounds[0]);


	// Connect right and left

	urBounds[4]->setNextSibling(ulBounds[1]);
	lrBounds[4]->setNextSibling(llBounds[1]);
	bottomurBounds[4]->setNextSibling(bottomulBounds[1]);
	bottomlrBounds[4]->setNextSibling(bottomllBounds[1]);

	urBounds[4]->setOperChange(1);
	ulBounds[1]->setOperChange(-1);
	lrBounds[4]->setOperChange(1);
	llBounds[1]->setOperChange(-1);
	bottomurBounds[4]->setOperChange(1);
	bottomulBounds[1]->setOperChange(-1);
	bottomlrBounds[4]->setOperChange(1);
	bottomllBounds[1]->setOperChange(-1);

}

void Region::draw(vec2 trans) { // Draw each sector in the net in relation to center (0,0)
	for(map<unsigned int, Sector*>::iterator it = sectors.begin(); it!=sectors.end(); it++) {
		it->second->draw(&trans);
		it->second->print();
	}
}

vector<HalfEdge*> Region::getHalfEdges() {
	vector<HalfEdge*> rtn;
	for(map<unsigned int, Sector*>::iterator it = sectors.begin(); it!=sectors.end(); it++) {
		Sector * currentSector = it->second;
		vector<HalfEdge*> bnds = currentSector->getBounds();
		for(vector<HalfEdge*>::iterator iter=bnds.begin(); iter!=bnds.end(); iter++ ) {
			rtn.push_back((*iter));
		}
	}
	return rtn;
}

void Region::addSector(Sector * s){
	//for(vector<HalfEdge*>::iterator it = s->getBounds().begin(); it!=s->getBounds().end(); it++) {
		
	//}
	s->setLabel(sectorCount++);	
	sectors[s->getLabel()] = s;	
}

void Region::removeSector(Sector * s) {
	cout << s->getLabel() << endl;
	sectors.erase(s->getLabel());
}

vector<Vertex*> Region::getVertices() {
	vector<Vertex *> verts;
	for(map<unsigned int, Sector*>::iterator it = sectors.begin(); it!=sectors.end(); it++) {
		if (it->second->getVertex() != NULL) {
			verts.push_back(it->second->getVertex());
		}
	}
	return verts;
}