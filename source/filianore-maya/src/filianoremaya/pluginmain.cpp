#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>

#include "flstandardsurfaceshaderoverride.h"
#include "flstandardsurfaceshader.h"

#include "plasticshadernode.h"
#include "plasticshader.h"

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
	MString sDrawFlStandardSurfaceDBClassification("drawdb/shader/surface/" + FlStandardSurfaceShader::name);
	MString sFlStandardSurfaceFullClassification("shader/surface:" + sDrawFlStandardSurfaceDBClassification);
	fnPlugin.registerNode(FlStandardSurfaceShader::name, FlStandardSurfaceShader::id, FlStandardSurfaceShader::creator, FlStandardSurfaceShader::initialize, MPxNode::kDependNode, &sFlStandardSurfaceFullClassification);
	MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(sDrawFlStandardSurfaceDBClassification, FlStandardSurfaceShader::name, FlStandardSurfaceShaderOverride::creator);

	MString sDrawPlasticShaderDBClassification("drawdb/shader/surface/" + PlasticShaderNode::name);
	MString sPlasticShaderFullClassification("shader/surface:" + sDrawPlasticShaderDBClassification);
	fnPlugin.registerNode(PlasticShaderNode::name, PlasticShaderNode::id, PlasticShaderNode::creator, PlasticShaderNode::initialize, MPxNode::kDependNode, &sPlasticShaderFullClassification);
	MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(sDrawPlasticShaderDBClassification, PlasticShaderNode::name, PlasticShader::creator);

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
	MString sDrawFlStandardSurfaceDBClassification("drawdb/shader/surface/" + FlStandardSurfaceShader::name);
	fnPlugin.deregisterNode(FlStandardSurfaceShader::id);
	MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(sDrawFlStandardSurfaceDBClassification, FlStandardSurfaceShader::name);

	MString sDrawPlasticShaderDBClassification("drawdb/shader/surface/" + PlasticShaderNode::name);
	fnPlugin.deregisterNode(PlasticShaderNode::id);
	MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(sDrawPlasticShaderDBClassification, PlasticShaderNode::name);

	// Commands
	status = fnPlugin.deregisterCommand(FinalRenderCommand::commandName);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error deregistering Final Render command.");

	// Scripts
	status = MGlobal::executePythonCommand("import register; register.unregister()", false, false);
	FILIANORE_MAYA_CHECK_MSTATUS_MSG(status, "Error uninitializing rendering engine.");

	FILIANORE_MAYA_LOG_INFO("Renderer Plugin deregistered successfully.");

	return status;
}