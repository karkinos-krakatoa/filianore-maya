#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>

#include "matteshadernode.h"
#include "matteshader.h"

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
	MString sDrawMatteDBClassification("drawdb/shader/surface/" + MatteShaderNode::name);
	MString sMatteFullClassification("shader/surface:" + sDrawMatteDBClassification);
	fnPlugin.registerNode(MatteShaderNode::name, MatteShaderNode::id, MatteShaderNode::creator, MatteShaderNode::initialize, MPxNode::kDependNode, &sMatteFullClassification);
	MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(sDrawMatteDBClassification, MatteShaderNode::name, MatteShader::creator);

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
	MString sDrawMatteDBClassification("drawdb/shader/surface/" + MatteShaderNode::name);
	fnPlugin.deregisterNode(MatteShaderNode::id);
	MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(sDrawMatteDBClassification, MatteShaderNode::name);

	// Commands
	status = fnPlugin.deregisterCommand(FinalRenderCommand::commandName);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error deregistering Final Render command.");

	// Scripts
	status = MGlobal::executePythonCommand("import register; register.unregister()", false, false);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error uninitializing rendering engine.");

	FILIANORE_MAYA_LOG_INFO("Renderer Plugin deregistered successfully.");

	return status;
}