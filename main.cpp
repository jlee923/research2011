#include "main.h"

using namespace std;

//****************************************************
// Some Classes
//****************************************************
class Viewport {
public:
    Viewport(): mousePos(0.0,0.0) { /* empty */ };
	int w, h; // width and height
	vec2 mousePos;
};

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;
UCB::ImageSaver * imgSaver;
Group instance;
int genus;
Sector *s,*p;
// pathing
bool split = false, merging = false, path = false, newPath = false;
// vertices
bool addVertex = false, clickVert;

/* pathEdge corresponds to: (after both edges split, before sector division)
 *  |                    ^
 *  | <-- this edge      |
 *  v                    |
 *  |                    ^
 *  |                    |
 *  v                    |
 */
HalfEdge * pathEdge = NULL;// * endVertEdge = NULL;
Vertex * startVertex = NULL, * endVertex = NULL;
vector<HalfEdge*> endVertEdges;
vec2 prevClick; // world coordinates of previous click
int oper = 0; // current region that is being operated on

//-------------------------------------------------------------------------------
/// Functions
///

// makes the sectors for the given genus object. Sectors made, left-to-right top-to-bottom
// which translates to counter-clockwise on the genus object top-then-bottom/outside-inside


//-------------------------------------------------------------------------------
/// You will be calling all of your drawing-related code from this function.
/// Nowhere else in your code should you use glBegin(...) and glEnd() except code
/// called from this method.
///
/// To force a redraw of the screen (eg. after mouse events or the like) simply call
/// glutPostRedisplay();
void display() {

	//Clear Buffers
    glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"
	
	instance.draw();

	//Now that we've drawn on the buffer, swap the drawing buffer and the displaying buffer.
	glutSwapBuffers();

}


