#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>

#include "lambertnode.h"
#include "lambertshader.h"
#include "mirrormaterialnode.h"
#include "mirrorshader.h"

#include "renderglobalsnode.h"
#include "finalrendercommand.h"

#include "filianore/color/spectruminits.h"

#include "util.h"

MStatus initializePlugin(MObject plugin)
{
	// Filianore Inits
	filianore::InitializeSpectrals();

	MStatus status;
	MFnPlugin fnPlugin(plugin, FILIANORE_MAYA_NAME, FILIANORE_MAYA_VERSION);

	// Rendering Nodes
	status = fnPlugin.registerNode(RenderGlobalsNode::name,
								   RenderGlobalsNode::id, RenderGlobalsNode::creator, RenderGlobalsNode::initialize);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error registering RenderGlobalNode.");

	// Shading Nodes
	MString sDrawLambertDBClassification("drawdb/shader/surface/" + LambertNode::name);
	MString sLambertFullClassification("shader/surface:" + sDrawLambertDBClassification);
	fnPlugin.registerNode(LambertNode::name, LambertNode::id, LambertNode::creator, LambertNode::initialize, MPxNode::kDependNode, &sLambertFullClassification);
	MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(sDrawLambertDBClassification, LambertNode::name, LambertShader::creator);

	MString sDrawMirrorMaterialDBClassification("drawdb/shader/surface/" + MirrorMaterialNode::name);
	MString sMirrorMaterialFullClassification("shader/surface:" + sDrawMirrorMaterialDBClassification);
	fnPlugin.registerNode(MirrorMaterialNode::name, MirrorMaterialNode::id, MirrorMaterialNode::creator, MirrorMaterialNode::initialize, MPxNode::kDependNode, &sMirrorMaterialFullClassification);
	MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(sDrawMirrorMaterialDBClassification, MirrorMaterialNode::name, MirrorShader::creator);

	// Commands
	status = fnPlugin.registerCommand(FinalRenderCommand::commandName, FinalRenderCommand::creator);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error registering Final Render command.");

	// Scripts
	status = MGlobal::executePythonCommand("import register; register.register()", false, false);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error initializing rendering engine.");

	FILIANORE_MAYA_LOG_INFO("Renderer Plugin registered successfully.");

	return status;
}

MStatus uninitializePlugin(MObject plugin)
{
	MStatus status;
	MFnPlugin fnPlugin(plugin);

	// Rendering Nodes
	RenderGlobalsNode::clean();
	status = fnPlugin.deregisterNode(RenderGlobalsNode::id);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error deregistering RenderGlobalNode.");

	// Shading Nodes
	MString sDrawLambertDBClassification("drawdb/shader/surface/" + LambertNode::name);
	fnPlugin.deregisterNode(LambertNode::id);
	MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(sDrawLambertDBClassification, LambertNode::name);

	MString sDrawMirrorMaterialDBClassification("drawdb/shader/surface/" + MirrorMaterialNode::name);
	fnPlugin.deregisterNode(MirrorMaterialNode::id);
	MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(sDrawMirrorMaterialDBClassification, MirrorMaterialNode::name);

	// Commands
	status = fnPlugin.deregisterCommand(FinalRenderCommand::commandName);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error deregistering Final Render command.");

	// Scripts
	status = MGlobal::executePythonCommand("import register; register.unregister()", false, false);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error uninitializing rendering engine.");

	FILIANORE_MAYA_LOG_INFO("Renderer Plugin deregistered successfully.");

	return status;
}