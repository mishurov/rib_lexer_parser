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

#include "maya_primitives.h"
#include "primitives.h"

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

MPointArray SpherePoints(int numu, int numv, float radius, float zmin,
				float zmax, float thetamax) {
	MPointArray ret;
	float args[] = { radius, zmin, zmax, thetamax };
	PopulatePointArray(&ret, numu, numv,
				quadrics::SpherePoint<MPoint>, args);
	return ret;
}

MPointArray ConePoints(int numu, int numv, float height, float radius,
							float thetamax) {
	MPointArray ret;
	float args[] = { height, radius, thetamax };
	PopulatePointArray(&ret, numu, numv, quadrics::ConePoint<MPoint>, args);
	return ret;
}

MPointArray CylinderPoints(int numu, int numv, float radius, float zmin,
				float zmax, float thetamax) {
	MPointArray ret;
	float args[] = { radius, zmin, zmax, thetamax };
	PopulatePointArray(&ret, numu, numv,
				quadrics::CylinderPoint<MPoint>, args);
	return ret;
}

MPointArray HyperboloidPoints(int numu, int numv, float x1, float y1, float z1,
				float x2, float y2, float z2, float thetamax) {
	MPointArray ret;
	float args[] = { x1, y1, z1, x2, y2, z2, thetamax };
	PopulatePointArray(&ret, numu, numv,
				quadrics::HyperboloidPoint<MPoint>, args);
	return ret;
}

MPointArray ParaboloidPoints(int numu, int numv, float rmax, float zmin,
				float zmax, float thetamax) {
	MPointArray ret;
	float args[] = { rmax, zmin, zmax, thetamax };
	PopulatePointArray(&ret, numu, numv,
				quadrics::ParaboloidPoint<MPoint>, args);
	return ret;
}

MPointArray DiskPoints(int numu, int numv, float height, float radius,
							float thetamax) {
	MPointArray ret;
	float args[] = { height, radius, thetamax };
	PopulatePointArray(&ret, numu, numv,
				quadrics::DiskPoint<MPoint>, args);
	return ret;
}

MPointArray TorusPoints(int numu, int numv, float rmajor, float rminor,
				float phimin, float phimax, float thetamax) {
	MPointArray ret;
	float args[] = { rmajor, rminor, phimin, phimax, thetamax };
	PopulatePointArray(&ret, numu, numv,
				quadrics::TorusPoint<MPoint>, args);
	return ret;
}