//-------------------------------------------------------------------------------
/// \brief	Called when the screen gets resized.
/// This gives you the opportunity to set up all the relevant transforms.
///
void reshape(int w, int h) {
	//Set up the viewport to ignore any size change.
	glViewport(0,0,viewport.w,viewport.h);

	//Set up the PROJECTION transformationstack to be a simple orthographic [-1, +1] projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, 1, -1);	// resize type = stretch

	//Set the MODELVIEW transformation stack to the identity.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//-------------------------------------------------------------------------------
/// Called to handle keyboard events.
void myKeyboardFunc (unsigned char key, int x, int y) {
	switch (key) {
		case 27:			// Escape key
			exit(0);
			break;

		case 'n':
			cout << "Starting a new path" << endl;
			path = true;
			//newPath = true;
			break;

		case 'e':
			cout << "Ending path" << endl;
			if (newPath && (pathEdge != NULL)) {
				// revert edges..?
			}
			path = false;
			prevClick = NULL;
			break;

		case 'u':
			// should be able to make this more efficient
			// also need to handle undo back to vertex
			if (!path && endVertEdges.size() != 0) {
				instance.removeSector(Sector::undoPath(pathEdge, endVertEdges.back()));
				
				path = true;
				endVertEdges.pop_back();
				glutPostRedisplay();
				break;
			} else if (!path) {
				cout << "nothing to undo (path deleted)" << endl;
				break;
			} else {
				cout << "Undo last move" << endl;
				if (startVertex != NULL) {
					map<unsigned int,Sector*> sects = instance.getSectors();
					for(map<unsigned int,Sector*>::iterator it=sects.begin(); it!=sects.end(); it++){
						if (it->second->getVertex() == startVertex) {
							it->second->setVertex(NULL);
							break;
						}
					}
					addVertex = false;
					path = false;
					newPath = false;
					pathEdge = NULL;
					prevClick = NULL;
					startVertex = NULL;
					glutPostRedisplay();
					break;
				}
				// might not need this check
				HalfEdge * deletedEdge = pathEdge->getForwardEdge()->getSibling()->getForwardEdge()->getSibling();
				if (deletedEdge == pathEdge) {
					cout << "revertting original edge and ending path" << endl;
					Sector::mergeEdges(pathEdge);
					path = false;
					prevClick = NULL;
					pathEdge = NULL;
					glutPostRedisplay();
					break;
				}
				HalfEdge * newPathEdge, * forSibEdge;
				newPathEdge = pathEdge->getSibling();
				forSibEdge = pathEdge->getForwardEdge()->getSibling();
				if (forSibEdge->getSector() == newPathEdge->getSector()) {
					forSibEdge->setForwardEdge(newPathEdge);
					Sector::mergeEdges(pathEdge);
					pathEdge = NULL;
					startVertex = deletedEdge->getVertex();
					newPath = true;
				} else {
					while(newPathEdge->getForwardEdge() != deletedEdge) {
						newPathEdge = newPathEdge->getForwardEdge();
					}
					instance.removeSector(Sector::undoPath(pathEdge));
					pathEdge = newPathEdge;
				}
				cout << "success?" << endl;
				glutPostRedisplay();
				break;
			}

		case 'v':
			cout << "Ending path (if applicable) and adding vertex" << endl;
			addVertex = true;
			path = false;
			prevClick = NULL;
			break;

		case 'f':
			cout << "flooding" << endl;
			HalfEdge * pathLocals = pathEdge;
			HalfEdge * endVertLocals = endVertEdges.back();
			queue<pair<int, Sector*>> floodQueue;
			queue<pair<int, Sector*>> startSectors;
			vector<pair<int, Sector*>> doneSectors;
			Sector * startSector = pathEdge->getSector();
			for (int i = 0; i < genus; i ++) {
				startSectors.push(pair<int, Sector*>(i, startSector));
			}
			// XXX when fix colorpicker, will need to adjust which colors to use here
			ColorPicker cp;
			vec3 floodColor;
			pair<int, Sector*> tempPair, pushPair;
			while(!startSectors.empty()) {
				floodColor = cp.pickNextColor();
				// need this first push because for loop shouldn't execute initially
				floodQueue.push(startSectors.front());
				for (vector<pair<int, Sector*>>::iterator it = doneSectors.begin(); it != doneSectors.end(); it ++) {
					if (startSectors.front().second == (*it).second && startSectors.front().first == (*it).first) {
						// first element should be next starting sector
						floodQueue.pop();
						break;
					}
				}
				startSectors.pop();
				while(!floodQueue.empty()) {
					tempPair = floodQueue.front();
					doneSectors.push_back(tempPair);
					tempPair.second->fill(floodColor, tempPair.first);
					floodQueue.pop();
					vector<HalfEdge*> nextEdges = tempPair.second->getPermiableEdges();
					for (vector<HalfEdge*>::iterator it = nextEdges.begin(); it != nextEdges.end(); it++) {
						int operReg = tempPair.first+(*it)->getOperChange();
						if (operReg < 0) operReg = genus-1;
						else if(operReg == genus) operReg = 0;
						pushPair = pair<int, Sector*>(operReg, (*it)->getSibling()->getSector());
						for (vector<pair<int, Sector*>>::iterator it = doneSectors.begin(); it != doneSectors.end(); it ++) {
							if (pushPair.second == (*it).second && pushPair.first == (*it).first) {
								break;
							} else if (it + 1 == doneSectors.end()) {
								floodQueue.push(pushPair);
							}
						}
						
					}
				}
			}

			cout << "testing a b" << endl;
			glutPostRedisplay();
			break;

		/*
		case 's':
			cout << "Spliting ON" << endl;
			split = true;
			pathEdge = splitEdge1 = splitEdge2 = NULL;
			splitSectorEdges.clear();
			cout << "with splitsectur edges " << splitSectorEdges.empty() << endl;
			break;
		case 'm':
			cout << "Merging ON" << endl;
			merging = true;
			mergeEdge = NULL;
			break;
		case 'p':
			cout << "Connect Path ON" << endl;
			path = true;
			pathEdge = splitEdge1 = splitEdge2 = NULL;
			splitSectorEdges.clear();
			pathSectorEdges.clear();
			break;
			*/
	}
}


//-------------------------------------------------------------------------------
/// This function is used to convert the pixel coordinates on the screen to the
/// coordinates of your view volume. Thus, it converts a pixel location to a coordinate
/// that can be used for picking and selection.
vec2 inverseViewportTransform(vec2 screenCoords) {
    //Create a vec2 on the local stack. See algebra3.h
    vec2 viewCoords(0.0,0.0);

    viewCoords[0] = ((float)screenCoords[0] - viewport.w/2)/((float)(viewport.w/2));
    viewCoords[1] = ((float)screenCoords[1] - viewport.h/2)/((float)(viewport.h/2));
    //Flip the values to get the correct position relative to our coordinate axis.
    viewCoords[1] = -viewCoords[1];
    //C++ will copy the whole vec2 to the calling function.
    return viewCoords;
}


//-------------------------------------------------------------------------------
///
void myMouseFunc( int button, int state, int x, int y ) {
    //Convert the pixel coordinates to view coordinates.
    vec2 screenCoords((double) x, (double) y);
    vec2 viewCoords = inverseViewportTransform(screenCoords);

	if ( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) {
		HalfEdge * edgeFound = NULL; 
		vec2 worldClick = inverseViewportTransform(vec2((double) x,(double)y));

		printf("%d, %d, %f, %f\n", x, y, worldClick[0], worldClick[1]);
		//cout << "Mouseclick at " << viewCoords[0] << "," << viewCoords[1] << "." << endl;
		//cout << "Clicked at world coord " << worldClick << endl;
		vec2 newClick2;
		vec2 newClick1 = instance.translateClick(&worldClick, &newClick2);
		
		vector<HalfEdge*> edges;
		edges = instance.getHalfEdges();
		cout << "Getting all halfedges" << endl;

		for(vector<HalfEdge*>::iterator it=edges.begin();it!=edges.end();it++){
			if ((*it)->pointOnEdge(newClick1) || (*it)->pointOnEdge(newClick2)) {
				edgeFound = *it;
				HalfEdge * blah = edgeFound;
				cout << "Found Edge: " << edgeFound->getID() << "; sibling is: " << edgeFound->getSibling()->getID() 
					 << "; forward is: " << edgeFound->getForwardEdge()->getID() 
					 << "; with sector: " << edgeFound->getSector()->getLabel()<< endl;
			}
		}

		// use a for loop and a map<unsigned int, vertex> implementation (or vector)
		vector<Vertex*> verts = instance.getVertices();
		for (vector<Vertex*>::iterator it = verts.begin(); it != verts.end(); it++) {
			if ((*it)->pointOnVert(newClick1)) {
				endVertex = *it;
				clickVert = true;
				edgeFound = NULL;
				break;
			}
		}
		
		if ((path || startVertex != NULL) && (edgeFound != NULL)) {
			if (!edgeFound->canPass()) {
				cout << "NOT ALLOWED TO PASS THROUGH HERE" << endl;
				return;
			}
			if (prevClick == NULL && startVertex == NULL) {
				prevClick = worldClick;
				cout << "Split the clicked edge" << endl;
				// do it here?
				pathEdge = edgeFound->getSector()->splitEdge(edgeFound);
				newPath = true;
			} else {
				//cout << worldClick[0] << " blahblahblah " << prevClick[0] << endl;.
				if (newPath) {
					cout << "in the new path phase" << endl;
					newPath = false;
					if (startVertex != NULL) {
						if (edgeFound->getSector()->getVertex() != startVertex) {
							edgeFound = edgeFound->getSibling();
						}

						pathEdge = edgeFound->getSector()->startPath(startVertex, edgeFound);
						prevClick = worldClick;

						startVertex = NULL;
						glutPostRedisplay();
						return;
					}
					else {
					cout << "No vertex has been specified to start a new path" << endl;
					}
					/* Simply do a first-edge orientation.  Then, don't need to such a long loop for the
					 * rest of the path.
					 * 
					 * the numbers might need to be updated, if you use a change of dimens --> might want 
					 * to make that a global thing.
					 */
					/*if (worldClick[0] - prevClick[0] > 0.0125) {
					cout << "going right" << endl;
					HalfEdge * locals = pathEdge;
					while (!(Sector::isSameSector(pathEdge, edgeFound) && 
						edgeFound->getPos(1.0)[0] >= min(pathEdge->getPos(0.0)[0], pathEdge->getPos(1.0)[0]))) {	
						if (Sector::isSameSector(pathEdge, edgeFound->getSibling()) &&
							edgeFound->getSibling()->getPos(0.0)[0] >= min(pathEdge->getPos(0.0)[0], pathEdge->getPos(1.0)[0])) {
								edgeFound = edgeFound->getSibling();
								//break;
						} 
						else if (Sector::isSameSector(pathEdge->getForwardEdge()->getSibling(), edgeFound) &&
							edgeFound->getPos(0.0)[0] >= min(pathEdge->getForwardEdge()->getSibling()->getPos(0.0)[0], pathEdge->getForwardEdge()->getSibling()->getPos(1.0)[0])) {
								pathEdge = pathEdge->getForwardEdge()->getSibling();
								//break;
						} 
						else if (Sector::isSameSector(pathEdge->getForwardEdge()->getSibling(), edgeFound->getSibling()) &&
							edgeFound->getSibling()->getPos(0.0)[0] >= min(pathEdge->getForwardEdge()->getSibling()->getPos(0.0)[0], pathEdge->getForwardEdge()->getSibling()->getPos(1.0)[0])) {
								edgeFound = edgeFound->getSibling();
								pathEdge = pathEdge->getForwardEdge()->getSibling();
								//break;
						} 
						else {
							cout << "failed" << endl;
							return;
						}
					}
					if (edgeFound == pathEdge) {
						cout << "same edge" << endl;
						return;
					}	
					edgeFound->getSector()->splitEdge(edgeFound);
					////
					cout << "Found Edge: " << edgeFound->getID() << "; sibling is: " << edgeFound->getSibling()->getID() 
					 << "; forward is: " << edgeFound->getForwardEdge()->getID() 
					 << "; with sector: " << edgeFound->getSector()->getLabel()<< endl;
					cout << "Prev Edge: " << pathEdge->getID() << "; sibling is: " << pathEdge->getSibling()->getID() 
					 << "; forward is: " << pathEdge->getForwardEdge()->getID() 
					 << "; with sector: " << pathEdge->getSector()->getLabel()<< endl;
					///
					HalfEdge * pathForward = edgeFound->getForwardEdge()->getSibling();
					instance.addSector(pathEdge->getSector()->divide(pathEdge, edgeFound));
					
					pathEdge = pathForward;
					if (worldClick[0] > 0.9 - 0.0125) {
						prevClick = worldClick;
						prevClick[0] = -prevClick[0];
					} else {
						prevClick = worldClick;
					}

					if (abs(worldClick[1]) > 0.85 - 0.0125) {
						prevClick = worldClick;
						prevClick[1] = -prevClick[1];
					} else {
						prevClick = worldClick;
					}

				} 
				else if (worldClick[0] - prevClick[0] < -0.0125) {
					// click went left
					cout << "click went left" << endl;
					while (!(Sector::isSameSector(pathEdge, edgeFound) &&
						edgeFound->getPos(1.0)[0] <= max(pathEdge->getPos(0.0)[0], pathEdge->getPos(1.0)[0]))) {
						if (Sector::isSameSector(pathEdge, edgeFound->getSibling()) &&
							edgeFound->getSibling()->getPos(1.0)[0] <= max(pathEdge->getPos(0.0)[0], pathEdge->getPos(1.0)[0])) {
							edgeFound = edgeFound->getSibling();
						} 
						else if (Sector::isSameSector(pathEdge->getForwardEdge()->getSibling(), edgeFound) &&
							edgeFound->getPos(0.0)[0] <= max(pathEdge->getForwardEdge()->getSibling()->getPos(0.0)[0], pathEdge->getForwardEdge()->getSibling()->getPos(1.0)[0])) {
								pathEdge = pathEdge->getForwardEdge()->getSibling();
								break;
						} 
						else if (Sector::isSameSector(pathEdge->getForwardEdge()->getSibling(), edgeFound->getSibling()) &&
							edgeFound->getSibling()->getPos(0.0)[0] <= max(pathEdge->getForwardEdge()->getSibling()->getPos(0.0)[0], pathEdge->getForwardEdge()->getSibling()->getPos(1.0)[1])) {
								edgeFound = edgeFound->getSibling();
								pathEdge = pathEdge->getForwardEdge()->getSibling();
								break;
						} else {
							cout << "failed" << endl;
							return;
						}
					}
					if (edgeFound == pathEdge) {
						cout << "same edge" << endl;
						return;
					}
					edgeFound->getSector()->splitEdge(edgeFound);
					/////
					cout << "Found Edge: " << edgeFound->getID() << "; sibling is: " << edgeFound->getSibling()->getID() 
					 << "; forward is: " << edgeFound->getForwardEdge()->getID() 
					 << "; with sector: " << edgeFound->getSector()->getLabel()<< endl;
					cout << "Prev Edge: " << pathEdge->getID() << "; sibling is: " << pathEdge->getSibling()->getID() 
					 << "; forward is: " << pathEdge->getForwardEdge()->getID() 
					 << "; with sector: " << pathEdge->getSector()->getLabel()<< endl;
					////
					HalfEdge * pathForward = edgeFound->getForwardEdge()->getSibling();
					instance.addSector(pathEdge->getSector()->divide(pathEdge, edgeFound));
					
					pathEdge = pathForward;
					if (worldClick[0] < -0.9 + 0.0125) {
						prevClick = worldClick;
						prevClick[0] = -prevClick[0];
					} else {
						prevClick = worldClick;
					}

					if (abs(worldClick[1]) > 0.85 - 0.0125) {
						prevClick = worldClick;
						prevClick[1] = -prevClick[1];
					} else {
						prevClick = worldClick;
					}
				} 
			///////////////////////////////////////////////////////////
				else if (abs(worldClick[0] - prevClick[0]) < 0.0125) {
					// click has same x coordinate, depend on y
					cout << "click directly up or down" << endl;
					if (worldClick[1] - prevClick[1] > 0.0125) {
						//click up
						cout << "click up" << endl;
						while (!(Sector::isSameSector(pathEdge, edgeFound) &&
							edgeFound->getPos(0.0)[1] >= pathEdge->getPos(0.0)[1])) {
							if (Sector::isSameSector(pathEdge, edgeFound->getSibling()) &&
								edgeFound->getSibling()->getPos(0.0)[1] > pathEdge->getPos(0.0)[1]) {
								edgeFound = edgeFound->getSibling();
							} 
							else if (Sector::isSameSector(pathEdge->getForwardEdge()->getSibling(), edgeFound) &&
								edgeFound->getPos(0.0)[0] >= min(pathEdge->getForwardEdge()->getSibling()->getPos(0.0)[1], pathEdge->getForwardEdge()->getSibling()->getPos(1.0)[1])) {
									pathEdge = pathEdge->getForwardEdge()->getSibling();
									//break;
							} 
							else if (Sector::isSameSector(pathEdge->getForwardEdge()->getSibling(), edgeFound->getSibling()) &&
								edgeFound->getSibling()->getPos(0.0)[0] >= min(pathEdge->getForwardEdge()->getSibling()->getPos(0.0)[1], pathEdge->getForwardEdge()->getSibling()->getPos(1.0)[1])) {
									edgeFound = edgeFound->getSibling();
									pathEdge = pathEdge->getForwardEdge()->getSibling();
									//break;
							} else {
								cout << "failed" << endl;
								return;
							}
						}
						if (edgeFound == pathEdge) {
							cout << "same edge" << endl;
							return;
						}
						edgeFound->getSector()->splitEdge(edgeFound);
						/////
						cout << "Found Edge: " << edgeFound->getID() << "; sibling is: " << edgeFound->getSibling()->getID() 
						 << "; forward is: " << edgeFound->getForwardEdge()->getID() 
						 << "; with sector: " << edgeFound->getSector()->getLabel()<< endl;
						cout << "Prev Edge: " << pathEdge->getID() << "; sibling is: " << pathEdge->getSibling()->getID() 
						 << "; forward is: " << pathEdge->getForwardEdge()->getID() 
						 << "; with sector: " << pathEdge->getSector()->getLabel()<< endl;
						////
						HalfEdge * pathForward = edgeFound->getForwardEdge()->getSibling();
						instance.addSector(pathEdge->getSector()->divide(pathEdge, edgeFound));
					
						pathEdge = pathForward;
						if (worldClick[0] < -0.9 + 0.0125) {
							prevClick = worldClick;
							prevClick[0] = -prevClick[0];
						} else {
							prevClick = worldClick;
						}

						if (abs(worldClick[1]) > 0.85 - 0.0125) {
							prevClick = worldClick;
							prevClick[1] = -prevClick[1];
						} else {
							prevClick = worldClick;
						}


					} else if (worldClick[1] - prevClick[1] < -0.0125) {
						// click down
						cout << "click down" << endl;
						while (!(Sector::isSameSector(pathEdge, edgeFound) &&
							edgeFound->getPos(0.0)[1] <= pathEdge->getPos(0.0)[1])) {
							if (Sector::isSameSector(pathEdge, edgeFound->getSibling()) &&
								edgeFound->getSibling()->getPos(0.0)[1] < pathEdge->getPos(0.0)[1]) {
									edgeFound = edgeFound->getSibling();
							} else if (Sector::isSameSector(pathEdge->getForwardEdge()->getSibling(), edgeFound) &&
								edgeFound->getPos(0.0)[0] <= max(pathEdge->getForwardEdge()->getSibling()->getPos(0.0)[0], pathEdge->getForwardEdge()->getSibling()->getPos(1.0)[0])) {
									pathEdge = pathEdge->getForwardEdge()->getSibling();
									//break;
							} 
							else if (Sector::isSameSector(pathEdge->getForwardEdge()->getSibling(), edgeFound->getSibling()) &&
								edgeFound->getSibling()->getPos(0.0)[0] <= max(pathEdge->getForwardEdge()->getSibling()->getPos(0.0)[0], pathEdge->getForwardEdge()->getSibling()->getPos(1.0)[1])) {
									edgeFound = edgeFound->getSibling();
									pathEdge = pathEdge->getForwardEdge()->getSibling();
									//break;
							} else {
								cout << "failed" << endl;
								return;
							}
						}
						if (edgeFound == pathEdge) {
							cout << "same edge" << endl;
							return;
						}
						edgeFound->getSector()->splitEdge(edgeFound);
						/////
						cout << "Found Edge: " << edgeFound->getID() << "; sibling is: " << edgeFound->getSibling()->getID() 
						 << "; forward is: " << edgeFound->getForwardEdge()->getID() 
						 << "; with sector: " << edgeFound->getSector()->getLabel()<< endl;
						cout << "Prev Edge: " << pathEdge->getID() << "; sibling is: " << pathEdge->getSibling()->getID() 
						 << "; forward is: " << pathEdge->getForwardEdge()->getID() 
						 << "; with sector: " << pathEdge->getSector()->getLabel()<< endl;
						////
						HalfEdge * pathForward = edgeFound->getForwardEdge()->getSibling();
						instance.addSector(pathEdge->getSector()->divide(pathEdge, edgeFound));
						
						pathEdge = pathForward;
						if (worldClick[0] < -0.9 + 0.0125) {
							prevClick = worldClick;
							prevClick[0] = -prevClick[0];
						} else {
							prevClick = worldClick;
						}

						if (abs(worldClick[1]) > 0.85 - 0.0125) {
							prevClick = worldClick;
							prevClick[1] = -prevClick[1];
						} else {
							prevClick = worldClick;
						}

					} else {
						// You clicked the exact same place
						cout << "you clicked the same location" << endl;
					}
				} 
			///////////////////////////////////////////////////////////
				else {
					// default case?
					cout << "um.... ?" << endl;
				}*/
				} else {
					if (!pathEdge->getSector()->isSameSector(pathEdge, edgeFound)) {
						if (pathEdge->getSector()->isSameSector(pathEdge, edgeFound->getSibling())) {
							edgeFound = edgeFound->getSibling();
						} else {
							cout << "invalid edge" << endl;
							return;
						}
					}
					if (edgeFound == pathEdge || edgeFound == pathEdge->getForwardEdge()) {
						cout << "same edge" << endl;
						return;
					}
					edgeFound->getSector()->splitEdge(edgeFound);

					cout << "Found Edge: " << edgeFound->getID() << "; sibling is: " << edgeFound->getSibling()->getID() 
						<< "; forward is: " << edgeFound->getForwardEdge()->getID() 
						<< "; with sector: " << edgeFound->getSector()->getLabel()<< endl;
					cout << "Prev Edge: " << pathEdge->getID() << "; sibling is: " << pathEdge->getSibling()->getID() 
						<< "; forward is: " << pathEdge->getForwardEdge()->getID() 
						<< "; with sector: " << pathEdge->getSector()->getLabel()<< endl;
					////
					HalfEdge * pathForward = edgeFound->getForwardEdge()->getSibling();
					instance.addSector(pathEdge->getSector()->divide(pathEdge, edgeFound));
				
					pathEdge = pathForward;
					if (worldClick[0] < -0.9 + 0.0125) {
						prevClick = worldClick;
						prevClick[0] = -prevClick[0];
					} else {
						prevClick = worldClick;
					}

					if (abs(worldClick[1]) > 0.85 - 0.0125) {
						prevClick = worldClick;
						prevClick[1] = -prevClick[1];
					} else {
						prevClick = worldClick;
					}

				}
			cout << "reached end of if" << endl;
			}
		}
		else if (clickVert && path) {
			cout << "I want to end the path" << endl;
			HalfEdge * closestInEdge = pathEdge->getForwardEdge();
			while (closestInEdge->getForwardEdge()->getVertex() != endVertex) {
				closestInEdge = closestInEdge->getForwardEdge();
			}
			instance.addSector(pathEdge->getSector()->divide(pathEdge, closestInEdge));
			path = false;
			clickVert = false;
			endVertEdges.push_back(closestInEdge);
		} 
		else {
			if (path) {
				cout << "AM I HERE??" << endl;
				// I have a path... so, place an intermediate vertex at the specified location --> do it here?
			}
			cout << "IN THE ELSE CASE" << endl;
			map<unsigned int,Sector*> sectors = instance.getSectors();
			for(map<unsigned int,Sector*>::iterator it=sectors.begin(); it!=sectors.end(); it++){
				if(it->second->pointInSector(newClick1)) { // won't be newClick2, since that is untranslated
					
					if ((split || (path && pathEdge != NULL))) {
						//splitSectorEdges = it->second->getBounds();
						cout << " try here " << endl;
					}
					else if (path) {// && pathSectorEdges.empty()){
						//pathSectorEdges = it->second->getBounds();
						cout << " it got here " << endl;
					}
					vector<HalfEdge*> b = it->second->getBounds();
					cout << "Found Sector "<< it->second->getLabel()<< " with:" << endl;
					for(vector<HalfEdge*>::iterator jt = b.begin();jt!=b.end();jt++) {
						cout << "\tedge " << (*jt)->getID() << endl;
					}
						
					if (addVertex && edgeFound == NULL) {
						Vertex* startPoint = new Vertex(newClick1);
						it->second->setVertex(startPoint);
						addVertex = false;
						startVertex = startPoint;
						newPath = true;
						path = true;
					} else {
						cout << "clicked too close to edge, please try again" << endl;
								
					}		
				}	
			}	
				
		}

		/*
		vector<HalfEdge*> edges;
		if ((split || (path && pathEdge != NULL)) && !splitSectorEdges.empty()) {
			edges = splitSectorEdges;
			cout << "LOOK HERE" << endl;
		}
		else if (path && !pathSectorEdges.empty())
			edges = pathSectorEdges;
		else {
			edges = instance.getHalfEdges();
			cout << "LOOK HERE INSTEAD " << path << " " << !pathSectorEdges.empty() << endl;
		}
		for(vector<HalfEdge*>::iterator it=edges.begin();it!=edges.end();it++){
			if ((*it)->pointOnEdge(newClick1) || (*it)->pointOnEdge(newClick2)){
				edgeFound = *it;
				cout << "Found Edge: " << edgeFound->getID() << "; sibling is: " << edgeFound->getSibling()->getID() 
					 << "; forward is: " << edgeFound->getForwardEdge()->getID() 
					 << "; with sector: " << edgeFound->getSector()->getLabel()<< endl;
			}
		}
		if (edgeFound != NULL){
			if (path && pathEdge == NULL) {
				pathEdge = edgeFound;
				cout << "Path Edge 1: " << pathEdge->getID() << " starting at: " << pathEdge->getPos(0.0) 
					 << " ending at: " << pathEdge->getForwardEdge()->getPos(0.0) 
					 << " sibling is: " << pathEdge->getSibling()->getID() << endl;
			} else if (merging) {
				// re-do memory in region?
				instance.removeSector(edgeFound->getSector());
				instance.removeSector(edgeFound->getSibling()->getSector());
				instance.addSector(Sector::merge(edgeFound));
				merging = false;
				cout << "Merge OFF " << endl;
			} else if ((split || path) && splitEdge1 == NULL) {
				splitEdge1 = edgeFound;
				cout << "Split Edge 1: " << splitEdge1->getID() << " starting at: " << splitEdge1->getPos(0.0) 
					 << " ending at: " << splitEdge1->getForwardEdge()->getPos(0.0) 
					 << " sibling is: " << splitEdge1->getSibling()->getID() << endl;
			} else if (split) {
				if (edgeFound == splitEdge1) {
					cout << "Picked same edge, try again" << endl;
				} else {
					if (splitEdge1->getSector() == edgeFound->getSector()) {
						//cool.
					} else if (splitEdge1->getSector() == edgeFound->getSibling()->getSector()) {
						edgeFound = edgeFound->getSibling();
					} else if (splitEdge1->getSibling()->getSector() == edgeFound->getSector()) {
						splitEdge1 = splitEdge1->getSibling();
					} else if (splitEdge1->getSibling()->getSector() == edgeFound->getSibling()->getSector()){
						splitEdge1 = splitEdge1->getSibling();
						edgeFound = edgeFound->getSibling();
					}
					
					splitEdge2 = edgeFound;
					cout << "Split Edge 2: " << splitEdge2->getID() << " starting at: " << splitEdge2->getPos(0.0) 
						 << " ending at: " << splitEdge2->getForwardEdge()->getPos(0.0) 
						 << " sibling is: " << splitEdge2->getSibling()->getID() << endl;
					instance.addSector(Sector::split(splitEdge1, splitEdge2));
					split = false;
					cout << "Splitting OFF" << endl;
				}
				
			} else if (path) {
				splitEdge2 = edgeFound;
			
				cout << "PATH: Split Edge 2: " << splitEdge2->getID() << " starting at: " << splitEdge2->getPos(0.0) << " ending at: " << splitEdge2->getForwardEdge()->getPos(0.0) 
					<< " sibling is: " << splitEdge2->getSibling()->getID() << endl;
				Sector * sect = Sector::split(pathEdge,splitEdge1,splitEdge2);
				if (sect != NULL) {
					cout << "did not make a sector " << endl;
					instance.addSector(sect);
				}
				path = false;
				cout << "Path OFF" << endl;
			}
		} else {
			map<unsigned int,Sector*> sectors = instance.getSectors();
			for(map<unsigned int,Sector*>::iterator it=sectors.begin(); it!=sectors.end(); it++){
				if(it->second->pointInSector(newClick1)) { // won't be newClick2, since it's the next region
				
					if ((split || (path && pathEdge != NULL)) && splitSectorEdges.empty()) {
						splitSectorEdges = it->second->getBounds();
						cout << " try here " << endl;
					}
					else if (path && pathSectorEdges.empty()){
						pathSectorEdges = it->second->getBounds();
						cout << " it got here " << endl;
					}
					vector<HalfEdge*> b = it->second->getBounds();
					cout << "Found Sector "<< it->second->getLabel()<< " with:" << endl;
					for(vector<HalfEdge*>::iterator jt = b.begin();jt!=b.end();jt++) {
						cout << "\tedge " << (*jt)->getID() << endl;
					}
				}
			}
		}*/
	}

	if ( button==GLUT_LEFT_BUTTON && state==GLUT_UP ) {

	   //YOUR CODE HERE
	    //Possibly do something with the tempVertex?

	}

	if ( button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN ) {
	    //Save a window capture to disk as a BITMAP file.

	    //YOUR CODE HERE

	    //Move this somewhere more convenient!
	    imgSaver->saveFrame(viewport.w, viewport.h);
	}

	//Force a redraw of the window
	glutPostRedisplay();

}


//-------------------------------------------------------------------------------
/// Called whenever the mouse moves while a button is pressed
void myActiveMotionFunc(int x, int y) {
    //Record the mouse location for drawing crosshairs
    viewport.mousePos = inverseViewportTransform(vec2((double)x,(double)y));

  //  if (tempVertex != NULL)
    //    tempVertex->setStartPos(viewport.mousePos);

    //Force a redraw of the window.
    glutPostRedisplay();
}


//-------------------------------------------------------------------------------
/// Called whenever the mouse moves without any buttons pressed.
void myPassiveMotionFunc(int x, int y) {
    //Record the mouse location for drawing crosshairs
    viewport.mousePos = inverseViewportTransform(vec2((double)x,(double)y));

    //Force a redraw of the window.
    glutPostRedisplay();
}


//-------------------------------------------------------------------------------
/// Initialize the environment
int main(int argc,char** argv) {
	cout << "Top edge connected to bottom edge (center hole gives +1 to genus)\n";
	cout << "Please enter desired genus: ";
	cin >> genus;
	genus --;
	prevClick = NULL;

	//Initialize OpenGL
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);

	//Set up global variables
	viewport.w = 800;
	viewport.h = 800;
	
	/*
	if (argc < 2) {
	    cout << "USAGE: morph poly.obj" << endl;
	    exit(1);
	}
	
	int argv1;
	stringstream(argv[1]) >> argv1;
//	map->makeSectors(argv1);
	*/
	//Initialize the screen capture class to save BMP captures
	//in the current directory, with the prefix "morph"
	imgSaver = new UCB::ImageSaver("./", "map");

	//Create OpenGL Window
	glutInitWindowSize(viewport.w,viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow("HandleMap");
	glClearColor(0.3f,0.3f,0.3f,0.0f);
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glLineStipple(1, (short) 0x5555);

	
	//Register event handlers with OpenGL.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(myKeyboardFunc);
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(myActiveMotionFunc);
	glutPassiveMotionFunc(myPassiveMotionFunc);

	//Make Groups
	instance = Group(genus);

	//And Go!
	glutMainLoop();
}
