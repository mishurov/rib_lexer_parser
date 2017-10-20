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

#include <iostream>
#include <cstdlib>
#include <cstring>
#include "parser/rib_driver.h"

void dfs(const rib::Node *node) {
	switch (node->type) {
	case rib::kJoint:
		printf("Joint node\n");
		break;
	case rib::kTranslate:
		printf("Translate node\n");
		break;
	case rib::kRotate:
		printf("Rotate node\n");
		break;
	case rib::kScale:
		printf("Scale node\n");
		break;
	case rib::kHyperboloid:
		printf("Hyperboloid node\n");
		break;
	case rib::kParaboloid:
		printf("Paraboloid node\n");
		break;
	case rib::kTorus:
		printf("Torus node\n");
		break;
	case rib::kCylinder:
		printf("Cylinder node\n");
		break;
	case rib::kSphere:
		printf("Sphere node\n");
		break;
	case rib::kDisk:
		printf("Disk node\n");
		break;
	case rib::kCone:
		printf("Cone node\n");
		break;
	case rib::kPattern:
		printf("Pattern node\n");
		break;
	case rib::kBxdf:
		printf("Bxdf node\n");
		break;
	case rib::kLight:
		printf("Light node\n");
		break;
	case rib::kPointsGeneralPolygons:
		printf("Polygons node\n");
		rib::PointsGeneralPolygonsNode *pnode =
			(rib::PointsGeneralPolygonsNode *) node;
		for(std::vector<int>::iterator
		    it = pnode->vertices.begin();
		    it != pnode->vertices.end();
		    ++it) {
			printf("Vertices attribute %i\n", *it);
		}
		for(std::vector<float>::iterator
		    it = pnode->params["P"].begin();
		    it != pnode->params["P"].end();
		    ++it) {
			printf("P parameter %f\n", *it);
		}
		break;
	}
	for(std::vector<rib::Node *>::const_iterator it =
	    node->children.begin();
	    it != node->children.end();
	    ++it) {
	    	dfs(*it);
	}
}

int main(const int argc, const char **argv)
{
	rib::Driver driver;
	rib::Node root = driver.parse(argv[1]);
	dfs(&root);
	return(EXIT_SUCCESS);
}
