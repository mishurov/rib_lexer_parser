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

#ifndef RIBPARSER_PRIMITIVES_H_
#define RIBPARSER_PRIMITIVES_H_

#include <math.h>

namespace quadrics {

inline float radians(float d) { return d * M_PI / 180; }

inline float degrees(float r) { return r * 180/ M_PI; }

template<typename T>
T SpherePoint(float u, float v, float *args) {
	float radius = args[0];
	float zmin = args[1];
	float zmax = args[2];
	float thetamax = args[3];

	float x, y, z, theta, phi, phimin, phimax;

	thetamax = radians(thetamax);
	phimin = zmin > -radius ? asin(zmin / radius) : radians(-90);
	phimax = zmax < radius ? asin(zmax / radius) : radians(90);
	phi = phimin + v * (phimax - phimin);
	theta = u * thetamax;
	x = radius * cos(theta) * cos(phi);
	y = radius * sin(theta) * cos(phi);
	z = radius * sin(phi);
	return T(x, y, z);
}

template<typename T>
T ConePoint(float u, float v, float *args) {
	float height = args[0];
	float radius = args[1];
	float thetamax = args[2];

	float x, y, z, theta;

	thetamax = radians(thetamax);
	theta = u * thetamax;

	x = radius * (1 - v) * cos(theta);
	y = radius * (1 - v) * sin(theta);
	z = v * height;
	return T(x, y, z);
}

template<typename T>
T CylinderPoint(float u, float v, float *args) {
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
	return T(x, y, z);
}

template<typename T>
T HyperboloidPoint(float u, float v, float *args) {
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
	return T(x, y, z);
}

template<typename T>
T ParaboloidPoint(float u, float v, float *args) {
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

	return T(x, y, z);
}

template<typename T>
T DiskPoint(float u, float v, float *args) {
	float height = args[0];
	float radius = args[1];
	float thetamax = args[2];

	float x, y, z, theta;

	thetamax = radians(thetamax);
	theta = u * thetamax;

	x = radius * (1 - v) * cos(theta);
	y = radius * (1 - v) * sin(theta);
	z = height;
	return T(x, y, z);
}

template<typename T>
T TorusPoint(float u, float v, float *args) {
	float rmajor = args[0];
	float rminor = args[1];
	float phimin = args[2];
	float phimax = args[3];
	float thetamax = args[4];

	float x, y, z, theta, phi, r;

	thetamax = radians(thetamax);
	phimin = radians(phimin);
	phimax = radians(phimax);
	theta = u * thetamax;
	phi = phimin + v * (phimax - phimin);
	r = rminor * cos(phi);

	z = rminor * sin(phi);
	x = (rmajor + r) * cos(theta);
	y = (rmajor + r) * sin(theta);

	return T(x, y, z);
}

} // namespace quadrics

#endif  // RIBPARSER_PRIMITIVES_H_
