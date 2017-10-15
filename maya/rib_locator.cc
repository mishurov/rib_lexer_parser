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

#include "maya/rib_locator.h"
#include "utils/maya_primitives.h"
#include "utils/primitives.h"


MTypeId RibLocator::id(kRibLocatorID);
MObject RibLocator::file_;
MString	RibLocator::drawDbClassification(kRibLocatorDbClassification);
MString	RibLocator::drawRegistrantId(kRibLocatorRegistrantId);

RibLocator::RibLocator() {}
RibLocator::~RibLocator() {}


void* RibLocator::creator()
{
	return new RibLocator();
}


MStatus RibLocator::initialize()
{ 
	MStatus status;

	MFnTypedAttribute t_attr;
	MFnStringData fn_string_data;
	MObject default_string;
	default_string = fn_string_data.create("Path to a rib file");
	file_ = t_attr.create("file", "file",
				MFnData::kString, default_string);
	t_attr.setStorable(true);
	t_attr.setKeyable(true);
	addAttribute(file_);

	return MS::kSuccess;
}

void RibLocator::postConstructor()
{
	MStatus status;
	MObject node = thisMObject();
	MCallbackId callback_id;
	callback_id = MNodeMessage::addAttributeChangedCallback(node,
						 	attributeChangedCB,
						 	this,
						 	&status);
	CHECK_MSTATUS(status);
	attribute_changed_id_ = callback_id;
}

void RibLocator::updateRibTree(MPlug &plug) {
	rib::ParseError ret;
	rib::Node node;
	
	MString file;
	MString error_msg;
	plug.getValue(file);
	ret = driver_.parseMaya(file.asChar(), &node);

	switch(ret) {
	case rib::BAD_FILE:
		error_msg = "Bad file";
		MGlobal::displayError(error_msg);
		break;
	case rib::PARSE_FAILED:
		error_msg = "Parse failed";
		MGlobal::displayError(error_msg);
		break;
	case rib::SUCCESS:
		driver_.clean(&root_);
		root_ = node;
		break;
	}
}

void RibLocator::attributeChangedCB(MNodeMessage::AttributeMessage msg,
					MPlug &plug, MPlug &otherPlug,
					void* client_data)
{
	if (plug == RibLocator::file_) {
		RibLocator *instance = (RibLocator*) client_data;
		MString file;
		plug.getValue(file);
		instance->updateRibTree(plug);
	}
}

MStatus RibLocator::compute(const MPlug& plug, MDataBlock& data)
{
	return MS::kUnknownParameter;
}

void RibLocator::draw(M3dView & view, const MDagPath & path,
				M3dView::DisplayStyle style,
				M3dView::DisplayStatus status )
{
	view.drawText(MString("The plug-in uses Viewport 2.0 locator draw"),
			MPoint(0.0, 0.0, 0.0),
			M3dView::kCenter);
}


RibLocatorDrawOverride::RibLocatorDrawOverride(const MObject& obj)
: MHWRender::MPxDrawOverride(obj, NULL, false)
{
	on_editor_changed_id_ = MEventMessage::addEventCallback(
		"modelEditorChanged", onModelEditorChanged, this);

	MStatus status;
	MFnDependencyNode node(obj, &status);
	rib_locator_ = status ? dynamic_cast<RibLocator*>(node.userNode()) : NULL;
}

RibLocatorDrawOverride::~RibLocatorDrawOverride()
{
	rib_locator_ = NULL;

	if (on_editor_changed_id_ != 0)
	{
		MMessage::removeCallback(on_editor_changed_id_);
		on_editor_changed_id_ = 0;
	}
}

void RibLocatorDrawOverride::onModelEditorChanged(void *clientData)
{
	RibLocatorDrawOverride *ovr = static_cast<RibLocatorDrawOverride*>(clientData);
	if (ovr && ovr->rib_locator_)
	{
		MHWRender::MRenderer::setGeometryDrawDirty(ovr->rib_locator_->thisMObject());
	}
}

