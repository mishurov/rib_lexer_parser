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

#include <fstream>
#include "rib_driver.h"

using namespace rib;

Driver::~Driver()
{
	delete(lexer);
	lexer = nullptr;
	delete(parser);
	parser = nullptr;
}

Node Driver::parse(const char * const filename)
{
	printf("Parsing...\n");
	std::ifstream in_file(filename);
	if (!in_file.good()) {
		printf("The file is bad\n");
		exit( EXIT_FAILURE );
	}

	delete lexer ;
	lexer = new Lexer(&in_file);

	delete parser;
	parser = new Parser((*lexer), (*this));

	current = &root;
	
	const int accept = 0;
	if (parser->parse() != accept) {
		printf("Parse failed\n");
	}
	return root;
}

ParseError Driver::parseMaya(const char * const filename, Node *node)
{
	std::ifstream in_file(filename);
	if (!in_file.good()) {
		return kBadFile;
	}

	delete lexer ;
	lexer = new Lexer(&in_file);

	delete parser;
	parser = new Parser((*lexer), (*this));

	current = node;
	
	const int accept = 0;
	if (parser->parse() != accept) {
		return kParseFailed;
	}
	
	return kSuccess;
}

void Driver::clean(Node *node)
{
	for(std::vector<Node *>::iterator it = node->children.begin();
	    it != node->children.end();
	    ++it) {
		clean(*it);
		delete *it;
	}
}

void Driver::addNode()
{
	Node *node = new Node;
	node->parent = current;
	current->children.push_back(node);
	current = current->children.back();
}

void Driver::selectParent()
{
	current = current->parent;
}

void Driver::addTranslate(const float x, const float y, const float z)
{
	TranslateNode *node = new TranslateNode(current, x, y, z);
	if (current->parent != nullptr)
		current->children.push_back(node);
}

void Driver::addRotate(const int x, const int y, const int z, const int w)
{
	RotateNode *node = new RotateNode(current, x, y, z, w);
	if (current->parent != nullptr)
		current->children.push_back(node);
}

void Driver::addScale(const float x, const float y, const float z)
{
	ScaleNode *node = new ScaleNode(current, x, y, z);
	if (current->parent != nullptr)
		current->children.push_back(node);
}

void Driver::addConcatTransform(const std::vector<float> matrix)
{
	ConcatTransformNode *node = new ConcatTransformNode(current, matrix);
	if (current->parent != nullptr)
		current->children.push_back(node);
}

void Driver::addHyperboloid(const float x1, const float y1, const float z1,
			const float x2, const float y2, const float z2,
			const float thetamax)
{
	HyperboloidNode *node = new HyperboloidNode(
		current, x1, y1, z1, x2, y2, z2, thetamax);
	current->children.push_back(node);
}

void Driver::addParaboloid(const float rmax, const float zmin,
				const float zmax, const float thetamax)
{
	ParaboloidNode *node = new ParaboloidNode(current,
						rmax, zmin, zmax, thetamax);
	current->children.push_back(node);
}

void Driver::addTorus(const float rmajor, const float rminor,
			const float phimin, const float phimax,
			const float thetamax)
{
	TorusNode *node = new TorusNode(current, rmajor, rminor,
						phimin, phimax, thetamax);
	current->children.push_back(node);
}

void Driver::addCylinder(const float radius, const float zmin,
				const float zmax, const float thetamax)
{
	CylinderNode *node = new CylinderNode(current, radius,
						zmin, zmax, thetamax);
	current->children.push_back(node);
}

void Driver::addSphere(const float radius, const float zmin,
			const float zmax, const float thetamax)
{
	SphereNode *node = new SphereNode(current, radius,
					zmin, zmax, thetamax);
	current->children.push_back(node);
}

void Driver::addDisk(const float height, const float radius,
			const float thetamax)
{
	DiskNode *node = new DiskNode(current, height, radius, thetamax);
	current->children.push_back(node);
}

