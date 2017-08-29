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

#include <maya/MFnPlugin.h>
#include "rib_locator.h"

MStatus initializePlugin(MObject obj)
{ 
	MStatus   status;
	MFnPlugin plugin(obj, "Rib locator plugin", "0.1", "Any");

	status = plugin.registerNode("ribLocator",
				RibLocator::id, 
				&RibLocator::creator,
				&RibLocator::initialize,
				MPxNode::kLocatorNode,
				&RibLocator::drawDbClassification);
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
		RibLocator::drawDbClassification,
		RibLocator::drawRegistrantId,
		RibLocatorDrawOverride::Creator);
	if (!status) {
		status.perror("registerDrawOverrideCreator");
		return status;
	}
	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj);

	status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(
		RibLocator::drawDbClassification,
		RibLocator::drawRegistrantId);
	if (!status) {
		status.perror("deregisterDrawOverrideCreator");
		return status;
	}

	status = plugin.deregisterNode(RibLocator::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}

