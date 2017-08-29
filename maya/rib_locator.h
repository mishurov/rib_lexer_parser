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

#ifndef MAYAPLUGIN_RIBLOCATOR_H_
#define MAYAPLUGIN_RIBLOCATOR_H_

#include <maya/MGlobal.h>
#include <maya/MString.h> 
#include <maya/MPxLocatorNode.h>
#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/M3dView.h>
#include <maya/MFnData.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h> 
#include <maya/MFnTypedAttribute.h> 
#include <maya/MFnNumericAttribute.h> 
#include <maya/MFnStringData.h>
#include <maya/MNodeMessage.h>

#include <maya/MDrawRegistry.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>
#include <maya/MDrawContext.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MEventMessage.h>
#include <maya/MFnDependencyNode.h>

#include <stack>
#include "parser/rib_driver.h"

#define kRibLocatorID 0x8000C
#define kRibLocatorDbClassification "drawdb/geometry/ribLocator"
#define kRibLocatorRegistrantId "RibLocatorPlugin"


class RibLocator : public MPxLocatorNode {
public:
	RibLocator();
	virtual ~RibLocator();
	virtual void postConstructor();
	virtual MStatus compute( const MPlug&, MDataBlock& );
	virtual void draw( M3dView & view, const MDagPath & path, 
					   M3dView::DisplayStyle style,
					   M3dView::DisplayStatus status );
	static void* creator();
	static MStatus initialize();
public:
	static MTypeId id;
	static MString drawDbClassification;
	static MString drawRegistrantId;
	static MObject file_;
	rib::Node root_;
private:
 	static void attributeChangedCB(MNodeMessage::AttributeMessage msg,
					MPlug &plug, MPlug &otherPlug, void*);
	void updateRibTree(MPlug &plug);
private:
	rib::Driver driver_;
	int attribute_changed_id_;
};

class RibLocatorData : public MUserData
{
public:
	RibLocatorData() : MUserData(false) {}
	virtual ~RibLocatorData() {}
};

class RibLocatorDrawOverride : public MHWRender::MPxDrawOverride
{
public:
	static MHWRender::MPxDrawOverride* Creator(const MObject& obj)
	{
		return new RibLocatorDrawOverride(obj);
	}

	virtual ~RibLocatorDrawOverride();

	virtual MHWRender::DrawAPI supportedDrawAPIs() const;

	virtual MUserData* prepareForDraw(
		const MDagPath& objPath,
		const MDagPath& cameraPath,
		const MHWRender::MFrameContext& frameContext,
		MUserData* oldData)
	{
		return oldData;
	}

	virtual bool hasUIDrawables() const { return true; }

	virtual void addUIDrawables(
		const MDagPath& objPath,
		MHWRender::MUIDrawManager& drawManager,
		const MHWRender::MFrameContext& frameContext,
		const MUserData* data);

	virtual bool traceCallSequence() const
	{
		return false;
	}

	virtual bool isBounded() const
	{
		return true;
	}
	virtual MBoundingBox boundingBox() const
	{
		return MBoundingBox(min_point_, max_point_);
	}
	virtual void handleTraceMessage( const MString &message ) const
	{
		MGlobal::displayInfo("RibLocatorDrawOverride: " + message);
		fprintf(stderr, "RibLocatorDrawOverride: ");
		fprintf(stderr, message.asChar());
		fprintf(stderr, "\n");
	}

private:
	RibLocatorDrawOverride(const MObject& obj);
	static void onModelEditorChanged(void *clientData);
	void DFS(MHWRender::MUIDrawManager& drawManager, rib::Node *root);
	void processNode(MHWRender::MUIDrawManager& drawManager,
				rib::Node *node);
	void drawPoints(MHWRender::MUIDrawManager& drawManager,
				 MPointArray& points);

	MTransformationMatrix basis_;
	std::stack<MTransformationMatrix> transform_stack_;
	RibLocator*  rib_locator_;
	MCallbackId on_editor_changed_id_;

	MPoint min_point_;
	MPoint max_point_;
};

#endif // MAYAPLUGIN_RIBLOCATOR_H_
