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

#ifndef MAYAPLUGIN_MAYA_PRIMITIVES_H_
#define MAYAPLUGIN_MAYA_PRIMITIVES_H_

#include <maya/MPointArray.h>


MPointArray SpherePoints(int numu, int numv, float radius, float zmin,
				float zmax, float thetamax);

MPointArray ConePoints(int numu, int numv, float height, float radius,
							float thetamax);

MPointArray CylinderPoints(int numu, int numv, float radius, float zmin,
				float zmax, float thetamax);
				
MPointArray HyperboloidPoints(int numu, int numv, float x1, float y1, float z1,
				float x2, float y2, float z2, float thetamax);

MPointArray ParaboloidPoints(int numu, int numv, float rmax, float zmin,
				float zmax, float thetamax);

MPointArray DiskPoints(int numu, int numv, float height, float radius,
							float thetamax);

MPointArray TorusPoints(int numu, int numv, float rmajor, float rminor,
				float phimin, float phimax, float thetamax);

#endif  // MAYAPLUGIN_MAYA_PRIMITIVES_H_