void Driver::addCone(const float height, const float radius,
			const float thetamax)
{
	ConeNode *node = new ConeNode(current, height, radius, thetamax);
	current->children.push_back(node);
}

void Driver::addPGP(std::vector<int> nloops, std::vector<int> nvertices,
			std::vector<int> vertices)
{
	PointsGeneralPolygonsNode *node = 
		new PointsGeneralPolygonsNode(current, nloops,
						nvertices, vertices);
	current->children.push_back(node);
}

void Driver::addPGPparam(const std::string &key, std::vector<float> value) {
	if (current->children.back()->type == kPointsGeneralPolygons) {
		PointsGeneralPolygonsNode *node =
			(PointsGeneralPolygonsNode *) current->children.back();
		node->params.insert({key, value});
	}
}

void Driver::addPP(std::vector<int> nvertices, std::vector<int> vertices)
{
	PointsPolygonsNode *node = 
		new PointsPolygonsNode(current, nvertices, vertices);
	current->children.push_back(node);
}

void Driver::addPPparam(const std::string &key, std::vector<float> value) {
	if (current->children.back()->type == kPointsPolygons) {
		PointsPolygonsNode *node =
			(PointsPolygonsNode *) current->children.back();
		node->params.insert({key, value});
	}
}

void AttributeNode::addStringParam(const std::string &key,
				std::vector<std::string> value) {
	string_params.insert({key, value});
}

void Driver::addAttribute(std::string name)
{
	AttributeNode *node = new AttributeNode(current, "", name);
	current->children.push_back(node);
}

void Driver::addAttrFlParam(const std::string &key, std::vector<float> value)
{
	if (current->children.back()->type == kAttribute) {
		AttributeNode *node =
				(AttributeNode *) current->children.back();
		node->addFloatParam(key, value);
	}
}

void AttributeNode::addFloatParam(const std::string &key,
				std::vector<float> value) {
	float_params.insert({key, value});
}

void Driver::addPattern(std::string item_type, std::string name)
{
	PatternNode *node = new PatternNode(current, item_type, name);
	current->children.push_back(node);
}

void Driver::addBxdf(std::string item_type, std::string name)
{
	BxdfNode *node = new BxdfNode(current, item_type, name);
	current->children.push_back(node);
}

void Driver::addLight(std::string item_type, std::string name)
{
	LightNode *node = new LightNode(current, item_type, name);
	current->children.push_back(node);
}


void Driver::addPatternStrParam(const std::string &key,
				std::vector<std::string> value) {
	if (current->children.back()->type == kPattern) {
		PatternNode *node = (PatternNode *) current->children.back();
		node->addStringParam(key, value);
	}
}

void Driver::addPatternFlParam(const std::string &key,
				std::vector<float> value) {
	if (current->children.back()->type == kPattern) {
		PatternNode *node = (PatternNode *) current->children.back();
		node->addFloatParam(key, value);
	}
}

void Driver::addBxdfStrParam(const std::string &key,
				std::vector<std::string> value) {
	if (current->children.back()->type == kBxdf) {
		BxdfNode *node = (BxdfNode *) current->children.back();
		node->addStringParam(key, value);
	}
}

void Driver::addBxdfFlParam(const std::string &key,
				std::vector<float> value) {
	if (current->children.back()->type == kBxdf) {
		BxdfNode *node = (BxdfNode *) current->children.back();
		node->addFloatParam(key, value);
	}
}

void Driver::addLightStrParam(const std::string &key,
				std::vector<std::string> value) {
	if (current->children.back()->type == kLight) {
		LightNode *node = (LightNode *) current->children.back();
		node->addStringParam(key, value);
	}
}

void Driver::addLightFlParam(const std::string &key,
				std::vector<float> value) {
	if (current->children.back()->type == kLight) {
		LightNode *node = (LightNode *) current->children.back();
		node->addFloatParam(key, value);
	}
}

