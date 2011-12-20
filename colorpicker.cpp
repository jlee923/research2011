#include "colorpicker.h"

vec3 ColorPicker::pickNextColor(void){
		vector <float> color(3);
		float beta, val;
		int i, j, k;

		if (alpha < 60) {
			beta = alpha;
			i = 0; j = 1; k = 2;
		} else if (alpha < 120) {
			beta = 120 - alpha;
			i = 1; j = 0; k = 2;
		} else if (alpha < 180) {
			beta = alpha - 120;
			i = 1; j = 2; k = 0;
		} else if (alpha < 240) {
			beta = 240 - alpha;
			i = 2; j = 1; k = 0;
		} else if (alpha < 300) {
			beta = alpha - 240;
			i = 2; j = 0; k = 1;
		} else {
			beta = 360 - alpha;
			i = 0; j = 2; k = 1;
		}

		if (beta < 30) {
			val = (0.5 - (((sqrt (3.0)) * tan ((30 - beta) * (PI / 180))) / 2));
		} else {
			val = (0.5 + (((sqrt (3.0)) * tan ((beta - 30) * (PI / 180))) / 2));
		}

		color[i] = 1.0;
		color[j] = val;
		color[k] = 0.0;
	
		if (alpha + _CRITICAL >= 360) {
			alpha = alpha + _CRITICAL - 360;
		} else {
			alpha = alpha + _CRITICAL;
		}
		vec3 colorV(color[0],color[1],color[2]);
		return colorV;
}


float ColorPicker::alpha = 0.0;