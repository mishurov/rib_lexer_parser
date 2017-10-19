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
	case rib::JOINT:
		printf("Joint node\n");
		break;
	case rib::TRANSLATE:
		printf("Translate node\n");
		break;
	case rib::ROTATE:
		printf("Rotate node\n");
		break;
	case rib::SCALE:
		printf("Scale node\n");
		break;
	case rib::HYPERBOLOID:
		printf("Hyperboloid node\n");
		break;
	case rib::PARABOLOID:
		printf("Paraboloid node\n");
		break;
	case rib::TORUS:
		printf("Torus node\n");
		break;
	case rib::CYLINDER:
		printf("Cylinder node\n");
		break;
	case rib::SPHERE:
		printf("Sphere node\n");
		break;
	case rib::DISK:
		printf("Disk node\n");
		break;
	case rib::CONE:
		printf("Cone node\n");
		break;
	case rib::PATTERN:
		printf("Pattern node\n");
		break;
	case rib::BXDF:
		printf("Bxdf node\n");
		break;
	case rib::LIGHT:
		printf("Light node\n");
		break;
	case rib::POINTS_GENERAL_POLYGONS:
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