MHWRender::DrawAPI RibLocatorDrawOverride::supportedDrawAPIs() const
{
	return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

void RibLocatorDrawOverride::drawPoints(MHWRender::MUIDrawManager& drawManager,
				 MPointArray& points) {
	for (int i = 0; i < points.length(); i++) {
		points[i] *= basis_.asMatrix();
		if (min_point_.x > points[i].x) min_point_.x = points[i].x;
		if (min_point_.y > points[i].y) min_point_.y = points[i].y;
		if (min_point_.z > points[i].z) min_point_.z = points[i].z;
		if (max_point_.x < points[i].x) max_point_.x = points[i].x;
		if (max_point_.y < points[i].y) max_point_.y = points[i].y;
		if (max_point_.z < points[i].z) max_point_.x = points[i].x;
	}
	drawManager.points(points, false);
}

void RibLocatorDrawOverride::processNode(MHWRender::MUIDrawManager& drawManager,
					rib::Node *node) {
	switch (node->type) {
	case rib::TRANSLATE:
		{
			rib::TranslateNode *n = (rib::TranslateNode *) node;
			MVector translation(n->x, n->y, n->z);
			basis_.addTranslation(translation, MSpace::kObject);
		}
		break;
	case rib::ROTATE:
		{
			rib::RotateNode *n = (rib::RotateNode *) node;
			const double rotation[] = {
				quadrics::radians(n->r * n->x),
				quadrics::radians(n->r * n->y),
				quadrics::radians(n->r * n->z)
			};
			basis_.addRotation(
				rotation,
				basis_.rotationOrder(),
				MSpace::kObject
			);
		}
		break;
	case rib::SCALE:
		{
			rib::ScaleNode *n = (rib::ScaleNode *) node;
			const double scale[] = {n->x, n->y, n->z};
			basis_.addScale(scale, MSpace::kObject);
		}
		break;
	case rib::SPHERE:
		{
			rib::SphereNode *n = (rib::SphereNode *) node;
			MPointArray points = SpherePoints(
				50, 30, n->radius, n->zmin,
				n->zmax, n->thetamax
			);
			drawPoints(drawManager, points);
		}
		break;
	case rib::CONE:
		{
			rib::ConeNode *n = (rib::ConeNode *) node;
			MPointArray points = ConePoints(
				50, 30, n->height, n->radius, n->thetamax
			);
			drawPoints(drawManager, points);
		}
		break;
	case rib::CYLINDER:
		{
			rib::CylinderNode *n = (rib::CylinderNode *) node;
			MPointArray points = CylinderPoints(
				50, 30, n->radius, n->zmin, n->zmax, n->thetamax
			);
			drawPoints(drawManager, points);
		}
		break;
	case rib::HYPERBOLOID:
		{
			rib::HyperboloidNode *n = (rib::HyperboloidNode *) node;
			MPointArray points = HyperboloidPoints(
				60, 60, n->x1, n->y1, n->z1,
				n->x2, n->y2, n->z2, n->thetamax
			);
			drawPoints(drawManager, points);
		}
		break;
	case rib::PARABOLOID:
		{
			rib::ParaboloidNode *n = (rib::ParaboloidNode *) node;
			MPointArray points = ParaboloidPoints(
				60, 60, n->rmax, n->zmin, n->zmax, n->thetamax
			);
			drawPoints(drawManager, points);
		}
		break;
	case rib::DISK:
		{
			rib::DiskNode *n = (rib::DiskNode *) node;
			MPointArray points = DiskPoints(
				40, 40, n->height, n->radius, n->thetamax
			);
			drawPoints(drawManager, points);
		}
		break;
	case rib::TORUS:
		{
			rib::TorusNode *n = (rib::TorusNode *) node;
			MPointArray points = TorusPoints(
				60, 30, n->rmajor, n->rminor,
				n->phimin, n->phimax, n->thetamax
			);
			drawPoints(drawManager, points);
		}
		break;
	case rib::POINTS_GENERAL_POLYGONS:
		{
			rib::PointsGeneralPolygonsNode *n =
				(rib::PointsGeneralPolygonsNode *) node;
			MPointArray points;
			for(int i = 0; i < n->params["P"].size() / 3; i++) {
				MPoint p;
			    	p.x = n->params["P"][i * 3];
			    	p.y = n->params["P"][i * 3 + 1];
			    	p.z = n->params["P"][i * 3 + 2];
				points.append(p);
			}
			drawPoints(drawManager, points);
		}
		break;
	}
}

void RibLocatorDrawOverride::DFS(MHWRender::MUIDrawManager& drawManager,
					rib::Node *node) {
	processNode(drawManager, node);
	for(std::vector<rib::Node *>::iterator it =
	    node->children.begin();
	    it != node->children.end(); ++it) {
		if (it == node->children.begin()) {
			MTransformationMatrix t = basis_;
			transform_stack_.push(t);
		}
		DFS(drawManager, *it);
		if(std::next(it) == node->children.end()) {
			basis_ = transform_stack_.top();
			transform_stack_.pop();
		}
	}
}

void RibLocatorDrawOverride::addUIDrawables(
		const MDagPath& objPath,
		MHWRender::MUIDrawManager& drawManager,
		const MHWRender::MFrameContext& frameContext,
		const MUserData* data)
{

	unsigned int displayStyle = frameContext.getDisplayStyle();
	bool filled = !(displayStyle & MHWRender::MFrameContext::kWireFrame);

	drawManager.beginDrawable();

	MColor color = MHWRender::MGeometryUtilities::wireframeColor(objPath);
	drawManager.setColor(color);
	
	drawManager.setPaintStyle(MHWRender::MUIDrawManager::kFlat);
	drawManager.setPointSize(1);
	
	MVector translation(0, 0, 0);
	basis_.setTranslation(translation, MSpace::kWorld);
	double rotation[] = {0, 0, 0};
	basis_.setRotation(
		rotation,
		basis_.rotationOrder(),
		MSpace::kWorld
	);
	double scale[] = {1, 1, 1};
	basis_.setScale(scale, MSpace::kWorld);

	DFS(drawManager, &rib_locator_->root_);

	drawManager.endDrawable();
}


