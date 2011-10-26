#include "group.h"

class HalfEdge;


Group::Group() {

}


Group::Group(unsigned int genus) {
	//center = cen;
	halfRegionWidth = _DRAW_WIDTH/(2*genus);
	for (unsigned int i = 0; i < genus; i++) {
		trans.push_back(vec2((-1 + .1 + (halfRegionWidth) + i*(_DRAW_WIDTH/(genus))), 0.0));
	}
	Region n = Region(genus, trans);
	baseRegion = n;
}

vec2 Group::translateClick(vec2* clickPoint, vec2* nextRegion) {
	// only want the first region.
	oper = 0;
	for (vector<vec2>::iterator it = trans.begin(); it != trans.end(); it++) {
		if (abs((*clickPoint)[0] - (*it)[0])-0.0125 < halfRegionWidth) {
			printf("%f, %f, %f\n", (*clickPoint)[0], (*it)[0], halfRegionWidth);
			if (it+1 == trans.end()) {
				*nextRegion = *clickPoint - trans.front();
			} else {
				*nextRegion = *clickPoint - *(it+1);
			}
			return *clickPoint - *it;
		}
		oper++;
	}
	return NULL;
	/*if (abs((*clickPoint)[0] - (trans[0])[0])-0.0125 < halfRegionWidth) {
		printf("%f, %f, %f\n", (*clickPoint)[0], (trans[0])[0], halfRegionWidth);
		return *clickPoint - trans[0];
	}*/
}

void Group::draw() {
	oper = 0;
	for (vector<vec2>::iterator it = trans.begin(); it != trans.end(); it++) {
		baseRegion.draw(*it);
		oper++;
	}
	//baseRegion.draw(vec2(0.0,0.0));
}