/*
 * global.h
 *
 *  Created on: Feb 2, 2009
 *      Author: njoubert
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define IMPLEMENT_ME(file,line)    cout << "METHOD NEEDS TO BE IMPLEMENTED AT " << file << ":" << line << endl; exit(1);

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <vector>
#include <map>
#include <queue>
#include <cmath>

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

//Include our math library
#include "algebra3.h"
//Include our Image Saving library.
#include "UCB/ImageSaver.h"
//Include the string library
#include <string.h>

/*class Vertex;
class HalfEdge;
static map<string, Vertex> vertexMap;		// Each vertex has a position. For determining if x,y positions already in use.
static map<string, HalfEdge> halfEdgeMap;  // general map for halfedges (unique string ID identifies each halfedge)
*/
#endif /* GLOBAL_H_ */
