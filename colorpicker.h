#ifndef _COLORPICKER_H_
#define _COLORPICKER_H_
#include "global.h"
#include <iostream>
#include "math.h"
#include <vector>
#include "algebra3.h"

#define _CRITICAL 131.0
#define PI 3.14159

class ColorPicker { 
private:
	static float alpha;
public:
	vec3 pickNextColor(void);
};

#endif 