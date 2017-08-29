/* ************************************************************************
 * Copyright 2017 Alexander Mishurov
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ************************************************************************/

#include "primitives.h"

float radians(float d) {
	return d * M_PI / 180;
}

float degrees(float r) {
	return r * 180/ M_PI;
}

void PopulatePointArray(MPointArray *ret, int numu, int numv,
			MPoint (*f)(float, float, float *), float *args)
{
	for (int i = 0; i < numu; i++) {
		for (int j = 0; j < numv; j++) {
			float r_u = ((float) rand() / (RAND_MAX)) + 0.5;
			float r_v = ((float) rand() / (RAND_MAX)) + 0.5;
			float step_u = 1.0 / numu;
			float step_v = 1.0 / numv;
			float u = step_u * i + step_u * r_u;
			float v = step_v * j + step_v * r_v;
        		ret->append(f(u, v, args));
		}
	}
}

MPoint SpherePoint(float u, float v, float *args) {
	float radius = args[0];
	float zmin = args[1];
	float zmax = args[2];
	float thetamax = args[3];

	float x, y, z, theta, phi, phimin, phimax;

	thetamax = radians(thetamax);
	phimin = zmin > -radius ? asin(zmin / radius) : -90;
	phimax = zmax < radius ? asin(zmax / radius) : 90;
	phi = phimin + v * (phimax - phimin);
	theta = u * thetamax;
	x = radius * cos(theta) * cos(phi);
	y = radius * sin(theta) * cos(phi);
	z = radius * sin(phi);
	return MPoint(x, y, z);
}

MPoint ConePoint(float u, float v, float *args) {
	float height = args[0];
	float radius = args[1];
	float thetamax = args[2];

	float x, y, z, theta;

	thetamax = radians(thetamax);
	theta = u * thetamax;

	x = radius * (1 - v) * cos(theta);
	y = radius * (1 - v) * sin(theta);
	z = v * height;
	return MPoint(x, y, z);
}

MPoint CylinderPoint(float u, float v, float *args) {
	float radius = args[0];
	float zmin = args[1];
	float zmax = args[2];
	float thetamax = args[3];

	float x, y, z, theta;

	thetamax = radians(thetamax);
	theta = u * thetamax;
	x = radius * cos(theta);
	y = radius * sin(theta);
	z = zmin + v * (zmax - zmin);
	return MPoint(x, y, z);
}

MPoint HyperboloidPoint(float u, float v, float *args) {
	float x1 = args[0];
	float y1 = args[1];
	float z1 = args[2];
	float x2 = args[3];
	float y2 = args[4];
	float z2 = args[5];
	float thetamax = args[6];

	float x, y, z, theta, xr, yr, zr;

	thetamax = radians(thetamax);
	theta = u * thetamax;
	
	xr = (1 - v) * x1 + v * x2;
	yr = (1 - v) * y1 + v * y2;
	zr = (1 - v) * z1 + v * z2;

	x = xr * cos(theta) - yr * sin(theta);
	y = xr * sin(theta) + yr * cos(theta);
	z = zr;
	return MPoint(x, y, z);
}

MPoint ParaboloidPoint(float u, float v, float *args) {
	float rmax = args[0];
	float zmin = args[1];
	float zmax = args[2];
	float thetamax = args[3];

	float x, y, z, theta, r;

	thetamax = radians(thetamax);
	theta = u * thetamax;
	z = zmin + v * (zmax - zmin);
	r = rmax * sqrt(z / zmax);

	x = r * cos(theta);
	y = r * sin(theta);

	return MPoint(x, y, z);
}

MPoint DiskPoint(float u, float v, float *args) {
	float height = args[0];
	float radius = args[1];
	float thetamax = args[2];

	float x, y, z, theta;

	thetamax = radians(thetamax);
	theta = u * thetamax;

	x = radius * (1 - v) * cos(theta);
	y = radius * (1 - v) * sin(theta);
	z = height;
	return MPoint(x, y, z);
}

MPoint TorusPoint(float u, float v, float *args) {
	float rmajor = args[0];
	float rminor = args[1];
	float phimin = args[2];
	float phimax = args[3];
	float thetamax = args[4];

	float x, y, z, theta, phi, r;

	thetamax = radians(thetamax);
	theta = u * thetamax;
	phi = phimin + v * (phimax - phimin);
	r = rminor * cos(phi);

	z = rminor * sin(phi);
	x = (rmajor + r) * cos(theta);
	y = (rmajor + r) * sin(theta);

	return MPoint(x, y, z);
}


MPointArray SpherePoints(int numu, int numv, float radius, float zmin,
				float zmax, float thetamax) {
	MPointArray ret;
	float args[] = { radius, zmin, zmax, thetamax };
	PopulatePointArray(&ret, numu, numv, SpherePoint, args);
	return ret;
}

MPointArray ConePoints(int numu, int numv, float height, float radius,
							float thetamax) {
	MPointArray ret;
	float args[] = { height, radius, thetamax };
	PopulatePointArray(&ret, numu, numv, ConePoint, args);
	return ret;
}

MPointArray CylinderPoints(int numu, int numv, float radius, float zmin,
				float zmax, float thetamax) {
	MPointArray ret;
	float args[] = { radius, zmin, zmax, thetamax };
	PopulatePointArray(&ret, numu, numv, CylinderPoint, args);
	return ret;
}

MPointArray HyperboloidPoints(int numu, int numv, float x1, float y1, float z1,
				float x2, float y2, float z2, float thetamax) {
	MPointArray ret;
	float args[] = { x1, y1, z1, x2, y2, z2, thetamax };
	PopulatePointArray(&ret, numu, numv, HyperboloidPoint, args);
	return ret;
}

MPointArray ParaboloidPoints(int numu, int numv, float rmax, float zmin,
				float zmax, float thetamax) {
	MPointArray ret;
	float args[] = { rmax, zmin, zmax, thetamax };
	PopulatePointArray(&ret, numu, numv, ParaboloidPoint, args);
	return ret;
}

MPointArray DiskPoints(int numu, int numv, float height, float radius,
							float thetamax) {
	MPointArray ret;
	float args[] = { height, radius, thetamax };
	PopulatePointArray(&ret, numu, numv, DiskPoint, args);
	return ret;
}

MPointArray TorusPoints(int numu, int numv, float rmajor, float rminor,
				float phimin, float phimax, float thetamax) {
	MPointArray ret;
	float args[] = { rmajor, rminor, phimin, phimax, thetamax };
	PopulatePointArray(&ret, numu, numv, TorusPoint, args);
	return ret;
}



