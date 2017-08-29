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

#ifndef MAYAPLUGIN_RIBDRIVER_H_
#define MAYAPLUGIN_RIBDRIVER_H_

#include <istream>
#include <vector>
#include <map>
#include "rib_lexer.h"
#include "rib_parser.tab.hh"

namespace rib {

enum ParseError {
	BAD_FILE,
	PARSE_FAILED,
	SUCCESS
};

enum NodeType {
	JOINT,
	TRANSLATE,
	ROTATE,
	SCALE,
	HYPERBOLOID,
	PARABOLOID,
	TORUS,
	CYLINDER,
	SPHERE,
	DISK,
	CONE,
	POINTS_GENERAL_POLYGONS
};

class Node {
public:
	std::vector<Node *> children;
	Node *parent = nullptr;
	NodeType type;

public:
	Node() = default;
	Node(Node *parent) : parent(parent) { type = JOINT; }
	~Node() {}
};

class TranslateNode : public Node {
public:
	float x;
	float y;
	float z;
public:
	TranslateNode(Node *parent, float x, float y, float z)
	: Node(parent), x(x), y(y), z(z) { type = TRANSLATE; }
	~TranslateNode() {}
};

class RotateNode : public Node {
public:
	int r;
	int x;
	int y;
	int z;
public:
	RotateNode(Node *parent, int r, int x, int y, int z)
	: Node(parent), r(r), x(x), y(y), z(z) { type = ROTATE; }
	~RotateNode() {}
};

class ScaleNode : public Node {
public:
	float x;
	float y;
	float z;
public:
	ScaleNode(Node *parent, float x, float y, float z)
	: Node(parent), x(x), y(y), z(z) { type = SCALE; }
	~ScaleNode() {}
};

/* quadrics */

class HyperboloidNode : public Node {
public:
	float x1;
	float y1;
	float z1;
	float x2;
	float y2;
	float z2;
	float thetamax;
public:
	HyperboloidNode(Node *parent, float x1, float y1, float z1,
			float x2, float y2, float z2, float thetamax)
	: Node(parent), x1(x1), y1(y1), z1(z1),
			x2(x2), y2(y2), z2(z2), thetamax(thetamax)
		{ type = HYPERBOLOID; }
	~HyperboloidNode() {}
};

class ParaboloidNode : public Node {
public:
	float rmax;
	float zmin;
	float zmax;
	float thetamax;
public:
	ParaboloidNode(Node *parent, float rmax, float zmin,
					float zmax, float thetamax)
	: Node(parent), rmax(rmax), zmin(zmin), zmax(zmax),
				thetamax(thetamax) { type = PARABOLOID; }
	~ParaboloidNode() {}
};

class TorusNode : public Node {
public:
	float rmajor;
	float rminor;
	float phimin;
	float phimax;
	float thetamax;
public:
	TorusNode(Node *parent, float rmajor, float rminor,
				float phimin, float phimax, float thetamax)
	: Node(parent), rmajor(rmajor), rminor(rminor),
			phimin(phimin), phimax(phimax), thetamax(thetamax)
		{ type = TORUS; }
	~TorusNode() {}
};


class CylinderNode : public Node {
public:
	float radius;
	float zmin;
	float zmax;
	float thetamax;
public:
	CylinderNode(Node *parent, float radius, float zmin,
				float zmax, float thetamax)
	: Node(parent), radius(radius), zmin(zmin),
			zmax(zmax), thetamax(thetamax)
		{ type = CYLINDER; }
	~CylinderNode() {}
};

class SphereNode : public Node {
public:
	float radius;
	float zmin;
	float zmax;
	float thetamax;
public:
	SphereNode(Node *parent, float radius, float zmin,
				float zmax, float thetamax)
	: Node(parent), radius(radius), zmin(zmin),
			zmax(zmax), thetamax(thetamax)
		{ type = SPHERE; }
	~SphereNode() {}
};

class DiskNode : public Node {
public:
	float height;
	float radius;
	float thetamax;
public:
	DiskNode(Node *parent, float height, float radius, float thetamax)
	: Node(parent), height(height), radius(radius), thetamax(thetamax)
		{ type = DISK; }
	~DiskNode() {}
};

class ConeNode : public Node {
public:
	float height;
	float radius;
	float thetamax;
public:
	ConeNode(Node *parent, float height, float radius, float thetamax)
	: Node(parent), height(height), radius(radius), thetamax(thetamax)
		{ type = CONE; }
	~ConeNode() {}
};

class PointsGeneralPolygonsNode : public Node {
public:
	std::vector<int> nloops;
	std::vector<int> nvertices;
	std::vector<int> vertices;
	std::map<std::string,std::vector<float>> params;
public:
	PointsGeneralPolygonsNode(Node *parent, std::vector<int> nloops,
			std::vector<int> nvertices, std::vector<int> vertices)
	: Node(parent), nloops(nloops), nvertices(nvertices),
			vertices(vertices) { type = POINTS_GENERAL_POLYGONS; }
	~PointsGeneralPolygonsNode() {}
};

class Driver {
public:
	Parser *parser = nullptr;
	Lexer *lexer = nullptr;
	Node root;
	Node *current = nullptr;
public:
	Driver() = default;
	virtual ~Driver();
	
	Node parse(const char * const filename);
	ParseError parseMaya(const char * const filename, Node *node);
	void clean(Node *node);
	// hierarchy
	void addNode();
	void selectParent();
	// transforms
	void addTranslate(const float x, const float y, const float z);
	void addRotate(const int r, const int x, const int y, const int z);
	void addScale(const float x, const float y, const float z);
	// quadrics
	void addHyperboloid(const float x1, const float y1, const float z1,
			const float x2, const float y2, const float z2,
			const float thetamax);
	void addParaboloid(const float rmax, const float zmin,
			const float zmax, const float thetamax);
	void addTorus(const float rmajor, const float rminor,
		const float phimin, const float phimax,
		const float thetamax);
	void addCylinder(const float radius, const float zmin,
			const float zmax, const float thetamax);
	void addSphere(const float radius, const float zmin,
		const float zmax, const float thetamax);
	void addDisk(const float height, const float radius,
		const float thetamax);
	void addCone(const float height, const float radius,
		const float thetamax);
	// primitives
	void addPGP(std::vector<int> nloops, std::vector<int> nvertices,
			std::vector<int> vertices);
	void addPGPparam(const std::string &key, std::vector<float> value);
};

} /* namespace rib */

#endif  // MAYAPLUGIN_RIBDRIVER_H_
